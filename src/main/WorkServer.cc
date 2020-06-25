/**
 * @project zapdos
 * @file src/main/WorkServer.cc
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
 *  WorkServer.cc : Work Server handles all initialization
 *
 */
#include <gflags/gflags.h>
DECLARE_bool(warmcache);

#include "async++.h"
#include "WorkServer.hpp"
#include "store/StoreLevel.hpp"
#include "utils/B64String.hpp"

#include "store/ExterCredService.hpp"
#include "store/ExterDataService.hpp"

#ifdef ZPDS_BUILD_WITH_XAPIAN
#include "search/SearchCache.hpp"
#endif

zpds::work::WorkServer::WorkServer(zpds::utils::SharedTable::pointer stptr)
	: zpds::utils::ServerBase(stptr)
{
	DLOG(INFO) << "WorkServer Created" << std::endl;
}

zpds::work::WorkServer::~WorkServer ()
{
	DLOG(INFO) << "WorkServer Deleted" << std::endl;
}

const std::string zpds::work::WorkServer::GetSection()
{
	return "work";
}

const zpds::utils::ServerBase::ParamsListT zpds::work::WorkServer::GetRequire()
{
	zpds::utils::ServerBase::ParamsListT p{"datadir","cachesize","default_adminpass" };

#ifdef ZPDS_USE_SEPARATE_LOGDB
	p.insert(p.end(), { "logdatadir","logcachesize" });
#endif

	return p;
}


void zpds::work::WorkServer::init(zpds::utils::ServerBase::ParamsListT params)
{

	if (params.size() != this->GetRequire().size() )
		throw zpds::ConfigException("WorkServer: params and required size mismatch");

	try {
		size_t pcount=0;
		std::string datadir = params[pcount];
		size_t cachesize = std::stoul(params[++pcount]);
		if (datadir.empty()) throw zpds::InitialException("datadir is needed");
		std::string default_adminpass = params[++pcount];


#ifdef ZPDS_USE_SEPARATE_LOGDB
		std::string logdatadir = params[++pcount];
		size_t logcachesize = std::stoul(params[++pcount]);
		if (logdatadir.empty()) throw zpds::InitialException("logdatadir is needed");
#endif

		// db setup
		LOG(INFO) << "Finding last keys  may take abnormally long : " ;
		uint64_t last_pkey=0;
		uint64_t last_lkey=0;
		zpds::store::StoreLevel s{NULL};
		s.Initialize(datadir,cachesize,last_pkey,last_lkey);
		sharedtable->maincounter.Set ( (last_pkey>0) ? last_pkey : 1 );
		sharedtable->maindb.Set( s.getDB() );

		// logdb
#ifdef ZPDS_USE_SEPARATE_LOGDB
		zpds::store::StoreLevel p {NULL};
		p.Initialize(logdatadir,logcachesize,last_pkey,last_lkey);
		sharedtable->logdb.Set( p.getDB() );
#else
		sharedtable->logdb.Set( s.getDB() );
#endif

		// if resets last_lkey
		sharedtable->logcounter.Set ( (last_lkey>0) ? last_lkey : 0 );

		uint64_t currtime = ZPDS_CURRTIME_MS;

		// check admin exists

		::zpds::store::ExterCredService ucs;
		::zpds::store::ExterDataService uds;
		{
			::zpds::store::ExterDataT admin;
			admin.set_name(ZPDS_DEFAULT_ADMIN);
			admin.set_passkey( default_adminpass );
			if ( sharedtable->is_master.Get() )
				uds.AdminCreateAction(sharedtable,&admin,false,true,false);
			ucs.Get(sharedtable,&admin);
			if (admin.id()>0)
				LOG(INFO) << ZPDS_DEFAULT_ADMIN << " found created at: " << admin.created_at();
			else
				LOG(INFO) << ZPDS_DEFAULT_ADMIN << " not found , need to initialize";
		}

		// check default exists
		{
			::zpds::store::ExterDataT admin;
			admin.set_name(ZPDS_DEFAULT_SEARCH_EXTER);
			admin.set_passkey( default_adminpass );
			if ( sharedtable->is_master.Get() )
				uds.AdminCreateAction(sharedtable,&admin,false,false,true);
			ucs.Get(sharedtable,&admin);
			if (admin.id()>0)
				LOG(INFO) << ZPDS_DEFAULT_SEARCH_EXTER << " found created at: " << admin.created_at();
		}

		currtime = ZPDS_CURRTIME_MS;

#ifdef ZPDS_BUILD_WITH_XAPIAN
		if (FLAGS_warmcache) {
			std::string xapath = sharedtable->xapath.Get();

			// collect the ids from index
			const google::protobuf::EnumDescriptor *l = ::zpds::search::LangTypeE_descriptor();
			const google::protobuf::EnumDescriptor *d = ::zpds::search::IndexTypeE_descriptor();
			for (auto i=0 ; i < l->value_count() ; ++i ) {
				for (auto j=0 ; j < d->value_count() ; ++j ) {
					LOG(INFO) << "Warming cache : " << l->value(i)->name() << "_" << d->value(j)->name() ;
					async::parallel_for(async::irange(0, 9), [xapath,i,j](size_t k) {
						::zpds::search::SearchCache trie( xapath );
						trie.WarmCache( ::zpds::search::LangTypeE(i),::zpds::search::IndexTypeE(j),k,10);
					});
				}
			}
			LOG(INFO) << "Cache Warming took: " << (ZPDS_CURRTIME_MS - currtime)/1000 << " s";
		}

#endif

		LOG(INFO) << ( sharedtable->is_master.Get() ? "Master" : "Slave" ) << " is Ready: ";
	}
	catch (zpds::BaseException& e) {
		DLOG(INFO) << "WorkServer init failed: " << e.what() << std::endl;
		std::rethrow_exception(std::current_exception());
	}
	DLOG(INFO) << "WorkServer LOOP Ends" << std::endl;
}

void zpds::work::WorkServer::stop()
{
	DLOG(INFO) << "Work Server Stop Done" << std::endl;
}

