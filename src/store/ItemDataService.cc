/**
 * @project zapdos
 * @file src/store/ItemDataService.cc
 * @author  S Roychowdhury < sroycode at gmail dot com >
 * @version 1.0.0
 *
 * @section LICENSE
 *
 * Copyright (c) 2018-2020 S Roychowdhury
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * @section DESCRIPTION
 *
 *  ItemDataService.cc : ItemData Manipulation impl
 *
 */
#include <unordered_set>

#include "store/ItemDataService.hpp"
#include "store/UserCredService.hpp"

#include "store/TempNameCache.hpp"
#include "store/StoreTrans.hpp"

#include "utils/SplitWith.hpp"
#include "utils/PrintWith.hpp"

const ::zpds::store::UserCredService ucserv;

/**
* GetBlindByPrimaryKey: get an arbitrary record by a primary key
*
*/
bool zpds::store::ItemDataService::GetBlindByPrimaryKey(
    ::zpds::utils::SharedTable::pointer stptr,
    ::zpds::store::ItemDataT* record, ::zpds::store::KeyTypeE keytype)
{
	std::string value;
	usemydb::Status s ;
	auto dbptr = stptr->maindb.Get();
	s = dbptr->Get(usemydb::ReadOptions(), EncodePrimaryKey(keytype,record->id()), &value);
	if ((!s.ok()) || s.IsNotFound()) {
		record->set_notfound(true);
		return false;
	}

	if (!record->ParseFromString(value))
		throw zpds::BadDataException("Record cannot be parsed");

	return true;
}

/**
* HandleAnother: handle parent
*
*/
bool zpds::store::ItemDataService::HandleAnother(
    ::zpds::utils::SharedTable::pointer stptr,
    ::zpds::store::ItemDataT* idata,
    ::zpds::store::ItemDataT* odata,
    bool merge)
{
	if ( idata->unique_id().empty() ) {
		// reset old parent
		if ( !(odata->id()==0 || merge) ) {
			odata->set_id(0);
			return true;
		}
	}
	else {
		if ( ! GetBlindBySecondaryKey(stptr, idata, idata->keytype(),true) )
			throw ::zpds::BadDataException("Parent Relation does not exist");
		// if id same dont change
		if (idata->id() == odata->id()) return false;
		odata->set_id(idata->id());
		return true;
	}
	return false;
}

/**
* GetBlindBySecondaryKey: get an arbitrary record by a secondary key
*
*/
bool zpds::store::ItemDataService::GetBlindBySecondaryKey(
    ::zpds::utils::SharedTable::pointer stptr,
    ::zpds::store::ItemDataT* record, ::zpds::store::KeyTypeE keytype, bool idonly)
{
	std::string value;
	usemydb::Status s ;
	if (record->unique_id().empty()) return false;
	std::string&& skey = EncodeSecondaryKey<std::string>( GetUniqueByPrimary(keytype), record->unique_id() );
	auto dbptr = stptr->maindb.Get();
	s = dbptr->Get(usemydb::ReadOptions(), skey, &value);
	if ((!s.ok()) || s.IsNotFound()) return false;
	NodeT node;
	if ( ! node.ParseFromString(value) ) return false; // dont throw here
	if (node.id()==0) return false;
	if (node.keytype()==keytype) return false;
	// set the id
	record->set_id( node.id() );
	return (idonly) ? true : GetBlindByPrimaryKey(stptr,record,keytype);
}

/**
 * ManageDataAction : sets itemdata common for all children
 *
 */
void zpds::store::ItemDataService::ManageDataAction(::zpds::utils::SharedTable::pointer stptr, ::zpds::query::ItemDataRespT* resp)
{
	if (!stptr->is_master.Get())
		throw ::zpds::BadCodeException("ItemData change is master only");

	auto status = resp->mutable_status();
	int32_t action=0;
	bool merge = false;
	std::tie(action,merge)=WriteActionFilter(resp->action());
	int fields = AllowedFieldsFilter(resp->fields());

	::zpds::store::ExterDataT updater;
	updater.set_name( resp->username() );
	updater.set_sessionkey( resp->sessionkey() );
	updater.set_keytype( ::zpds::store::K_EXTERDATA );

	DLOG(INFO) << std::endl << resp->DebugString() << std::endl;

	if (! CheckSession(stptr, &updater, true) )
		throw zpds::BadDataException("Invalid user login or Invalid session",M_INVALID_PARAM);

	// just add the payload to payloads for easy processing
	if (!resp->payload().unique_id().empty())
		resp->add_payloads()->Swap(resp->mutable_payload());
	status->set_inputcount( resp->payloads_size() );

	// start addition , first format the data , then update
	uint64_t currtime = ZPDS_CURRTIME_MS;

	zpds::store::TransactionT trans;
	::zpds::store::TempNameCache namecache{stptr};

	// update payloads if exists
	for (size_t i = 0 ; i<resp->payloads_size(); ++i)	{
		auto rdata = resp->mutable_payloads(i);
		if ( rdata->unique_id().empty() )
			throw zpds::BadDataException("unique_id cannot be empty",M_INVALID_PARAM);

		bool item_found = PrepareData(stptr,&updater,rdata,action,fields,merge);

		// reserve this unique_id
		if (namecache.CheckLocal(GetKeyType(),rdata->unique_id()))
			throw zpds::BadDataException("Duplicate unique_id found: " + rdata->unique_id(),M_INVALID_PARAM);
		if (!namecache.ReserveName(GetKeyType(),rdata->unique_id(),false))
			throw zpds::BadDataException("This unique_id is being worked on: " + rdata->unique_id(),M_INVALID_PARAM);

		if (! item_found) {
			rdata->set_manager( updater.name() );
			rdata->set_id(stptr->maincounter.GetNext() );
			rdata->set_created_at( currtime );
		}

		rdata->set_updated_at( currtime );
		status->set_updatecount( status->updatecount() + 1 );

		// add rdata
		if (!AddOneRecord(stptr,rdata,&trans,item_found))
			throw ::zpds::BadDataException("Cannot Insert item data");
	}

	// commit to table
	zpds::store::StoreTrans storetrans(currtime);
	trans.set_updater( updater.name() );
	storetrans.Commit(stptr,&trans,true); // as master

	status->set_success(true);
}

/**
* ReadDataAction : common read data for all children
*
*/
void zpds::store::ItemDataService::ReadDataAction(
    ::zpds::utils::SharedTable::pointer stptr,
    ::zpds::query::ItemDataRespT* resp)

{
	int fields = AllowedFieldsFilter(resp->fields());
	auto status = resp->mutable_status();

	::zpds::store::UserDataT reader;
	reader.set_name( resp->username() );
	reader.set_sessionkey( resp->sessionkey() );
	reader.set_keytype( resp->usertype() );

	if (! CheckSession(stptr, &reader, false) )
		throw zpds::BadDataException("Invalid reader or Invalid session",M_INVALID_PARAM);

	// update payload if exists
	if (!resp->payload().unique_id().empty()) {
		auto rdata = resp->mutable_payload();
		ReadData(stptr,&reader,rdata,fields);
		status->set_inputcount( status->inputcount() + ( rdata->notfound() ? 0 : 1 ) );
	}

	// update payloads if exists
	for (auto i = 0 ; i<resp->payloads_size(); ++i)	{
		if (resp->payloads(i).unique_id().empty()) continue;
		auto rdata = resp->mutable_payloads(i);
		ReadData(stptr,&reader,rdata,fields);
		status->set_inputcount( status->inputcount() + ( rdata->notfound() ? 0 : 1 ) );
	}

	status->set_success(true);
}

/**
* Get : gets the data
*
*/
void zpds::store::ItemDataService::Get(::zpds::utils::SharedTable::pointer stptr, ::zpds::store::ItemDataT* data) const
{
	std::string temp;
	std::string skey = EncodeSecondaryKey<std::string>( GetUniqueByPrimary(GetKeyType()), data->unique_id());
	bool item_found= stptr->dbcache->GetAssoc(skey, temp);
	if (item_found) {
		item_found = data->ParseFromString(temp);
	}
	else {
		item_found = GetOneRecord(stptr,data);
		if (item_found) {
			stptr->dbcache->SetAssoc(skey, Pack(data) ) ;
		}
	}
	if (!item_found) data->set_notfound(true);
}

/**
* Pack : shrink to bare essentials for packing into cache
*
*/
std::string zpds::store::ItemDataService::Pack(::zpds::store::ItemDataT* data) const
{
	::zpds::store::ItemDataT tocache;
	tocache.set_id( data->id() );
	tocache.set_unique_id( data->unique_id() );
	// TODO: other relevant types
	tocache.set_is_deleted( data->is_deleted() );
	std::string packed;
	tocache.SerializeToString(&packed);
	return packed;
}

/**
* GetUniqueByPrimary : get the unique key for a keytype
*
*/
zpds::store::KeyTypeE zpds::store::ItemDataService::GetUniqueByPrimary(::zpds::store::KeyTypeE keytype) const
{
	::zpds::store::KeyTypeE outkey = ::zpds::store::NOKEY;

	switch(keytype) {
	default:
		break;
	case ::zpds::store::K_LOCALDATA :
		outkey = ::zpds::store::U_LOCALDATA_UNIQUEID;
		break;
	case ::zpds::store::K_WIKIDATA :
		outkey = ::zpds::store::U_WIKIDATA_UNIQUEID;
		break;
	}
	return outkey;
}

/**
* PrepareData : prepares a single itemdata data for update
*
*/
bool zpds::store::ItemDataService::PrepareData(
    ::zpds::utils::SharedTable::pointer stptr,
    ::zpds::store::ExterDataT* updater,
    ::zpds::store::ItemDataT* data,
    int32_t action, int32_t fields, bool merge)
{

	if ( updater->id() == 0 ) {
		throw ::zpds::BadDataException("Updater cannot be found",M_INVALID_PARAM);
	}
	if (data->unique_id().empty())
		throw ::zpds::BadDataException("Unique ID needed",M_INVALID_PARAM);

	::zpds::store::ItemDataT cdata;
	cdata.set_unique_id( data->unique_id() );
	bool item_found = GetOneRecord( stptr, &cdata );

	bool&& item_not_found = !item_found;

	if (item_found && action==ZPDS_UACTION_CREATE)
		throw ::zpds::BadDataException("Item Data already exists, cannot create",M_INVALID_PARAM);

	if (item_not_found && action==ZPDS_UACTION_UPDATE)
		throw ::zpds::BadDataException("Item Data doesnot exist, cannot update",M_INVALID_PARAM);

	if (item_not_found && action==ZPDS_UACTION_DELETE)
		throw ::zpds::BadDataException("Item Data doesnot exist, cannot delete",M_INVALID_PARAM);

	if ( (!updater->is_admin()) && item_found && cdata.locked() )
		throw ::zpds::BadDataException("Item Data is locked, cannot edit",M_INVALID_PARAM);

	int32_t perms = 0;
	// Logic: check if admin
	//        - check user has correct permission
	//        - CREATE ok
	//        - if not present UPSERT ok
	//        - if present UPSERT ior UPDATE requires id match
	if (updater->is_admin())
		perms = ZPDS_WRITR_IS_ADMIN;
	else if (updater->can_add_items()) {
		if (item_not_found && ( action==ZPDS_UACTION_CREATE || action==ZPDS_UACTION_UPSERT ))
			perms = ZPDS_WRITR_CREATING;
		else if (item_found && ( action==ZPDS_UACTION_UPSERT || action==ZPDS_UACTION_UPDATE ) && (updater->name() == cdata.manager()) )
			perms = ZPDS_WRITR_UPDATING;
		else if (item_found && ( action==ZPDS_UACTION_DELETE ) && (updater->name() == cdata.manager()) )
			perms = ZPDS_WRITR_DELETING;
	}

	// handle delete / expired / locked
	if (action==ZPDS_UACTION_DELETE && ( ZPDS_WRITR_IS_ADMIN | ZPDS_WRITR_DELETING )) {
		cdata.set_is_deleted( data->is_deleted() );
		cdata.set_expired( data->expired() );
		cdata.set_locked( data->locked() );
		cdata.Swap(data);
		return item_found;
	}

	// proceed ahead if not delete
	bool changes=false;

	// Modify: olddata
	// dont swap this, we may need both later on
	if (item_found) {
		auto olddata = cdata.mutable_olddata();
		olddata->set_origin( cdata.origin() );
		olddata->mutable_categories()->CopyFrom( cdata.categories() );
	}

#define LOCAL_HANDLE_STRING(AAA) if (!(merge && data->AAA().empty())) { cdata.set_##AAA( data->AAA() ); changes=true; }
#define LOCAL_HANDLE_ENUM(AAA) if (!(merge && (int)data->AAA()==0)) { cdata.set_##AAA( data->AAA() ); changes=true; }
#define LOCAL_HANDLE_NUMBER(AAA) if (!(merge && data->AAA()==0)) { cdata.set_##AAA( data->AAA() ); changes=true; }

	// Modify: categories
	if ( (fields & ZPDS_IDFLD_CATEGORY) && ( perms & ( ZPDS_WRTPID_CATEGORY ))) {
		if ( HandleUserCategories(stptr,data->mutable_categories(),cdata.mutable_categories(),merge) )
			changes=true;
	}

	// Modify: origin
	if ( (fields & ZPDS_IDFLD_ORIGIN) && ( perms & ( ZPDS_WRTPID_ORIGIN ))) {
		LOCAL_HANDLE_STRING(origin)
	}

	// Modify: tags ( allow merge )
	if ( (fields & ZPDS_IDFLD_TAGS) && ( perms & ( ZPDS_WRTPID_TAGS ))) {
		if ( HandleUserTags(stptr,data->mutable_tags(),cdata.mutable_tags(), {GetKeyType()},merge) )
			changes=true;
	}

	// Modify: name fields
	if ( (fields & ZPDS_IDFLD_TEXTFIELDS) && ( perms & ( ZPDS_WRTPID_TEXTFIELDS ) ) ) {
		LOCAL_HANDLE_STRING(title)
		LOCAL_HANDLE_STRING(summary)
		LOCAL_HANDLE_STRING(details)
	}

	// Modify: lang
	if ( (fields & ZPDS_IDFLD_LANGUAGE) && ( perms & ( ZPDS_WRTPID_LANGUAGE ))) {
		LOCAL_HANDLE_ENUM(lang)
	}

	// Modify: cansee fields
	if ( (fields & ZPDS_IDFLD_CANSEE) && ( perms & ( ZPDS_WRTPID_CANSEE ))) {
		LOCAL_HANDLE_ENUM(cansee_type)
	}

	// Modify: image/poster fields
	if ( (fields & ZPDS_IDFLD_IMAGES) && ( perms & ( ZPDS_WRTPID_IMAGES ))) {
		LOCAL_HANDLE_STRING(thumbnail)
		LOCAL_HANDLE_STRING(poster)
		LOCAL_HANDLE_STRING(landscape)
		if ( HandleStrings(data->mutable_posters(), cdata.mutable_posters(),merge) )
			changes=true;
	}

	// Modify: contacts
	if ( (fields & ZPDS_IDFLD_CONTACTS) && ( perms & ( ZPDS_WRTPID_CONTACTS ))) {
		if ( HandleContacts(data->mutable_contacts(), cdata.mutable_contacts(),merge) )
			changes=true;
	}

	// Modify: parent
	if ( (fields & ZPDS_IDFLD_PARENT) && ( perms & ( ZPDS_WRTPID_PARENT ))) {
		if ( HandleAnother(stptr, data->mutable_parent(), cdata.mutable_parent(),merge) )
			changes=true;
	}

	// Modify: alias
	if ( (fields & ZPDS_IDFLD_ALIAS) && ( perms & ( ZPDS_WRTPID_ALIAS ))) {
		if ( HandleAnother(stptr, data->mutable_alias(), cdata.mutable_alias(),merge) )
			changes=true;
	}

	// Modify: importance
	if ( (fields & ZPDS_IDFLD_IMPORTANCE) && ( perms & ( ZPDS_WRTPID_IMPORTANCE ) ) ) {
		LOCAL_HANDLE_NUMBER(importance)
	}

	// Modify: rating
	if ( (fields & ZPDS_IDFLD_RATING) && ( perms & ( ZPDS_WRTPID_RATING ) ) ) {
		LOCAL_HANDLE_NUMBER(rating)
	}

	// Modify: geometry
	if ( (fields & ZPDS_IDFLD_GEOMETRY) && ( perms & ( ZPDS_WRTPID_GEOMETRY ) ) ) {
		LOCAL_HANDLE_STRING(geometry);
	}

	// Modify: local fields
	if ( (fields & ZPDS_IDFLD_LOCALFIELDS) && ( perms & ( ZPDS_WRTPID_LOCALFIELDS ) ) ) {
		LOCAL_HANDLE_STRING(ccode)
		LOCAL_HANDLE_STRING(scode)
		LOCAL_HANDLE_STRING(city)
		LOCAL_HANDLE_STRING(country)
		LOCAL_HANDLE_STRING(state)
		LOCAL_HANDLE_STRING(fld_name)
		LOCAL_HANDLE_STRING(fld_area)
		LOCAL_HANDLE_STRING(address)
		LOCAL_HANDLE_STRING(pincode)
		LOCAL_HANDLE_STRING(region)
		LOCAL_HANDLE_NUMBER(accuracy)
		LOCAL_HANDLE_NUMBER(lat)
		LOCAL_HANDLE_NUMBER(lon)
		LOCAL_HANDLE_STRING(location)
		LOCAL_HANDLE_STRING(landmark)
		LOCAL_HANDLE_STRING(local_name)
		// LOCAL_HANDLE_STRING(geohash9)
	}

	// Modify: osm fields
	if ( (fields & ZPDS_IDFLD_OSMFIELDS) && ( perms & ( ZPDS_WRTPID_OSMFIELDS ) ) ) {
		LOCAL_HANDLE_NUMBER(osm_id)
		LOCAL_HANDLE_STRING(osm_key)
		LOCAL_HANDLE_STRING(osm_value)
		LOCAL_HANDLE_STRING(osm_type)
		if ( HandleUserAttr(data->mutable_attr(),cdata.mutable_attr(), merge) )
			changes=true;

	}

#undef LOCAL_HANDLE_STRING
#undef LOCAL_HANDLE_ENUM
#undef LOCAL_HANDLE_NUMBER

	// add notfound if not found
	if (item_not_found && (action==ZPDS_UACTION_CREATE || action==ZPDS_UACTION_UPSERT) ) {
		cdata.set_notfound( false );
	}

	// finally swap
	cdata.Swap(data);

	DLOG(INFO) << "data: " << data->DebugString() << std::endl;

	return item_found;

}

/**
* ReadData : reads a single itemdata data, common for all children
*
*/
void zpds::store::ItemDataService::ReadData(
    ::zpds::utils::SharedTable::pointer stptr,
    ::zpds::store::UserDataT* reader,
    ::zpds::store::ItemDataT* data, int32_t fields)
{

	// if unique_id present
	if (data->unique_id().empty())
		throw ::zpds::BadDataException("unique_id is needed",M_INVALID_PARAM);

	::zpds::store::ItemDataT cdata;
	cdata.set_unique_id( data->unique_id() );
	bool item_found = GetOneRecord( stptr, &cdata );

	if (!item_found) {
		data->set_notfound(true);
		return;
	}

	// Default public
	::zpds::query::ReaderTypeE rtype = ::zpds::query::R_PUBLIC;
	// check if admin, manager or valid user
	if ( reader->is_admin() )
		rtype = ::zpds::query::R_ADMIN;
	else if (reader->keytype() == ::zpds::store::K_EXTERDATA && reader->name() == cdata.manager() )
		rtype = ::zpds::query::R_CREATR;
	else if (reader->keytype() == ::zpds::store::K_EXTERDATA)
		rtype = ::zpds::query::R_OTHER;
	else if (reader->keytype() == ::zpds::store::K_USERDATA && ucserv.ValidateUserCategories(stptr, reader,data->mutable_categories()) )
		rtype = ::zpds::query::R_USER;

	int32_t perms = ReaderTypeFilter(rtype);

	// Read: category
	if ( (fields & ZPDS_IDFLD_CATEGORY) && ( perms & ( ZPDS_READID_CATEGORY ))) {
		data->mutable_categories()->Swap( cdata.mutable_categories() );
	}

	// Read: origin
	if ( (fields & ZPDS_IDFLD_ORIGIN) && ( perms & ( ZPDS_READID_ORIGIN ))) {
		data->set_origin( cdata.origin() );
	}

	// Read: tags
	if ( (fields & ZPDS_IDFLD_TAGS) && ( perms & ( ZPDS_READID_TAGS ))) {
		for (auto i=0; i< cdata.tags_size(); ++i) {
			auto tag = data->add_tags();
			tag->set_name( cdata.tags(i).name() );
			tag->set_value( cdata.tags(i).value() );
		}
	}

	// Read: text fields
	if ( (fields & ZPDS_IDFLD_TEXTFIELDS) && ( perms & ( ZPDS_READID_TEXTFIELDS ) ) ) {
		data->set_title( cdata.title() );
		data->set_summary( cdata.summary() );
		data->set_details( cdata.details() );
	}

	// Read: language
	if ( (fields & ZPDS_IDFLD_LANGUAGE) && ( perms & ( ZPDS_READID_LANGUAGE ))) {
		data->set_lang( cdata.lang() );
	}

	// Read: cansee fields
	if ( (fields & ZPDS_IDFLD_CANSEE) && ( perms & ( ZPDS_READID_CANSEE ))) {
		data->set_cansee_type( cdata.cansee_type() );
	}

	// Read: image/poster fields
	if ( (fields & ZPDS_IDFLD_IMAGES) && ( perms & ( ZPDS_READID_IMAGES ))) {
		data->set_thumbnail( cdata.thumbnail() );
		data->set_poster( cdata.poster() );
		data->mutable_posters()->Swap( cdata.mutable_posters() );
		data->set_landscape( cdata.landscape() );
	}

	// Read: contacts
	if ( (fields & ZPDS_IDFLD_CONTACTS) && ( perms & ( ZPDS_READID_CONTACTS ))) {
		data->mutable_contacts()->Swap( cdata.mutable_contacts() );
	}

	// Read: parent
	if ( (fields & ZPDS_IDFLD_PARENT) && ( perms & ( ZPDS_READID_PARENT ))) {
		data->mutable_parent()->Swap( cdata.mutable_parent() );
	}

	// Read: alias
	if ( (fields & ZPDS_IDFLD_ALIAS) && ( perms & ( ZPDS_READID_ALIAS ))) {
		data->mutable_alias()->Swap( cdata.mutable_alias() );
	}

	// Read: importance
	if ( (fields & ZPDS_IDFLD_IMPORTANCE) && ( perms & ( ZPDS_READID_IMPORTANCE ))) {
		data->set_importance( cdata.importance() );
	}

	// Read: rating
	if ( (fields & ZPDS_IDFLD_RATING) && ( perms & ( ZPDS_READID_RATING ))) {
		data->set_rating( cdata.rating() );
	}

	// Read: geometry
	if ( (fields & ZPDS_IDFLD_GEOMETRY) && ( perms & ( ZPDS_READID_GEOMETRY ))) {
		data->set_geometry( cdata.geometry() );
	}

	// Read: local fields
	if ( (fields & ZPDS_IDFLD_LOCALFIELDS) && ( perms & ( ZPDS_READID_LOCALFIELDS ) ) ) {
		data->set_ccode( cdata.ccode() );
		data->set_scode( cdata.scode() );
		data->set_city( cdata.city() );
		data->set_country( cdata.country() );
		data->set_state( cdata.state() );
		data->set_fld_name( cdata.fld_name() );
		data->set_fld_area( cdata.fld_area() );
		data->set_address( cdata.address() );
		data->set_pincode( cdata.pincode() );
		data->set_region( cdata.region() );
		data->set_accuracy( cdata.accuracy() );
		data->set_lat( cdata.lat() );
		data->set_lon( cdata.lon() );
		data->set_location( cdata.location() );
		data->set_landmark( cdata.landmark() );
		data->set_local_name( cdata.local_name() );
	}

	// Read: osm fields
	if ( (fields & ZPDS_IDFLD_OSMFIELDS) && ( perms & ( ZPDS_READID_OSMFIELDS ) ) ) {
		data->set_osm_id( cdata.osm_id() );
		data->set_osm_key( cdata.osm_key() );
		data->set_osm_value( cdata.osm_value() );
		data->set_osm_type( cdata.osm_type() );
	}

}

/**
* HandleRelation : handle child relation between items
*
*/
bool zpds::store::ItemDataService::HandleRelation(
    ::zpds::utils::SharedTable::pointer stptr, ::zpds::store::KeyTypeE keytype,
    ::google::protobuf::RepeatedPtrField< ::zpds::store::ItemDataT >* idata,
    ::google::protobuf::RepeatedPtrField< ::zpds::store::ItemDataT >* odata,
    bool merge)
{
	bool change=false;
	std::unordered_map<uint64_t, ::zpds::store::ItemDataT* > kmap;
	// if not merge clear old
	if (merge) {
		for (auto i=0; i<odata->size(); ++i) {
			auto var = odata->Mutable(i);
			kmap[var->id()] = var;
		}
	}
	else {
		odata->Clear();
	}
	for (auto i=0; i<idata->size(); ++i) {
		auto var = idata->Mutable(i);
		if ( ! GetBlindBySecondaryKey(stptr, var,keytype,true) )
			throw ::zpds::BadDataException("Child Relation does not exist");
		if (kmap.find(var->id())==kmap.end()) {
			kmap[var->id()] = odata->Add();
			kmap[var->id()]->set_id( var->id() );
			change=true;
		}
	}
	return change;
}

/**
* HandleContacts : handle contacts
*
*/
bool zpds::store::ItemDataService::HandleContacts(
    ::google::protobuf::RepeatedPtrField< ::zpds::store::VerifiedT >* idata,
    ::google::protobuf::RepeatedPtrField< ::zpds::store::VerifiedT >* odata,
    bool merge)
{
	bool change=false;
	std::unordered_map<std::string, ::zpds::store::VerifiedT* > kmap;
	// if not merge clear old
	if (merge) {
		for (auto i=0; i<odata->size(); ++i) {
			auto var = odata->Mutable(i);
			std::string skey = EncodeSecondaryKey<int,std::string>(::zpds::store::K_NONODE, (int) var->vtype(), var->data() );
			kmap[skey] = var;
		}
	}
	else {
		odata->Clear();
	}
	for (auto i=0; i<idata->size(); ++i) {
		auto var = idata->Mutable(i);
		if ( var->data().empty() )
			throw ::zpds::BadDataException("Data for contact does not exist");
		std::string skey = EncodeSecondaryKey<int,std::string>(::zpds::store::K_NONODE, (int) var->vtype(), var->data() );
		if (kmap.find(skey)==kmap.end()) {
			kmap[skey] = odata->Add();
			kmap[skey]->Swap( var );
			change=true;
		}
	}
	return change;
}
