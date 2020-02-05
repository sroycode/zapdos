/**
 * @project zapdos
 * @file src/store/LookupRecordService.cc
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
 *  LookupRecordService.cc : Implementation of LookupRecord Service
 *
 */

#include <cmath>
#include <unordered_set>

#include "store/LookupRecordService.hpp"
#include "store/LookupRecordTable.hpp"
#include "utils/StringHelpers.hpp"

#include "store/ExtraAttribTable.hpp"

/**
* AddDataAction : data addition
*
* Updates in parallel not supported on master
*  Slave is anyway single threaded
*  1. Xapian is single update
*  2. LookupRecord data updates with merge can mess up if parallel
*
*/
void zpds::store::LookupRecordService::AddDataAction(
    ::zpds::utils::SharedTable::pointer stptr, ::zpds::query::LookupParamsT* esparams)
{

	boost::unique_lock< boost::shared_mutex > lock {stptr->update_lock, boost::try_to_lock};
	if (!lock.owns_lock()) {
		throw ::zpds::BadDataException("Cannot Insert lookup data in more than one query");
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
			                       U_LOOKUPRECORD_STYP_LANG_UNIQUEID,
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
			                       U_LOOKUPRECORD_STYP_LANG_UNIQUEID,
			                       data->records(i).styp(), data->records(i).lang(), data->records(i).uniqueid()  );
			auto it = IsDoneMap.find(hash);
			if (it!=IsDoneMap.end()) continue; // duplicate already merged
			auto styp = data->mutable_records(i)->styp();
			if ( MergeLookupRecord(stptr,data->mutable_records(i),&trans) ) {
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
			                       U_LOOKUPRECORD_STYP_LANG_UNIQUEID,
			                       data->records(i).styp(), data->records(i).lang(), data->records(i).uniqueid()  );
			auto it = IsDoneMap.find(hash);
			if (it!=IsDoneMap.end()) continue; // duplicate
			auto styp = data->mutable_records(i)->styp();
			if ( UpsertLookupRecord(stptr,data->mutable_records(i),&trans) ) {
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
void zpds::store::LookupRecordService::GetDataAction(
    ::zpds::utils::SharedTable::pointer stptr, ::zpds::query::LookupParamsT* esparams)
{
	auto data = esparams->mutable_ldata();
	auto resp = esparams->mutable_lresp();
	::zpds::store::LookupRecordTable sth_table{stptr->maindb.Get()};
	::zpds::store::ExtraAttribTable att_table{stptr->maindb.Get()};
	for (auto i=0; i<data->records_size(); ++i) {
		::zpds::store::LookupRecordT* sth = data->mutable_records(i);
		if (! sth_table.GetOne(sth,U_LOOKUPRECORD_STYP_LANG_UNIQUEID) ) {
			sth->set_notfound(true);
			continue;
		}

		// extrattribs
		if ( esparams->extra() ) {
			::zpds::store::ExtraAttribT att;
			att.set_styp( sth->styp() );
			att.set_lang( sth->lang() );
			att.set_uniqueid( sth->uniqueid() );
			if (att_table.GetOne(&att,U_EXTRAATTRIB_STYP_LANG_UNIQUEID) ) {
				sth->mutable_attr()->Swap( att.mutable_attr() );
			}
		}


	}
}

/**
* GetIndexDataAction : data addition
*
*/
void zpds::store::LookupRecordService::GetIndexDataAction(
    ::zpds::utils::SharedTable::pointer stptr,
    ::zpds::query::LookupParamsT* esparams)
{
	auto data = esparams->mutable_ldata();
	auto resp = esparams->mutable_lresp();
	::zpds::store::LookupRecordTable sth_table{stptr->maindb.Get()};

	// collect the ids from index
	auto styp = esparams->styp();
	auto lang = esparams->lang();
	const google::protobuf::EnumDescriptor *e = ::zpds::search::SourceTypeE_descriptor();

	for (auto j=0; j< e->value_count(); ++j) {
		if ( e->value(j)->number() == ::zpds::search::SourceTypeE::S_NONE ) continue;
		if ( ( esparams->styp() != ::zpds::search::SourceTypeE::S_NONE ) && ( esparams->styp() != e->value(j)->number() )) continue;
		std::string keymatch = sth_table.EncodeSecondaryKey<int32_t,int32_t>(
		                           I_LOOKUPRECORD_STYP_LANG_LASTUP,
		                           e->value(j)->number(), lang
		                       );
		std::string keystart = sth_table.EncodeSecondaryKey<int32_t, int32_t, uint64_t, uint64_t>(
		                           I_LOOKUPRECORD_STYP_LANG_LASTUP,
		                           e->value(j)->number(), lang,
		                           esparams->last_up(), esparams->last_id()
		                       );

		auto last_up = esparams->last_up();
		auto last_id = esparams->last_id();

		sth_table.ScanIndex( keystart, keymatch, esparams->limit(),
		[data, last_up,last_id](::zpds::store::NodeT* record) {
			// if ( record->last_id() == last_id &&  record->last_up() == last_up ) return true;
			data->add_records()->set_id( record->id() );
			return true;
		});

	}

	::zpds::store::ExtraAttribTable att_table{stptr->maindb.Get()};

	// populate from data
	for (auto i=0; i<data->records_size(); ++i) {
		::zpds::store::LookupRecordT* sth = data->mutable_records(i);
		if (! sth_table.GetOne(sth,K_LOOKUPRECORD) ) {
			sth->set_notfound(true);
			continue;
		}
		// geohash9
		if (sth->lat() !=0.0 && sth->lon() !=0.0) {
			try {
				sth->set_geohash9( gh.Encode( sth->lat(), sth->lon(), 9) );
				sth->set_location( PrintWithComma( sth->lat(), sth->lon() ));
			}
			catch (::zpds::BaseException& e) {
				// if bad lat lon erase
				sth->clear_lat();
				sth->clear_lon();
			}
		}

		// suggest
		sth->set_suggest( PrintWithComma( sth->fld_name(), sth->fld_area(), sth->city(), sth->country() ) );

		// extrattribs
		if ( esparams->extra() ) {
			::zpds::store::ExtraAttribT att;
			att.set_styp( sth->styp() );
			att.set_lang( sth->lang() );
			att.set_uniqueid( sth->uniqueid() );
			if (att_table.GetOne(&att,U_EXTRAATTRIB_STYP_LANG_UNIQUEID) ) {
				sth->mutable_attr()->Swap( att.mutable_attr() );
			}
		}
		// others
	}
}

/**
* UpsertLookupRecord : LookupRecord data addition
*
*/
bool zpds::store::LookupRecordService::UpsertLookupRecord(::zpds::utils::SharedTable::pointer stptr,
        zpds::store::LookupRecordT* data, zpds::store::TransactionT* trans)
{
	std::string hash = EncodeSecondaryKey<int32_t,uint32_t,std::string>(
	                       U_LOOKUPRECORD_STYP_LANG_UNIQUEID,
	                       data->styp(), data->lang(), data->uniqueid()
	                   );

	uint64_t currtime = ZPDS_CURRTIME_MS;
	::zpds::store::LookupRecordT sth;
	sth.set_styp ( data->styp() );
	sth.set_lang ( data->lang() );
	sth.set_uniqueid ( data->uniqueid() );
	::zpds::store::LookupRecordTable sth_table{stptr->maindb.Get()};
	bool hash_found = sth_table.GetOne(&sth,U_LOOKUPRECORD_STYP_LANG_UNIQUEID);
	data->set_id( hash_found ? sth.id() : stptr->maincounter.GetNext() );
	data->set_updated_at( currtime );
	data->set_created_at( hash_found ? sth.created_at() : currtime );
	data->set_notfound(false);
	if (!sth_table.AddRecord(data,trans,hash_found))
		throw ::zpds::BadDataException("Cannot Insert data");
	return true;

}

/**
* MergeLookupRecord : LookupRecord data addition by merge
*
*/
bool zpds::store::LookupRecordService::MergeLookupRecord(::zpds::utils::SharedTable::pointer stptr,
        zpds::store::LookupRecordT* data, zpds::store::TransactionT* trans)
{
	std::string hash = EncodeSecondaryKey<int32_t,int32_t,std::string>(U_LOOKUPRECORD_STYP_LANG_UNIQUEID,
	                   data->styp(), data->lang(), data->uniqueid()
	                                                                  );
	uint64_t currtime = ZPDS_CURRTIME_MS;
	::zpds::store::LookupRecordT sth;
	sth.set_styp ( data->styp() );
	sth.set_lang ( data->lang() );
	sth.set_uniqueid ( data->uniqueid() );
	::zpds::store::LookupRecordTable sth_table{stptr->maindb.Get()};
	bool hash_found = sth_table.GetOne(&sth,U_LOOKUPRECORD_STYP_LANG_UNIQUEID);
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
* MergeOne : LookupRecord data merging
*
*/
bool zpds::store::LookupRecordService::MergeOne(zpds::store::LookupRecordT* mto, zpds::store::LookupRecordT* mfrom)
{
	if ( ( mto->styp() != mfrom->styp() ) || ( mto->uniqueid() != mfrom->uniqueid() ) || ( mto->lang() != mfrom->lang() ) )
		throw ::zpds::BadDataException("Mismatch of id in data");

	using StrSetT = std::unordered_set<std::string>;
	StrSetT dset;

	size_t change=0;

	// ccode
	if ((! mfrom->ccode().empty()) && (mto->ccode() != mfrom->ccode()) ) {
		mto->set_ccode( mfrom->ccode() ); ;
		++change;
	}

	// scode
	if ((! mfrom->scode().empty()) && (mto->scode() != mfrom->scode()) ) {
		mto->set_scode( mfrom->scode() ); ;
		++change;
	}

	// city
	if ((! mfrom->city().empty()) && (mto->city() != mfrom->city()) ) {
		mto->set_city( mfrom->city() ); ;
		++change;
	}

	// country
	if ((! mfrom->country().empty()) && (mto->country() != mfrom->country()) ) {
		mto->set_country( mfrom->country() ); ;
		++change;
	}

	// address
	if ((! mfrom->address().empty()) && (mto->address() != mfrom->address()) ) {
		mto->set_address( mfrom->address() ); ;
		++change;
	}

	// pincode
	if ((! mfrom->pincode().empty()) && (mto->pincode() != mfrom->pincode()) ) {
		mto->set_pincode( mfrom->pincode() ); ;
		++change;
	}

	// fld_name
	if ((! mfrom->fld_name().empty()) && (mto->fld_name() != mfrom->fld_name()) ) {
		mto->set_fld_name( mfrom->fld_name() ); ;
		++change;
	}

	// fld_area
	if ((! mfrom->fld_area().empty()) && (mto->fld_area() != mfrom->fld_area()) ) {
		mto->set_fld_area( mfrom->fld_area() ); ;
		++change;
	}

	// landmark
	if ((! mfrom->landmark().empty()) && (mto->landmark() != mfrom->landmark()) ) {
		mto->set_landmark( mfrom->landmark() ); ;
		++change;
	}

	// importance
	if ( (mfrom->importance() >0 ) && (mto->importance() != mfrom->importance()) ) {
		mto->set_importance( mfrom->importance() ); ;
		++change;
	}

	// accuracy
	if ( (mfrom->accuracy() >0 ) && (mto->accuracy() != mfrom->accuracy()) ) {
		mto->set_accuracy( mfrom->accuracy() ); ;
		++change;
	}

	// lat
	if ( (mfrom->lat() !=0 ) && (mto->lat() != mfrom->lat()) ) {
		mto->set_lat( mfrom->lat() ); ;
		++change;
	}

	// lon
	if ( (mfrom->lon() !=0 ) && (mto->lon() != mfrom->lon()) ) {
		mto->set_lon( mfrom->lon() ); ;
		++change;
	}

	// rating
	if ( (mfrom->rating() >0 ) && (mto->rating() != mfrom->rating()) ) {
		mto->set_rating( mfrom->rating() );
		++change;
	}

	// is_in_place
	if ((! mfrom->is_in_place().empty()) && (mto->is_in_place() != mfrom->is_in_place()) ) {
		mto->set_is_in_place( mfrom->is_in_place() ); ;
		++change;
	}

	// tags
	if ((! mfrom->tags().empty()) && (mto->tags() != mfrom->tags()) ) {
		mto->set_tags( mfrom->tags() ); ;
		++change;
	}

	// osm_id
	if (( mfrom->osm_id() != 0) && (mto->osm_id() != mfrom->osm_id()) ) {
		mto->set_osm_id( mfrom->osm_id() ); ;
		++change;
	}

	// osm_key
	if ((! mfrom->osm_key().empty()) && (mto->osm_key() != mfrom->osm_key()) ) {
		mto->set_osm_key( mfrom->osm_key() ); ;
		++change;
	}

	// osm_value
	if ((! mfrom->osm_value().empty()) && (mto->osm_value() != mfrom->osm_value()) ) {
		mto->set_osm_value( mfrom->osm_value() ); ;
		++change;
	}

	// osm_type
	if ((! mfrom->osm_type().empty()) && (mto->osm_type() != mfrom->osm_type()) ) {
		mto->set_osm_type( mfrom->osm_type() ); ;
		++change;
	}

	// alias_styp
	if (mto->alias_styp() != mfrom->alias_styp() ) {
		mto->set_alias_styp( mfrom->alias_styp() ); ;
		++change;
	}

	// alias_uniqueid
	if ((! mfrom->alias_uniqueid().empty()) && (mto->alias_uniqueid() != mfrom->alias_uniqueid()) ) {
		mto->set_alias_uniqueid( mfrom->alias_uniqueid() ); ;
		++change;
	}

	// geometry
	if ((! mfrom->geometry().empty()) && (mto->geometry() != mfrom->geometry()) ) {
		mto->set_geometry( mfrom->geometry() ); ;
		++change;
	}

	// is_deleted
	if ( (mfrom->is_deleted() ) && (mto->is_deleted() != mfrom->is_deleted()) ) {
		mto->set_is_deleted( mfrom->is_deleted() ); ;
		++change;
	}

	return (change>0);
}
