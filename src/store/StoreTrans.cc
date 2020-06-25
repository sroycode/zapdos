/**
 * @project zapdos
 * @file src/store/StoreTrans.cc
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
 *  StoreTrans.cc : Implementation for transaction store
 *
 */
#include "store/StoreTrans.hpp"
#include "hrpc/HrpcClient.hpp"

#include "store/TagDataService.hpp"
#include "store/ExterCredService.hpp"
#include "store/UserCredService.hpp"
#include "store/CategoryService.hpp"

#include "store/TempNameCache.hpp"
#include "utils/PrintWith.hpp"

#ifdef ZPDS_BUILD_WITH_XAPIAN
#include "search/IndexLocal.hpp"
#include "search/IndexWiki.hpp"
const ::zpds::search::IndexLocal indexlocal;
const ::zpds::search::IndexWiki indexwiki;
#endif

/**
* Constructor with currtime
*
*/
zpds::store::StoreTrans::StoreTrans(uint64_t currtime) : use_currtime(currtime) {}

/**
* Commit : write both transaction and log
*
*/
void zpds::store::StoreTrans::StoreTrans::Commit(::zpds::utils::SharedTable::pointer stptr, TransactionT* trans, bool is_master)
{

	// For clean shutdown, to check
	// keep a lock on slave updates
	::zpds::store::TempNameCache namecache{stptr};
	if ( ! stptr->is_master.Get() ) {
		namecache.MultiTypeLock(ZPDS_LOCK_SLAVE_UPDATES);
	}

	if (trans->item_size()==0) return;
	if (!CommitLog(stptr,trans,is_master))
		throw ::zpds::BadDataException("Insert failed for log");
	if (!CommitData(stptr,trans))
		throw ::zpds::BadDataException("Insert failed for data");
	// replicate this data on lastslave
	auto lastslave = stptr->lastslave.Get();
	if (is_master && stptr->is_master.Get() && (lastslave.length()>0)) {
		zpds::hrpc::HrpcClient hclient;
		bool status = hclient.SendToRemote(stptr,lastslave,::zpds::hrpc::R_BUFFTRANS,trans,true);
		if (!status) stptr->lastslave.Set("");
	}
	// add to cache
	AddToCache(stptr,trans);

}

/**
* CommitLog : write log ensure id is sequentially generated
*
*/
bool zpds::store::StoreTrans::StoreTrans::CommitLog(::zpds::utils::SharedTable::pointer stptr, TransactionT* trans, bool is_master)
{
	if (trans->item_size()==0) return true;
	dbpointer logdb = stptr->logdb.Get();
	// the logcounter is incremented just before writing
	trans->set_id( stptr->logcounter.GetNext() );
	if (is_master) trans->set_ts( use_currtime );
	std::string value;
	trans->SerializeToString(&value);

	std::string blank = std::to_string(trans->id());

	usemydb::WriteBatch batch;
	batch.Put(EncodePrimaryKey(K_LOGNODE,trans->id()),value);
	batch.Put(EncodeSecondaryKey<int64_t, int64_t>(I_LOGNODE_TS, trans->ts(), trans->id()),blank);
	usemydb::Status s = logdb->Write(usemydb::WriteOptions(), &batch);
	return s.ok();
}

/**
* CommitData : write data
*
*/
bool zpds::store::StoreTrans::StoreTrans::CommitData(::zpds::utils::SharedTable::pointer stptr, TransactionT* trans)
{
	if (trans->item_size()==0) return true;
	usemydb::WriteBatch batch;
	for (auto i=0; i<trans->item_size(); ++i) {
		if (! trans->mutable_item(i)->to_del() ) {
			batch.Put(trans->mutable_item(i)->key(), trans->mutable_item(i)->value() );
		}
		else {
			batch.Delete(trans->mutable_item(i)->key());
		}
	}
	dbpointer maindb = stptr->maindb.Get();
	usemydb::Status s = maindb->Write(usemydb::WriteOptions(), &batch);
	return s.ok();
}

/**
* ReadLog : read from log in sequence
*
*/
void zpds::store::StoreTrans::StoreTrans::ReadLog(::zpds::utils::SharedTable::pointer stptr, TransListT* tlist)
{
	auto logc = stptr->logcounter.Get();
	if (tlist->lastid()>logc)
		throw ::zpds::BadDataException("Remote Log Counter ahead");

	// if no change return
	if (tlist->lastid()==logc) {
		tlist->set_ts( ZPDS_CURRTIME_MS );
		tlist->set_currid( logc );
		tlist->set_limit( 0 );
		return;
	}

	dbpointer logdb = stptr->logdb.Get();
	std::shared_ptr<usemydb::Iterator> it(logdb->NewIterator(usemydb::ReadOptions()));
	// start from lastid and skip first, if zero will hit and skip fence
	std::string keystart = EncodePrimaryKey(K_LOGNODE,tlist->lastid());
	usemydb::Slice start = usemydb::Slice ( keystart.c_str(), ZPDS_LEN_NODE_KEY);
	usemydb::Slice match = usemydb::Slice ( keystart.c_str(), ZPDS_KEYID_LEN ); // only this keytype

	uint64_t counter=0;

	for (it->Seek(start); it->Valid() && it->key().starts_with(match) ; it->Next()) {
		TransactionT record;
		if (!record.ParseFromString(it->value().ToString())) continue;
		DLOG(INFO) << " ### " << tlist->lastid() << " -- " << record.id();
		if (record.id() == tlist->lastid()) continue; // skips the first
		if ((tlist->lastid()+1)!=record.id() ) break; // out of sequence
		tlist->set_lastid( record.id() );
		record.Swap(tlist->add_trans());
		if (++counter > tlist->limit() ) break; // break on exceed
	}
	tlist->set_currid( stptr->logcounter.Get() );
	tlist->set_ts( ZPDS_CURRTIME_MS );
	if (counter) tlist->set_limit( counter-1 );
}

/**
* ReadOne : read a single transaction
*
*/
void zpds::store::StoreTrans::ReadOne(::zpds::utils::SharedTable::pointer stptr, TransactionT* trans)
{
	std::string value;
	dbpointer logdb = stptr->logdb.Get();
	auto s = logdb->Get(usemydb::ReadOptions(), EncodePrimaryKey(K_LOGNODE,trans->id()), &value);
	if (!s.ok()) {
		trans->set_notfound( true );
		return;
	}
	if (! trans->ParseFromString(value) ) trans->set_notfound( true );
}

/**
* AddToCache : adds a transaction to cache
*
*/
void zpds::store::StoreTrans::AddToCache(::zpds::utils::SharedTable::pointer stptr, ::zpds::store::TransactionT* trans)
{
	if (trans->item_size()==0) return;
	// bool only_if_exists=true;

	for (auto i=0; i<trans->item_size(); ++i) {
		std::string key = trans->mutable_item(i)->key();
		bool to_del = trans->mutable_item(i)->to_del() ;
		if (!CheckPrimaryKey(key)) continue;
		auto kp = DecodePrimaryKey( key );
		switch(kp.first) {
		default:
			break;
		case K_TAGDATA: {
			::zpds::store::TagDataT record;
			if (!record.ParseFromString(trans->mutable_item(i)->value())) continue;
			// delete from cache
			std::string&& name = EncodeSecondaryKey<int32_t, std::string>(U_TAGDATA_KEYTYPE_NAME, record.keytype(), record.name());
			stptr->dbcache->DelAssoc( name );
			break;
		}

		case K_CATEGORY: {
			::zpds::store::CategoryT record;
			if (!record.ParseFromString(trans->mutable_item(i)->value())) continue;
			// delete from cache
			std::string&& name = EncodeSecondaryKey<std::string>(U_CATEGORY_NAME, record.name());
			stptr->dbcache->DelAssoc( name );
			break;
		}

		case K_EXTERDATA: {
			::zpds::store::ExterDataT record;
			if (!record.ParseFromString(trans->mutable_item(i)->value())) continue;
			// delete from cache
			std::string&& name = EncodeSecondaryKey<std::string>(U_EXTERDATA_NAME, record.name());
			stptr->dbcache->DelAssoc( name );
			break;
		}

		case K_USERDATA: {
			::zpds::store::UserDataT record;
			if (!record.ParseFromString(trans->mutable_item(i)->value())) continue;
			// delete from cache
			std::string&& name = EncodeSecondaryKey<std::string>(U_USERDATA_NAME, record.name());
			stptr->dbcache->DelAssoc( name );
			break;
		}

		case K_LOCALDATA: {
			// as of now this needs only xapian , if there are more place this line inside ZPDS_BUILD_WITH_XAPIAN
			if ( stptr->no_xapian.Get() ) continue;

			::zpds::store::ItemDataT record;
			if (!record.ParseFromString(trans->mutable_item(i)->value())) continue;
#ifdef ZPDS_BUILD_WITH_XAPIAN
			// add to search
			if ( to_del ) indexlocal.DelRecord(stptr, &record);
			else indexlocal.AddRecord(stptr, &record);
#endif
			break;
		}

		case K_WIKIDATA: {
			// as of now this needs only xapian , if there are more place this line inside ZPDS_BUILD_WITH_XAPIAN
			if ( stptr->no_xapian.Get() ) continue;

			::zpds::store::ItemDataT record;
			if (!record.ParseFromString(trans->mutable_item(i)->value())) continue;
#ifdef ZPDS_BUILD_WITH_XAPIAN
			// add to search
			if ( to_del ) indexwiki.DelRecord(stptr, &record);
			else indexwiki.AddRecord(stptr, &record);
#endif
			break;
		}

		// end of switch

		}
	}
#ifdef ZPDS_BUILD_WITH_XAPIAN
	if (stptr->force_commit.Get()) {
		stptr->force_commit.Set(false);
		stptr->xapdb->CommitData();
	}
#endif
}
