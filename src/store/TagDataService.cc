/**
 * @project zapdos
 * @file src/store/TagDataService.cc
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
 *  TagDataService.cc : User Data Manipulation impl
 *
 */
#include "store/TagDataService.hpp"
#include "store/ServiceDefine.hh"

#include "store/TagDataTable.hpp"
#include "store/StoreTrans.hpp"
#include "store/TempNameCache.hpp"

/**
* Get : gets the data
*
*/
void zpds::store::TagDataService::Get(::zpds::utils::SharedTable::pointer stptr, ::zpds::store::TagDataT* data) const
{
	std::string temp;
	bool tag_found= stptr->dbcache->GetAssoc(
	                    EncodeSecondaryKey<int32_t,std::string>(U_TAGDATA_KEYTYPE_NAME, data->keytype(), data->name() ), temp);
	if (tag_found) {
		tag_found = data->ParseFromString(temp);
	}
	else {
		::zpds::store::TagDataTable data_table{stptr->maindb.Get()};
		tag_found = data_table.GetOne(data,::zpds::store::U_TAGDATA_KEYTYPE_NAME);
		if (tag_found) {
			stptr->dbcache->SetAssoc(
			    EncodeSecondaryKey<int32_t,std::string>(U_TAGDATA_KEYTYPE_NAME, data->keytype(), data->name()), Pack(data) ) ;
		}
	}
	if (!tag_found) data->set_notfound(true);
}

/**
* Pack : shrink to bare essentials for packing into cache
*
*/
std::string zpds::store::TagDataService::Pack(::zpds::store::TagDataT* data) const
{
	::zpds::store::TagDataT tocache;
	tocache.set_id( data->id() );
	tocache.set_keytype( data->keytype() );
	tocache.set_name( data->name() );
	tocache.set_is_allowed( data->is_allowed() );
	tocache.set_is_repeated( data->is_repeated() );
	tocache.set_is_searchable( data->is_searchable() );
	tocache.set_is_deleted( data->is_deleted() );
	std::string packed;
	tocache.SerializeToString(&packed);
	return packed;
}

/**
* ManageDataAction : sets tagdata
*
*/
void zpds::store::TagDataService::ManageDataAction(::zpds::utils::SharedTable::pointer stptr, ::zpds::query::TagDataRespT* resp)
{
	if (!stptr->is_master.Get())
		throw ::zpds::BadCodeException("TagData change is master only");

	auto status = resp->mutable_status();
	int action=0;
	bool merge=false;
	std::tie(action,merge)=WriteActionFilter(resp->action());

	::zpds::store::ExterDataT updater;
	updater.set_name( resp->username() );
	updater.set_sessionkey( resp->sessionkey() );
	updater.set_keytype( ::zpds::store::K_EXTERDATA );
	if (! CheckSession(stptr, &updater, true) )
		throw zpds::BadDataException("Invalid updater or Invalid session",M_INVALID_PARAM);
	auto&& updater_can_add_tags = updater.is_admin() || updater.can_add_tags() ;
	if (! updater_can_add_tags )
		throw zpds::BadDataException("This updater cannot update tags",M_INVALID_PARAM);

	::zpds::store::TagDataTable tag_table{stptr->maindb.Get()};

	// just add the payload to payloads for easy processing
	if (!resp->payload().name().empty()) resp->add_payloads()->Swap(resp->mutable_payload());
	status->set_inputcount( resp->payloads_size() );

	// start addition , first format the data , then update
	uint64_t currtime = ZPDS_CURRTIME_MS;

	::zpds::store::TransactionT trans;
	::zpds::store::TempNameCache namecache{stptr};

	// update payloads if exists
	for (size_t i = 0 ; i<resp->payloads_size(); ++i)	{
		auto rdata = resp->mutable_payloads(i);
		if ( rdata->keytype() <= K_NONODE || rdata->keytype() >= K_LOGNODE )
			throw zpds::BadDataException("Tag must have valid keytype: " + rdata->name(),M_INVALID_PARAM);
		if ( rdata->name().empty() )
			throw zpds::BadDataException("Tag must have name: " + rdata->name(),M_INVALID_PARAM);
		if ( rdata->name() != SanitNSLower(rdata->name()) )
			throw zpds::BadDataException("Tag has invalid name: " + rdata->name(),M_INVALID_PARAM);
		if ( namecache.CheckLocal(rdata->keytype(),rdata->name()) )
			throw zpds::BadDataException("Duplicate tag found: " + rdata->name(),M_INVALID_PARAM);
		status->set_updatecount( status->updatecount() + 1 );

		::zpds::store::TagDataT tdata;
		tdata.set_name( rdata->name() );
		tdata.set_keytype( rdata->keytype() );
		bool tag_found = tag_table.GetOne(&tdata,::zpds::store::U_TAGDATA_KEYTYPE_NAME);
		if ( tag_found && (!updater.is_admin()) && (tdata.manager()!=updater.name()) )
			throw zpds::BadDataException("This exter cannot update this tag",M_INVALID_PARAM);
		if (tag_found && action == ZPDS_UACTION_CREATE)
			throw zpds::BadDataException("Create failed, Tag already exists: " + rdata->name(),M_INVALID_PARAM);
		if ((!tag_found) && (action == ZPDS_UACTION_UPDATE || action == ZPDS_UACTION_DELETE))
			throw zpds::BadDataException("Update failed, Tag does not exist: " + rdata->name(),M_INVALID_PARAM);

		if (!tag_found) {
			tdata.set_notfound(false);
			tdata.set_id(stptr->maincounter.GetNext() );
			tdata.set_created_at( currtime );
			tdata.set_manager( updater.name() );
		}
		if ( action == ZPDS_UACTION_DELETE) {
			tdata.set_is_deleted( true );
		}
		else {
			tdata.set_lang( rdata->lang() );
			tdata.set_is_allowed( rdata->is_allowed() );
			tdata.set_is_repeated( rdata->is_repeated() );
			tdata.set_is_searchable( rdata->is_searchable() );
			tdata.set_is_deleted( rdata->is_deleted() );
		}
		tdata.set_updated_at( currtime );
		// add tdata
		if (!tag_table.AddRecord(&tdata,&trans,tag_found))
			throw ::zpds::BadDataException("Cannot Insert tags data");
	}

	// commit to table
	zpds::store::StoreTrans storetrans(currtime);
	trans.set_updater( updater.name() );
	storetrans.Commit(stptr,&trans,true); // as master

	status->set_success(true);

}

/**
* AllowedFieldsFilter : sets update variable, not needed
*
*/
int32_t zpds::store::TagDataService::AllowedFieldsFilter(int32_t update)
{
	return update;
}


/**
* ReadDataAction : read TagData
*
*/
void zpds::store::TagDataService::ReadDataAction(::zpds::utils::SharedTable::pointer stptr, ::zpds::query::TagDataRespT* resp)
{
	auto status = resp->mutable_status();

	::zpds::store::ExterDataT reader;
	reader.set_name( resp->username() );
	reader.set_sessionkey( resp->sessionkey() );
	reader.set_keytype( ::zpds::store::K_EXTERDATA );

	if (! CheckSession(stptr, &reader, true) )
		throw zpds::BadDataException("Invalid reader or Invalid session",M_INVALID_PARAM);

	::zpds::store::TagDataTable data_table{stptr->maindb.Get()};

	// update payload if exists
	if (!resp->payload().name().empty()) {
		auto rdata = resp->mutable_payload();
		bool data_found = data_table.GetOne(rdata,::zpds::store::U_TAGDATA_KEYTYPE_NAME);
		if (!data_found) rdata->set_notfound(true);
		status->set_inputcount( status->inputcount() + ( rdata->notfound() ? 0 : 1 ) );
	}

	// update payloads if exists
	for (auto i = 0 ; i<resp->payloads_size(); ++i)	{
		if (resp->payloads(i).name().empty()) continue;
		auto rdata = resp->mutable_payloads(i);
		bool data_found = data_table.GetOne(rdata,::zpds::store::U_TAGDATA_KEYTYPE_NAME);
		if (!data_found) rdata->set_notfound(true);
		status->set_inputcount( status->inputcount() + ( rdata->notfound() ? 0 : 1 ) );
	}

	status->set_success(true);
}

/**
* GetAllNames : read all TagData names
*
*/
std::vector<std::string> zpds::store::TagDataService::GetAllNames(
    ::zpds::utils::SharedTable::pointer stptr, ::zpds::store::KeyTypeE keytype )
{
	auto local_stptr=stptr->share();
	::zpds::store::TagDataTable sth_table(stptr->maindb.Get());
	std::vector<std::string> svec;
	sth_table.ScanTable(0,UINT_LEAST64_MAX,[&svec,keytype](::zpds::store::TagDataT* record) {
		// only matching keytype
		if (record->keytype()==keytype)
			svec.push_back(record->name());
		return true;
	});
	return svec;
}
