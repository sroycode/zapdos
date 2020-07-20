/**
 * @project zapdos
 * @file src/store/CategoryService.cc
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
 *  CategoryService.cc : Category Data Manipulation impl
 *
 */
#include "store/CategoryService.hpp"
#include "store/ServiceDefine.hh"

#include "store/CategoryTable.hpp"
#include "store/StoreTrans.hpp"
#include "store/TempNameCache.hpp"
#include "utils/SplitWith.hpp"
#include "utils/PrintWith.hpp"


/**
* Get : gets the data
*
*/
void zpds::store::CategoryService::Get(::zpds::utils::SharedTable::pointer stptr, ::zpds::store::CategoryT* data) const
{
	std::string temp;
	bool category_found= stptr->dbcache->GetAssoc(EncodeSecondaryKey<std::string>(U_CATEGORY_NAME, data->name() ), temp);
	if (category_found) {
		category_found = data->ParseFromString(temp);
	}
	else {
		::zpds::store::CategoryTable data_table{stptr->maindb.Get()};
		category_found = data_table.GetOne(data,::zpds::store::U_CATEGORY_NAME);
		if (category_found) {
			stptr->dbcache->SetAssoc(
			    EncodeSecondaryKey<std::string>(U_CATEGORY_NAME, data->name()), Pack(data) ) ;
		}
	}
	if (!category_found) data->set_notfound(true);
}

/**
* Pack : shrink to bare essentials for packing into cache
*
*/
std::string zpds::store::CategoryService::Pack(::zpds::store::CategoryT* data) const
{
	::zpds::store::CategoryT tocache;
	tocache.set_id( data->id() );
	tocache.set_name( data->name() );
	tocache.set_cansee_type( data->cansee_type() );
	tocache.set_cansee_price( data->cansee_price() );
	tocache.set_is_deleted( data->is_deleted() );
	std::string packed;
	tocache.SerializeToString(&packed);
	return packed;
}

/**
* ManageDataAction : sets categorydata
*
*/
void zpds::store::CategoryService::ManageDataAction(::zpds::utils::SharedTable::pointer stptr, ::zpds::query::CategoryRespT* resp)
{
	if (!stptr->is_master.Get())
		throw ::zpds::BadCodeException("Category change is master only");

	auto status = resp->mutable_status();
	int32_t action=0;
	bool merge=false;
	std::tie(action,merge)=WriteActionFilter(resp->action());

	::zpds::store::ExterDataT updater;
	updater.set_name( resp->username() );
	updater.set_sessionkey( resp->sessionkey() );
	updater.set_keytype( ::zpds::store::K_EXTERDATA );
	if (! CheckSession(stptr, &updater,true) )
		throw zpds::BadDataException("Invalid updater or Invalid session",M_INVALID_PARAM);
	auto&& updater_can_add_category = updater.is_admin() || updater.can_add_category() ;
	if (! updater_can_add_category )
		throw zpds::BadDataException("This updater cannot update category",M_INVALID_PARAM);

	::zpds::store::CategoryTable category_table{stptr->maindb.Get()};

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
		if ( rdata->name().empty() )
			throw zpds::BadDataException("Category must have name: " + rdata->name(),M_INVALID_PARAM);
		if (rdata->name() != SanitNSLower(rdata->name()) )
			throw zpds::BadDataException("Category has invalid name: " + rdata->name(),M_INVALID_PARAM);

		// checks
		if (namecache.CheckLocal(::zpds::store::K_CATEGORY,rdata->name()))
			throw zpds::BadDataException("Duplicate category found: " + rdata->name(),M_INVALID_PARAM);
		if (!namecache.ReserveName(::zpds::store::K_CATEGORY,rdata->name(),false))
			throw zpds::BadDataException("This category is being worked on: " + rdata->name(),M_INVALID_PARAM);
		status->set_updatecount( status->updatecount() + 1 );

		::zpds::store::CategoryT tdata;
		tdata.set_name( rdata->name() );
		bool category_found = category_table.GetOne(&tdata,::zpds::store::U_CATEGORY_NAME);
		if ( category_found && (!updater.is_admin()) && (tdata.manager()!=updater.name()) )
			throw zpds::BadDataException("This exter cannot update this category",M_INVALID_PARAM);
		if (category_found && action == ZPDS_UACTION_CREATE)
			throw zpds::BadDataException("Create failed, category already exists: " + rdata->name(),M_INVALID_PARAM);
		if ((!category_found) && (action == ZPDS_UACTION_UPDATE || action == ZPDS_UACTION_DELETE))
			throw zpds::BadDataException("Update failed, category does not exist: " + rdata->name(),M_INVALID_PARAM);

		if (!category_found) {
			tdata.set_notfound(false);
			tdata.set_id(stptr->maincounter.GetNext() );
			tdata.set_created_at( currtime );
			tdata.set_manager( updater.name() );
		}
		if ( action == ZPDS_UACTION_DELETE) {
			tdata.set_is_deleted( true );
		}
		else {
			if ( !( (rdata->lang() == ::zpds::search::EN) && merge) )
				tdata.set_lang( rdata->lang() );
			if ( !(rdata->description().empty() && merge) )
				tdata.set_description( rdata->description() );
			if ( !( (rdata->cansee_type() == ::zpds::store::C_BASIC) && merge) )
				tdata.set_cansee_type( rdata->cansee_type() );
			if ( !( (rdata->cansee_price()>0) && merge) )
				tdata.set_cansee_price( rdata->cansee_price() );
			if ( !(rdata->is_deleted() && merge) )
				tdata.set_is_deleted( rdata->is_deleted() );
		}
		tdata.set_updated_at( currtime );
		// add tdata
		if (!category_table.AddRecord(&tdata,&trans,category_found))
			throw ::zpds::BadDataException("Cannot Insert category data");
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
int32_t zpds::store::CategoryService::AllowedFieldsFilter(int32_t update)
{
	return update;
}


/**
* ReadDataAction : read Category
*
*/
void zpds::store::CategoryService::ReadDataAction(::zpds::utils::SharedTable::pointer stptr, ::zpds::query::CategoryRespT* resp)
{
	auto status = resp->mutable_status();

	::zpds::store::ExterDataT reader;
	reader.set_name( resp->username() );
	reader.set_sessionkey( resp->sessionkey() );
	reader.set_keytype( ::zpds::store::K_EXTERDATA );

	if (! CheckSession(stptr, &reader, true) )
		throw zpds::BadDataException("Invalid reader or Invalid session",M_INVALID_PARAM);

	::zpds::store::CategoryTable data_table{stptr->maindb.Get()};

	// update payload if exists
	if (!resp->payload().name().empty()) {
		auto rdata = resp->mutable_payload();
		bool data_found = data_table.GetOne(rdata,::zpds::store::U_CATEGORY_NAME);
		if (!data_found) rdata->set_notfound(true);
		status->set_inputcount( status->inputcount() + ( rdata->notfound() ? 0 : 1 ) );
	}

	// update payloads if exists
	for (auto i = 0 ; i<resp->payloads_size(); ++i)	{
		if (resp->payloads(i).name().empty()) continue;
		auto rdata = resp->mutable_payloads(i);
		bool data_found = data_table.GetOne(rdata,::zpds::store::U_CATEGORY_NAME);
		if (!data_found) rdata->set_notfound(true);
		status->set_inputcount( status->inputcount() + ( rdata->notfound() ? 0 : 1 ) );
	}

	status->set_success(true);
}

/**
* GetAllNames : read all Category names
*
*/
std::vector<std::string> zpds::store::CategoryService::GetAllNames(
    ::zpds::utils::SharedTable::pointer stptr, ::zpds::store::KeyTypeE keytype )
{
	auto local_stptr=stptr->share();
	::zpds::store::CategoryTable sth_table(stptr->maindb.Get());
	std::vector<std::string> svec;
	sth_table.ScanTable(0,UINT_LEAST64_MAX,[&svec](::zpds::store::CategoryT* record) {
		svec.push_back(record->name());
		return true;
	});
	return svec;
}
