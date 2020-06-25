/**
 * @project zapdos
 * @file src/store/UserDataService.cc
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
 *  UserDataService.cc : User Data Manipulation impl
 *
 */
#include <unordered_map>
#include <unordered_set>
#include <map>

#include "store/UserDataService.hpp"
#include "store/CategoryService.hpp"

#include "store/TempNameCache.hpp"
#include "store/UserDataTable.hpp"
#include "crypto/CryptoTraits.hpp"
#include "store/StoreTrans.hpp"

/**
* AllowedFieldsFilter : sets update variable
*
*/
int32_t zpds::store::UserDataService::AllowedFieldsFilter(int32_t update)
{
	int32_t xupdate=0;
	if (update & ZPDS_UDFLD_SIGNKEY) xupdate+=ZPDS_UDFLD_SIGNKEY;
	if (update & ZPDS_UDFLD_PASSKEY) xupdate+=ZPDS_UDFLD_PASSKEY;
	// if (update & ZPDS_UDFLD_ROLE) xupdate+=ZPDS_UDFLD_ROLE;
	if (update & ZPDS_UDFLD_INFO) xupdate+=ZPDS_UDFLD_INFO;
	if (update & ZPDS_UDFLD_GNAME) xupdate+=ZPDS_UDFLD_GNAME;
	if (update & ZPDS_UDFLD_MOBILE) xupdate+=ZPDS_UDFLD_MOBILE;
	if (update & ZPDS_UDFLD_EMAIL) xupdate+=ZPDS_UDFLD_EMAIL;
	// if (update & ZPDS_UDFLD_PROFILES) xupdate+=ZPDS_UDFLD_PROFILES;
	if (update & ZPDS_UDFLD_INVITED) xupdate+=ZPDS_UDFLD_INVITED;
	if (update & ZPDS_UDFLD_TAGS) xupdate+=ZPDS_UDFLD_TAGS;
	if (update & ZPDS_UDFLD_SUBSCR) xupdate+=ZPDS_UDFLD_SUBSCR;
	if (update & ZPDS_UDFLD_CATEGORIES) xupdate+=ZPDS_UDFLD_CATEGORIES;
	return xupdate;
}

/**
* HandleQueryProfiles : handles user profiles
*
*/
bool zpds::store::UserDataService::HandleQueryProfiles(
    ::google::protobuf::RepeatedPtrField< ::zpds::search::QueryProfT >* idata,
    ::google::protobuf::RepeatedPtrField< ::zpds::search::QueryProfT >* odata,
    bool merge)
{
	bool change=false;

	std::unordered_map<::zpds::search::QueryTypeE, ::zpds::search::QueryProfT* > kmap;
	// if not merge clear old
	if (merge) {
		for (auto i=0; i<odata->size(); ++i) {
			auto var = odata->Mutable(i);
			kmap[var->query_type()] = var;
		}
	}
	else {
		odata->Clear();
	}
	for (auto i=0; i<idata->size(); ++i) {
		auto var = idata->Mutable(i);
		if ( var->query_type()== ::zpds::search::QRY_UNKNOWN )
			throw ::zpds::BadDataException("query_type is needed for each ruleset");
		if (var->rules_size() > ZPDS_MAX_PROFILE_RULES )
			throw ::zpds::BadDataException("max rules size exceeded for ruleset");
		if (kmap.find(var->query_type())==kmap.end()) {
			std::map<int, ::zpds::search::QueryRuleT* > lmap;
			for (auto j=0; j<var->rules_size(); ++j) {
				if (var->rules(j).weight() > ZPDS_MAX_RULE_WEIGHT )
					throw ::zpds::BadDataException("max rules weight exceeded for ruleset");
				auto ikey = ZPDS_MAX_PROFILE_RULES * ZPDS_MAX_RULE_WEIGHT + j - var->rules(j).weight() * ZPDS_MAX_PROFILE_RULES;
				lmap.emplace( ikey, var->mutable_rules(j) );
			}
			kmap[var->query_type()] = odata->Add();
			auto nvar = kmap[var->query_type()];

			nvar->set_query_type( var->query_type() );
			for (auto& rs : lmap ) {
				nvar->add_rules()->Swap( rs.second );
			}
			change=true;
		}
	}

	return change;
}

/**
* SetDefaultProfile : handles setting default profile
*
*/
void zpds::store::UserDataService::SetDefaultProfile(
    ::google::protobuf::RepeatedPtrField< ::zpds::search::QueryProfT >* prof)
{
	namespace zs = ::zpds::search;
	prof->Clear();
	// NOTE: The Order has to be high to low, no checks
	auto photon_p = prof->Add();
	photon_p->set_query_type(zs::QRY_COMPLETION_PHOTON);
	{
		auto r = photon_p->add_rules();
		r->set_weight(100);
		r->set_desc("one word query for upto 2 chars");
		r->set_input_type(zs::I_ONEWORD);
		r->set_search_type(zs::S_BEGINWITH);
		r->set_max_records(10);
		r->set_oneword_length(2);
	}
	{
		auto r = photon_p->add_rules();
		r->set_distance_band(1000);
		r->set_max_records(10);
		r->set_weight(90);
		r->set_desc("by distance 1km bands");
		r->set_order_type(zs::O_DIST_BAND);
	}
	{
		auto r = photon_p->add_rules();
		r->set_weight(80);
		r->set_desc("if not local");
		r->set_max_records(10);
	}
	auto notoph_p = prof->Add();
	notoph_p->set_query_type(zs::QRY_COMPLETION_NOTOPH);
	{
		auto r = notoph_p->add_rules();
		r->set_order_type(zs::O_DIST_BAND);
		r->set_desc("by distance only so band is small");
		r->set_limit_type(zs::L_NBRHASH5);
		r->set_weight(100);
		r->set_max_records(5);
		r->set_distance_band(10);
	}
	auto wiki_p = prof->Add();
	wiki_p->set_query_type(zs::QRY_COMPLETION_WIKIDATA);
	{
		auto r = wiki_p->add_rules();
		r->set_oneword_length(2);
		r->set_max_records(10);
		r->set_desc("one word query for upto 2 chars");
		r->set_input_type(zs::I_ONEWORD);
		r->set_search_type(zs::S_BEGINWITH);
		r->set_weight(100);
	}
	{
		auto r = wiki_p->add_rules();
		r->set_max_records(10);
		r->set_desc("find in name");
		r->set_search_type(zs::S_NAME);
		r->set_weight(90);
	}
	{
		auto r = wiki_p->add_rules();
		r->set_max_records(10);
		r->set_desc("find in suggest");
		r->set_weight(80);
	}
}

/**
* PrepareData : sets userdata, updater is expected to be prefilled
*
*/
bool zpds::store::UserDataService::PrepareData(
    ::zpds::utils::SharedTable::pointer stptr,
    ::zpds::store::ExterDataT* updater,
    ::zpds::store::UserDataT* data,
    int32_t action, int32_t fields, bool merge)
{

	if ( updater->id() == 0 ) {
		throw ::zpds::BadDataException("Updater cannot be found",M_INVALID_PARAM);
	}
	if (data->name().empty())
		throw ::zpds::BadDataException("Name needed",M_INVALID_PARAM);

	::zpds::store::UserDataTable user_table{stptr->maindb.Get()};
	::zpds::store::UserDataT cdata;
	cdata.set_name( data->name() );
	bool user_found = user_table.GetOne(&cdata,::zpds::store::U_USERDATA_NAME);

	bool&& user_not_found = !user_found;

	if (user_found && action==ZPDS_UACTION_CREATE)
		throw ::zpds::BadDataException("User Data already exists, cannot create",M_INVALID_PARAM);

	if (user_not_found && action==ZPDS_UACTION_UPDATE)
		throw ::zpds::BadDataException("User Data doesnot exist, cannot update",M_INVALID_PARAM);

	if (user_not_found && action==ZPDS_UACTION_DELETE)
		throw ::zpds::BadDataException("User Data doesnot exist, cannot delete",M_INVALID_PARAM);

	if ( (!updater->is_admin()) && user_found && cdata.locked() )
		throw ::zpds::BadDataException("User Data is locked, cannot edit",M_INVALID_PARAM);

	int32_t perms = 0;
	// Logic: check if self or admin
	//        - check user has correct permission
	//        - CREATE ok
	//        - if not present UPSERT ok
	//        - if present UPSERT ior UPDATE requires id match
	if (updater->keytype() == ::zpds::store::K_USERDATA && updater->name() == cdata.name() )
		perms = ZPDS_WRITR_IS_SELF;
	else if (updater->is_admin())
		perms = ZPDS_WRITR_IS_ADMIN;
	else if (updater->can_add_users()) {
		if (user_not_found && ( action==ZPDS_UACTION_CREATE || action==ZPDS_UACTION_UPSERT ))
			perms = ZPDS_WRITR_CREATING;
		else if (user_found && ( action==ZPDS_UACTION_UPSERT || action==ZPDS_UACTION_UPDATE ) && (updater->name() == cdata.manager()) )
			perms = ZPDS_WRITR_UPDATING;
		else if (user_found && ( action==ZPDS_UACTION_DELETE ) && (updater->name() == cdata.manager()) )
			perms = ZPDS_WRITR_DELETING;
	}
	if (perms==ZPDS_WRITR_IS_SELF && action!=ZPDS_UACTION_UPDATE )
		throw ::zpds::BadDataException("Self can only update",M_INVALID_PARAM);

	// handle delete / expired / locked
	if (action==ZPDS_UACTION_DELETE && ( ZPDS_WRITR_IS_ADMIN | ZPDS_WRITR_DELETING )) {
		cdata.set_is_deleted( data->is_deleted() );
		cdata.set_expired( data->expired() );
		cdata.set_locked( data->locked() );
		cdata.Swap(data);
		return user_found;
	}

	// proceed ahead if not delete
	bool changes=false;

	// Modify: signkey
	if ( (fields & ZPDS_UDFLD_SIGNKEY) && ( perms & ( ZPDS_WRTPUD_SIGNKEY ) ) ) {
		// create signkey, this will not change TODO: provision to change
		auto signkey = cdata.mutable_signkey();
		::zpds::crypto::CryptoTraits::pointer signer = ::zpds::crypto::CryptoTraits::Create(ENCODEKEY_ALGO_DEFAULT);
		signkey->set_pubkey ( signer->GetPublicKey() );
		signkey->set_seckey ( signer->GetSecretKey() );
		signkey->set_algo ( ENCODEKEY_ALGO_DEFAULT );
		changes=true;
	}

#define LOCAL_HANDLE_STRING(AAA) if (!(merge && data->AAA().empty())) { cdata.set_##AAA( data->AAA() ); changes=true; }
#define LOCAL_HANDLE_ENUM(AAA) if (!(merge && (int)data->AAA()==0)) { cdata.set_##AAA( data->AAA() ); changes=true; }
#define LOCAL_HANDLE_NUMBER(AAA) if (!(merge && data->AAA()==0)) { cdata.set_##AAA( data->AAA() ); changes=true; }

	// Modify: passkey
	if ( (fields & ZPDS_UDFLD_PASSKEY) && ( perms & ( ZPDS_WRTPUD_PASSKEY ) ) ) {
		if (data->passkey().empty() )
			throw ::zpds::BadDataException("Passkey is blank",M_INVALID_PARAM);
		LOCAL_HANDLE_STRING(passkey)
	}

	// Modify: role

	// Modify: description
	if ( (fields & ZPDS_UDFLD_INFO) && ( perms & ( ZPDS_WRTPUD_INFO ) ) ) {
		LOCAL_HANDLE_STRING(description)
	}

	// Modify: given_name
	if ( (fields & ZPDS_UDFLD_GNAME) && ( perms & ( ZPDS_WRTPUD_GNAME ) ) ) {
		LOCAL_HANDLE_STRING(given_name)
	}

	// Modify: subscription
	if ( (fields & ZPDS_UDFLD_SUBSCR) && ( perms & ( ZPDS_WRTPUD_SUBSCR ) ) ) {
		LOCAL_HANDLE_ENUM(subscription)
	}

	// Modify: primary_mobile
	if ( (fields & ZPDS_UDFLD_MOBILE) && ( perms & ( ZPDS_WRTPUD_MOBILE ) ) ) {
		LOCAL_HANDLE_STRING(primary_mobile)
	}

	// Modify: primary_email
	if ( (fields & ZPDS_UDFLD_EMAIL) && ( perms & ( ZPDS_WRTPUD_EMAIL ) ) ) {
		LOCAL_HANDLE_STRING(primary_email)
	}

	// Modify: profiles ( allow merge ) kept for future if allow user profile
	if ( (fields & ZPDS_UDFLD_PROFILES) && ( perms & ( ZPDS_WRTPUD_PROFILES ) ) ) {
		if ( HandleQueryProfiles(data->mutable_profiles(),cdata.mutable_profiles(),merge) )
			changes=true;
	}

	// Modify: invited
	if ( (fields & ZPDS_UDFLD_INVITED) && ( perms & ( ZPDS_WRTPUD_INVITED ))) {
		cdata.set_is_invited( data->is_invited() );
		changes=true;
	}

	// Modify: tags ( allow merge )
	if ( (fields & ZPDS_UDFLD_TAGS) && ( perms & ( ZPDS_WRTPUD_TAGS ))) {
		if ( HandleUserTags(stptr,data->mutable_tags(),cdata.mutable_tags(), {GetKeyType()},merge) )
			changes=true;
	}

	// Modify: categories ( allow merge )
	if ( (fields & ZPDS_UDFLD_CATEGORIES) && ( perms & ( ZPDS_WRTPUD_CATEGORIES ))) {
		if ( HandleUserCategories(stptr,data->mutable_categories(),cdata.mutable_categories(),merge) )
			changes=true;
	}

	// add id name if not found
	if (user_not_found && (action==ZPDS_UACTION_CREATE || action==ZPDS_UACTION_UPSERT) ) {
		cdata.set_notfound( false );
	}

#undef LOCAL_HANDLE_STRING
#undef LOCAL_HANDLE_ENUM
#undef LOCAL_HANDLE_NUMBER

	// finally swap
	cdata.Swap(data);
	return user_found;

}

/**
* ManageDataAction : sets userdata
*
*/
void zpds::store::UserDataService::ManageDataAction(::zpds::utils::SharedTable::pointer stptr, ::zpds::query::UserDataRespT* resp)
{
	if (!stptr->is_master.Get())
		throw ::zpds::BadCodeException("UserData change is master only");

	auto status = resp->mutable_status();
	int32_t action=0;
	bool merge = false;
	std::tie(action,merge)=WriteActionFilter(resp->action());
	int fields = AllowedFieldsFilter(resp->fields());

	::zpds::store::UserDataT updater;
	updater.set_name( resp->username() );
	updater.set_sessionkey( resp->sessionkey() );
	updater.set_keytype( resp->keytype() );

	DLOG(INFO) << std::endl << resp->DebugString() << std::endl;

	if (! CheckSession(stptr, &updater, false) )
		throw zpds::BadDataException("Invalid user login or Invalid session",M_INVALID_PARAM);

	// just add the payload to payloads for easy processing
	if (!resp->payload().name().empty())
		resp->add_payloads()->Swap(resp->mutable_payload());
	status->set_inputcount( resp->payloads_size() );

	// start addition , first format the data , then update
	uint64_t currtime = ZPDS_CURRTIME_MS;

	zpds::store::TransactionT trans;
	::zpds::store::UserDataTable user_table{stptr->maindb.Get()};
	::zpds::store::TempNameCache namecache{stptr};

	std::unordered_set<std::string> forbidden { ZPDS_FORBIDDEN_USERNAMES };

	// update payloads if exists
	for (size_t i = 0 ; i<resp->payloads_size(); ++i)	{
		auto rdata = resp->mutable_payloads(i);
		if ( rdata->name().empty() )
			throw zpds::BadDataException("Name not found: " + rdata->name(),M_INVALID_PARAM);
		if (rdata->name() != SanitNSLower(rdata->name()) )
			throw zpds::BadDataException("Name invalid, lowercase and no spaces: " + rdata->name(),M_INVALID_PARAM);

		bool user_found = PrepareData(stptr,&updater,rdata,action,fields,merge);

		// reserve this name
		if (namecache.CheckLocal(::zpds::store::K_USERDATA,rdata->name()))
			throw zpds::BadDataException("Duplicate name found: " + rdata->name(),M_INVALID_PARAM);
		if (!namecache.ReserveName(::zpds::store::K_USERDATA,rdata->name(),false))
			throw zpds::BadDataException("This name is being worked on: " + rdata->name(),M_INVALID_PARAM);

		if (! user_found) {
			if ( forbidden.find(rdata->name()) != forbidden.end() )
				throw zpds::BadDataException("Name not valid: " + rdata->name(),M_INVALID_PARAM);
			rdata->set_manager( updater.name() );
			rdata->set_id(stptr->maincounter.GetNext() );
			rdata->set_created_at( currtime );
		}

		rdata->set_updated_at( currtime );
		status->set_updatecount( status->updatecount() + 1 );

		if (!user_table.AddRecord(rdata,&trans,user_found))
			throw ::zpds::BadDataException("Cannot Insert user data");
	}

	// commit to table
	zpds::store::StoreTrans storetrans(currtime);
	trans.set_updater( updater.name() );
	storetrans.Commit(stptr,&trans,true); // as master

	status->set_success(true);
}

/**
* ReadData : sets userdata for read , assuming reader is set
*
*/
void zpds::store::UserDataService::ReadData(
    ::zpds::utils::SharedTable::pointer stptr,
    ::zpds::store::UserDataT* reader,
    ::zpds::store::UserDataT* data, int32_t fields)
{

	::zpds::store::UserDataTable user_table{stptr->maindb.Get()};
	::zpds::store::UserDataT cdata;
	bool user_found=false;
	// if name present
	if (data->name().empty())
		throw ::zpds::BadDataException("Name is needed",M_INVALID_PARAM);
	cdata.set_name( data->name() );
	user_found = user_table.GetOne(&cdata,::zpds::store::U_USERDATA_NAME);

	if (!user_found) {
		data->set_notfound(true);
		return;
	}

	// Default public
	::zpds::query::ReaderTypeE rtype = ::zpds::query::R_PUBLIC;
	// check if admin, manager or self
	if ( reader->is_admin() )
		rtype = ::zpds::query::R_ADMIN;
	else if (reader->keytype() == ::zpds::store::K_EXTERDATA && reader->name() == cdata.manager() )
		rtype = ::zpds::query::R_CREATR;
	else if (reader->keytype() == ::zpds::store::K_USERDATA && reader->name() == cdata.name() )
		rtype = ::zpds::query::R_SELF;
	else if (reader->keytype() == ::zpds::store::K_EXTERDATA)
		rtype = ::zpds::query::R_OTHER;
	else if (reader->keytype() == ::zpds::store::K_USERDATA )
		rtype = ::zpds::query::R_USER;

	int32_t perms = ReaderTypeFilter(rtype);
	DLOG(INFO) << "Start Reading Fields" << std::endl;

	// Read: signkey disabled
	if ( (fields & ZPDS_UDFLD_SIGNKEY) && ( perms & ( ZPDS_READUD_SIGNKEY ) ) ) {
		// data->mutable_signkey()->Swap( cdata.mutable_signkey() );
	}

	// Read: passkey disabled
	if ( (fields & ZPDS_UDFLD_PASSKEY) && ( perms & ( ZPDS_READUD_PASSKEY ) ) ) {
		// data->set_passkey( cdata.passkey() );
	}

	// Read: role
	if ( (fields & ZPDS_UDFLD_ROLE) && ( perms & ( ZPDS_READUD_ROLE ) ) ) {
		// NA
	}

	// Read: description
	if ( (fields & ZPDS_UDFLD_INFO) && ( perms & ( ZPDS_READUD_INFO ) ) ) {
		data->set_description( cdata.description() );
	}

	// Read: given_name
	if ( (fields & ZPDS_UDFLD_GNAME) && ( perms & ( ZPDS_READUD_GNAME ) ) ) {
		data->set_given_name( cdata.given_name() );
	}

	// Read: subscription
	if ( (fields & ZPDS_UDFLD_SUBSCR) && ( perms & ( ZPDS_READUD_SUBSCR ) ) ) {
		data->set_subscription( cdata.subscription() );
	}

	// Read: mobile + primary_mobile
	if ( (fields & ZPDS_UDFLD_MOBILE) && ( perms & ( ZPDS_READUD_MOBILE ) ) ) {
		data->set_primary_mobile(cdata.primary_mobile());
	}

	// Read: email + primary_email
	if ( (fields & ZPDS_UDFLD_EMAIL) && ( perms & ( ZPDS_READUD_EMAIL ) ) ) {
		data->set_primary_email(cdata.primary_email());
	}

	// Read: profiles
	if ( (fields & ZPDS_UDFLD_PROFILES) && ( perms & ( ZPDS_READUD_PROFILES ) ) ) {
		data->mutable_profiles()->Swap( cdata.mutable_profiles() );
	}

	// Read: invited
	if ( (fields & ZPDS_UDFLD_INVITED) && ( perms & ( ZPDS_READUD_INVITED ))) {
		data->set_is_invited(cdata.is_invited());
	}

	DLOG(INFO) << "Start Reading Tags" << std::endl;
	// Read: tags
	if ( (fields & ZPDS_UDFLD_TAGS) && ( perms & ( ZPDS_READUD_TAGS ))) {
		for (auto i=0; i< cdata.tags_size(); ++i) {
			auto tag = data->add_tags();
			tag->set_name( cdata.tags(i).name() );
			tag->set_value( cdata.tags(i).value() );
		}
	}

	DLOG(INFO) << "Start Reading Categories" << std::endl;
	// Read: categories
	if ( (fields & ZPDS_UDFLD_CATEGORIES) && ( perms & ( ZPDS_READUD_CATEGORIES ))) {
		for (auto i=0; i< cdata.categories_size(); ++i) {
			data->add_categories( cdata.categories(i) );
		}
	}

}

/**
* ReadDataAction : read externaldata
*
*/
void zpds::store::UserDataService::ReadDataAction(
    ::zpds::utils::SharedTable::pointer stptr,
    ::zpds::query::UserDataRespT* resp)

{
	int fields = AllowedFieldsFilter(resp->fields());
	auto status = resp->mutable_status();

	::zpds::store::UserDataT reader;
	reader.set_name( resp->username() );
	reader.set_sessionkey( resp->sessionkey() );
	reader.set_keytype( resp->keytype() );

	if (! CheckSession(stptr, &reader, false) )
		throw zpds::BadDataException("Invalid reader or Invalid session",M_INVALID_PARAM);

	// update payload if exists
	if (!resp->payload().name().empty()) {
		auto rdata = resp->mutable_payload();
		ReadData(stptr,&reader,rdata,fields);
		status->set_inputcount( status->inputcount() + ( rdata->notfound() ? 0 : 1 ) );
	}

	// update payloads if exists
	for (auto i = 0 ; i<resp->payloads_size(); ++i)	{
		if (resp->payloads(i).name().empty()) continue;
		auto rdata = resp->mutable_payloads(i);
		ReadData(stptr,&reader,rdata,fields);
		status->set_inputcount( status->inputcount() + ( rdata->notfound() ? 0 : 1 ) );
	}

	status->set_success(true);
}

/**
* GetKeyType : handles keytype for tags
*
*/
::zpds::store::KeyTypeE zpds::store::UserDataService::GetKeyType() const
{
	return ::zpds::store::K_USERDATA;
}

/**
* ResetPassAction : update password
*
*/
void zpds::store::UserDataService::ResetPassAction(::zpds::utils::SharedTable::pointer stptr, ::zpds::query::ResetRespT* resp)
{
	if (!stptr->is_master.Get())
		throw ::zpds::BadCodeException("Reset pass is master only");

	if (resp->username().empty())
		throw ::zpds::BadDataException("Name cannot be empty",M_INVALID_PARAM);
	if (resp->newpass().empty())
		throw ::zpds::BadDataException("Newpass cannot be empty",M_INVALID_PARAM);
	if (resp->oldpass().empty() && resp->otp().empty())
		throw ::zpds::BadDataException("Need oldpass or OTP",M_INVALID_PARAM);

	::zpds::store::UserDataT data;
	data.set_name( resp->username());
	::zpds::store::UserDataTable ext_table{stptr->maindb.Get()};
	bool user_found = ext_table.GetOne(&data,::zpds::store::U_USERDATA_NAME);
	if (!user_found)
		throw ::zpds::BadDataException("This user does not exist",M_INVALID_PARAM);

	if (resp->otp().empty() && data.passkey() != resp->oldpass() )
		throw ::zpds::BadDataException("Old password is bad",M_INVALID_PARAM);

	if (resp->oldpass().empty() && data.last_otp() != resp->otp() )
		throw ::zpds::BadDataException("OTP mismatch",M_INVALID_PARAM);

	uint64_t currtime = ZPDS_CURRTIME_MS;
	data.set_passkey( resp->newpass() );
	data.set_updated_at( currtime );

	zpds::store::TransactionT trans;
	if (!ext_table.AddRecord(&data,&trans,user_found))
		throw ::zpds::BadDataException("Cannot Insert passkey data");
	zpds::store::StoreTrans storetrans(currtime);
	trans.set_updater( ZPDS_DEFAULT_ADMIN );
	storetrans.Commit(stptr,&trans,true); // as master
	resp->mutable_status()->set_success(true);
}

