/**
 * @project zapdos
 * @file src/store/ExtraAttribService.cc
 * @author S Roychowdhury < sroycode at gmail dot com >
 * @version 1.0.0
 *
 * @section LICENSE
 *
 * Copyright (c) 2018-2019 S Roychowdhury.
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
 *  ExtraAttribService.cc : Implementation of ExtraAttrib Service
 *
 */

#include <cmath>
#include <unordered_set>

#include "store/ExtraAttribService.hpp"
#include "store/ExtraAttribTable.hpp"
#include "utils/StringHelpers.hpp"

/**
* AddDataAction : data addition
*
* Updates in parallel not supported on master
*
*/
void zpds::store::ExtraAttribService::AddDataAction(
    ::zpds::utils::SharedTable::pointer stptr, ::zpds::query::ExtraParamsT* esparams)
{

	boost::unique_lock< boost::shared_mutex > lock {stptr->update_lock, boost::try_to_lock};
	if (!lock.owns_lock()) {
		throw ::zpds::BadDataException("Cannot Insert extra data in more than one query");
	}
	DLOG(INFO) << "Owns Lock" ;

	using StrIntMapT = std::unordered_map<std::string,size_t>;
	StrIntMapT IsDoneMap;
	size_t upcount=0;

	auto data = esparams->mutable_ldata();
	auto resp = esparams->mutable_lresp();
	zpds::store::TransactionT trans;

	if ( data->records_size() ==0 ) return;

	if (data->merge()) {
		DLOG(INFO) << " Merge : " << data->records_size();
		for (auto i=0; i<data->records_size(); ++i) {
			std::string hash = EncodeSecondaryKey<int32_t,int32_t,std::string>(
			                       U_EXTRAATTRIB_STYP_LANG_UNIQUEID,
			                       data->records(i).styp(), data->records(i).lang(), data->records(i).uniqueid()  );
			auto it = IsDoneMap.find(hash);
			if (it!=IsDoneMap.end()) {
				if (it->second!=i)
					bool stat = MergeOne( data->mutable_records(it->second), data->mutable_records(i) );
			}
			else {
				IsDoneMap.insert({hash,i});
			}
		}
		IsDoneMap.clear();
		for (auto i=0; i<data->records_size(); ++i) {
			std::string hash = EncodeSecondaryKey<int32_t,int32_t,std::string>(
			                       U_EXTRAATTRIB_STYP_LANG_UNIQUEID,
			                       data->records(i).styp(), data->records(i).lang(), data->records(i).uniqueid()  );
			auto it = IsDoneMap.find(hash);
			if (it!=IsDoneMap.end()) continue; // duplicate already merged
			auto styp = data->mutable_records(i)->styp();
			if ( MergeExtraAttrib(stptr,data->mutable_records(i),&trans) ) {
				++upcount;
			}
			IsDoneMap.insert({hash,0}); // no need to handle handle duplicates
		}
	}
	else {
		DLOG(INFO) << " Upsert : " << data->records_size();
		// go from bottom hence upper duplicates are discarded
		for (auto i=data->records_size()-1; i>=0; --i) {
			std::string hash = EncodeSecondaryKey<int32_t,int32_t,std::string>(
			                       U_EXTRAATTRIB_STYP_LANG_UNIQUEID,
			                       data->records(i).styp(), data->records(i).lang(), data->records(i).uniqueid()  );
			auto it = IsDoneMap.find(hash);
			if (it!=IsDoneMap.end()) continue; // duplicate
			auto styp = data->mutable_records(i)->styp();
			if ( UpsertExtraAttrib(stptr,data->mutable_records(i),&trans) ) {
				++upcount;
			}
			IsDoneMap.insert({hash,0}); // upsert does not handle duplicates
		}
	}
	zpds::store::StoreTrans storetrans;
	storetrans.Commit(stptr,&trans,true); // as master
	resp->set_count( upcount );
	resp->set_success( true );
	lock.unlock();
}

/**
* GetDataAction : data addition
*
*/
void zpds::store::ExtraAttribService::GetDataAction(
    ::zpds::utils::SharedTable::pointer stptr, ::zpds::query::ExtraParamsT* esparams)
{
	auto data = esparams->mutable_ldata();
	auto resp = esparams->mutable_lresp();
	::zpds::store::ExtraAttribTable sth_table{stptr->maindb.Get()};
	for (auto i=0; i<data->records_size(); ++i) {
		::zpds::store::ExtraAttribT* sth = data->mutable_records(i);
		if (! sth_table.GetOne(sth,U_EXTRAATTRIB_STYP_LANG_UNIQUEID) )
			sth->set_notfound(true);
	}
}

/**
* UpsertExtraAttrib : ExtraAttrib data addition
*
*/
bool zpds::store::ExtraAttribService::UpsertExtraAttrib(::zpds::utils::SharedTable::pointer stptr,
        zpds::store::ExtraAttribT* data, zpds::store::TransactionT* trans)
{
	std::string hash = EncodeSecondaryKey<int32_t,uint32_t,std::string>(
	                       U_EXTRAATTRIB_STYP_LANG_UNIQUEID,
	                       data->styp(), data->lang(), data->uniqueid()
	                   );

	uint64_t currtime = ZPDS_CURRTIME_MS;
	::zpds::store::ExtraAttribT sth;
	sth.set_styp ( data->styp() );
	sth.set_lang ( data->lang() );
	sth.set_uniqueid ( data->uniqueid() );
	::zpds::store::ExtraAttribTable sth_table{stptr->maindb.Get()};
	bool hash_found = sth_table.GetOne(&sth,U_EXTRAATTRIB_STYP_LANG_UNIQUEID);
	data->set_id( hash_found ? sth.id() : stptr->maincounter.GetNext() );
	data->set_updated_at( currtime );
	data->set_created_at( hash_found ? sth.created_at() : currtime );
	data->set_notfound(false);
	if (!sth_table.AddRecord(data,trans,hash_found))
		throw ::zpds::BadDataException("Cannot Insert data");
	return true;

}

/**
* MergeExtraAttrib : ExtraAttrib data addition by merge
*
*/
bool zpds::store::ExtraAttribService::MergeExtraAttrib(::zpds::utils::SharedTable::pointer stptr,
        zpds::store::ExtraAttribT* data, zpds::store::TransactionT* trans)
{
	std::string hash = EncodeSecondaryKey<int32_t,int32_t,std::string>(U_EXTRAATTRIB_STYP_LANG_UNIQUEID,
	                   data->styp(), data->lang(), data->uniqueid()
	                                                                  );
	uint64_t currtime = ZPDS_CURRTIME_MS;
	::zpds::store::ExtraAttribT sth;
	sth.set_styp ( data->styp() );
	sth.set_lang ( data->lang() );
	sth.set_uniqueid ( data->uniqueid() );
	::zpds::store::ExtraAttribTable sth_table{stptr->maindb.Get()};
	bool hash_found = sth_table.GetOne(&sth,U_EXTRAATTRIB_STYP_LANG_UNIQUEID);
	if (hash_found) {
		// merge to original and swap
		bool stat = MergeOne( &sth, data );
		if (!stat) return false; // nothing to merge , identical records
		sth.Swap(data);
	}
	else {
		data->set_id( stptr->maincounter.GetNext() );
	}
	data->set_updated_at( currtime );
	data->set_created_at( hash_found ? sth.created_at() : currtime );
	data->set_notfound(false);
	if (!sth_table.AddRecord(data,trans,hash_found))
		throw ::zpds::BadDataException("Cannot Insert data");
	return true;

}

/**
* MergeOne : ExtraAttrib data merging
*
*/
bool zpds::store::ExtraAttribService::MergeOne(zpds::store::ExtraAttribT* mto, zpds::store::ExtraAttribT* mfrom)
{
	if ( ( mto->styp() != mfrom->styp() ) || ( mto->uniqueid() != mfrom->uniqueid() ) || ( mto->lang() != mfrom->lang() ) )
		throw ::zpds::BadDataException("Mismatch of id in data");

	using StrMapT = std::unordered_map<std::string, ::zpds::store::KeyValueT* >;
	StrMapT dmap;

	size_t change=0;

	for (auto i = 0 ; i < mto->attr_size() ; ++i ) {
		if ( dmap.find( mto->mutable_attr(i)->key() ) != dmap.end() )
			throw ::zpds::BadDataException("Merge not possible for repeating keys");
		dmap[ mto->mutable_attr(i)->key() ] =  mto->mutable_attr(i);
	}

	for (auto i = 0 ; i < mfrom->attr_size() ; ++i ) {
		std::string key = mfrom->attr(i).key();
		
		auto it = dmap.find(key);
		if ( it == dmap.end() ) {
			auto newa = mto->add_attr( );
			newa->set_key ( mfrom->attr(i).key() );
			newa->set_value ( mfrom->attr(i).value() );
			++change;
		} else if ( it->second->value() != mfrom->attr(i).value() ) {
			dmap[key]->set_value( mfrom->attr(i).value() );
			++change;
		} else {
			// no change
		}
	}

	// is_deleted
	if ( (mfrom->is_deleted() ) && (mto->is_deleted() != mfrom->is_deleted()) ) {
		mto->set_is_deleted( mfrom->is_deleted() ); ;
		++change;
	}

	return (change>0);
}
