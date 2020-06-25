/**
 * @project zapdos
 * @file src/main/DefaultServer.cc
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
 *  XapIndex.cc : Create Xapian Index
 *
 */
#define STRIP_FLAG_HELP 1
#define STRIP_INTERNAL_FLAG_HELP 1
#include <gflags/gflags.h>

/* GFlags Start */
DEFINE_bool(h, false, "Show help");
DECLARE_bool(help);
DECLARE_bool(helpshort);

static bool IsNonEmptyMessage(const char *flagname, const std::string &value)
{
	return ( value[0] != '\0' );
}
static bool IsPositive(const char *flagname, const int32_t value)
{
	return value >0;
}
static bool IsValidDType(const char *flagname, const std::string &value)
{
	return ( value=="localdata" || value=="wikidata" );
}

DEFINE_int32(threads, 8, "No of threads to use");
DEFINE_validator(threads, &IsPositive);

DEFINE_int32(cachesize, 100, "DB cache size");
DEFINE_validator(cachesize, &IsPositive);

DEFINE_string(xapath, "", "Xapian Database Directory");
DEFINE_validator(xapath, &IsNonEmptyMessage);

DEFINE_string(dbpath, "", "DB Database Directory");
DEFINE_validator(dbpath, &IsNonEmptyMessage);

DEFINE_string(accesslog, "zpds_xapindex_access.log", "access log");
DEFINE_validator(accesslog, &IsNonEmptyMessage);

DEFINE_string(errorlog, "zpds_xapindex_error.log", "error log");
DEFINE_validator(errorlog, &IsNonEmptyMessage);

DEFINE_string(indextype, "", "Index Type: Should be localdata or wikidata");
DEFINE_validator(indextype, &IsValidDType);

DEFINE_bool(nomerge, false, "Flag for skipping merge indexes created");
DEFINE_bool(noindex, false, "Flag for merge only assuming indexes created");

/* GFlags End */

#include <glog/logging.h>
#include <iostream>
#include <functional>
#include <thread>
#include <chrono>

#include <exception>
#include <stdexcept>
#include <boost/filesystem.hpp>

#include "utils/BaseUtils.hpp"
#include "utils/SharedTable.hpp"
#include "store/StoreLevel.hpp"
#include "search/IndexLocal.hpp"
#include "search/IndexWiki.hpp"
#include "search/WriteIndex.hpp"
#include "utils/SharedQueue.hpp"

#define ZPDS_DEFAULT_EXE_NAME "zpds_xapindex"
#define ZPDS_DEFAULT_EXE_VERSION "1.0.0"
#define ZPDS_DEFAULT_EXE_COPYRIGHT "Copyright (c) 2020 S Roychowdhury"

#define ZPDS_MAX_QUEUE_SIZE 1000000UL
#define ZPDS_SENDER_SLEEP_MS 1000
#define ZPDS_RECEIVER_SLEEP_MS 10
#define ZPDS_COMMIT_TRANSACTIONS 500000

using JobQueT = ::zpds::utils::SharedQueue<std::string>;
using JobQuePtr = std::shared_ptr<JobQueT>;

// Handle for Level/RocksDB
class HandleLevel : public ::zpds::store::StoreLevel {
public:
	using ::zpds::store::StoreLevel::dbpointer;
	using ::zpds::store::StoreLevel::StoreLevel;

	HandleLevel() = delete;
	void Process(::zpds::store::KeyTypeE keytype, JobQuePtr jq);
};

// Handle for Write Index
class HandleWrite : virtual public zpds::search::WriteIndex {
public:
	using DatabaseT = Xapian::WritableDatabase;
	using ::zpds::search::WriteIndex::WriteIndex;
	void Update(::zpds::search::LangTypeE ltyp, ::zpds::search::IndexTypeE dtyp,
	            const std::string& idterm, ::Xapian::Document&& doc) override;
protected:
	DatabaseT& Get(::zpds::search::LangTypeE ltyp, ::zpds::search::IndexTypeE dtyp) override;
};

// Handle for LocaldataT index
// Handle for Search Data
class HandleIndex : virtual public zpds::search::IndexBase {
public:
	using DatabaseT = Xapian::WritableDatabase;
	using pointer = std::shared_ptr<HandleIndex>;
	HandleIndex() = delete;
	HandleIndex(const std::string& path);
	void Process(::zpds::utils::SharedTable::pointer stptr, JobQuePtr jq);
protected:
	HandleWrite index;
	size_t zcount=0;
};

// Handle for LocaldataT index
class HandleLocal : virtual public HandleIndex, virtual public ::zpds::search::IndexLocal {
public:
	using HandleIndex::HandleIndex;
};

// Handle for WikidataT index
class HandleWiki : virtual public HandleIndex, virtual public ::zpds::search::IndexWiki {
public:
	using HandleIndex::HandleIndex;
};

// Handle for Merging indexes
class MergeIndex {
public:
	void Process(std::string& dest, std::vector<std::string>& indexes);
};

/** main */

/** main */
int main(int argc, char *argv[])
{

	setenv("XAPIAN_FLUSH_THRESHOLD","300000",0);
	/** GFlags **/
	std::string usage("Usage:\n");
	usage += std::string(argv[0]) + " -xapath /path/to/xapiandb -\n" ;
	gflags::SetUsageMessage(usage);
	gflags::SetVersionString(ZPDS_DEFAULT_EXE_VERSION);
	// log only to stderr changed to log also
	if (FLAGS_logtostderr) {
		FLAGS_logtostderr = false;
		FLAGS_alsologtostderr = true;
	}

	// read command line
	gflags::ParseCommandLineFlags(&argc, &argv, true);
	// override default help
	if (FLAGS_h) {
		FLAGS_help = false;
		FLAGS_helpshort = true;
	}
	gflags::HandleCommandLineHelpFlags();

	/** Logging **/
	google::InitGoogleLogging(argv[0]);
	google::SetLogDestination( google::GLOG_INFO, FLAGS_accesslog.c_str() );
	google::SetLogDestination( google::GLOG_ERROR, FLAGS_errorlog.c_str() );


	try {

		if (! FLAGS_noindex ) {

			if (FLAGS_xapath.empty()) throw zpds::InitialException("xapath is needed");

			if (boost::filesystem::exists(FLAGS_xapath))
				throw zpds::InitialException("xapath already exists");

			// dbpath
			if (FLAGS_dbpath.empty()) throw zpds::InitialException("dbpath is needed");
			if (!boost::filesystem::exists(FLAGS_dbpath)) throw zpds::InitialException("dbpath needs to have data");

			// indextype
			std::string& indextype = FLAGS_indextype;

			auto stptr = zpds::utils::SharedTable::create();

			// set the database
			{
				uint64_t last_pkey=0;
				uint64_t last_lkey=0;
				HandleLevel s{NULL};
				s.Initialize(FLAGS_dbpath,FLAGS_cachesize,last_pkey,last_lkey);
				stptr->maindb.Set( s.getDB() );
				stptr->is_ready.Set(true); // is_ready is used when data is ready
			}

			// create job queue
			JobQuePtr jqueue { new JobQueT };

			// vector of threads
			std::vector<std::thread> mythreads;

			// create threads
			for (auto i=0 ; i < FLAGS_threads ; ++i ) {
				std::string mypath = FLAGS_xapath + "/" + std::to_string(i);
				boost::filesystem::create_directories(mypath);

				mythreads.emplace_back( std::thread([indextype, mypath, stptr, jqueue]() {
					std::shared_ptr<HandleIndex> iptr{nullptr};
					if ( indextype == "localdata" ) iptr.reset(new HandleLocal(mypath) );
					else if ( indextype == "wikidata" ) iptr.reset(new HandleWiki(mypath) );
					else throw ::zpds::BadCodeException("Unknown index type");
					iptr->Process(stptr,jqueue);
				}) );
			}
			LOG(INFO) << "Threads Created";

			// processing
			{
				HandleLevel s{stptr->maindb.Get()};
				::zpds::store::KeyTypeE ktype;
				if ( indextype == "localdata" ) ktype = ::zpds::store::KeyTypeE::K_LOCALDATA;
				else if ( indextype == "wikidata" ) ktype = ::zpds::store::KeyTypeE::K_WIKIDATA;
				else throw ::zpds::BadCodeException("Unknown index type");
				s.Process( ktype, jqueue );
			}

			// done , wait till data is consumed
			while ( jqueue->GetSize() != 0 ) {
				std::this_thread::sleep_for( std::chrono::milliseconds( ZPDS_SENDER_SLEEP_MS ) );
			}

			// tell threads to stop
			stptr->is_ready.Set(false);

			// after over
			for (auto& i : mythreads ) {
				i.join();
			}
			LOG(INFO) << "Threads Joined ";
		}

		if (! FLAGS_nomerge) {
			// merging databases
			if (FLAGS_xapath.empty()) throw zpds::InitialException("xapath is needed");
			std::string mainpath = FLAGS_xapath + "/main";

			if (boost::filesystem::exists(mainpath))
				throw zpds::InitialException(mainpath + " already exists");

			boost::filesystem::create_directories(mainpath);

			std::string itype;
			if ( FLAGS_indextype == "localdata" ) itype = "I_LOCALDATA";
			else if ( FLAGS_indextype == "wikidata" ) itype = "I_WIKIDATA";
			else throw ::zpds::BadCodeException("Unknown index type");

			const google::protobuf::EnumDescriptor *l = zpds::search::LangTypeE_descriptor();
			for (auto i=0 ; i < l->value_count() ; ++i ) {
				std::string fname { l->value(i)->name() + "_" +  itype };
				std::vector<std::string> paths;
				for (auto i=0 ; i < FLAGS_threads ; ++i ) {
					paths.emplace_back( FLAGS_xapath + "/" + std::to_string(i) + "/" + fname );
				}
				MergeIndex mi;
				std::string mpath = mainpath + "/" + fname;
				mi.Process(mpath, paths);
			}

		}

	}
	catch(zpds::BadCodeException& e) {
		LOG(INFO) << "Error (bad code): " << e.what();
	}
	catch(zpds::BaseException& e) {
		LOG(INFO) << "Error : " << e.what();
	}
	catch(Xapian::Error& e) {
		LOG(INFO) << "Error (xap) : " << e.get_msg() << " " << e.get_description();
	}
	catch(std::exception& e) {
		LOG(INFO) << "Error (sys) : " << e.what();
	}
	catch(...) {
		LOG(INFO) << "Error (unknown) ";
	}

	gflags::ShutDownCommandLineFlags();
	LOG(INFO) << "EXITING" << std::endl;
	return 0;
}

/* Class Function Definitions */

/* HandleWrite */
void HandleWrite::Update(::zpds::search::LangTypeE ltyp, ::zpds::search::IndexTypeE dtyp,
                         const std::string& idterm, ::Xapian::Document&& doc) 
{
	Get(ltyp, dtyp).replace_document(idterm, doc);
}

HandleWrite::DatabaseT& HandleWrite::Get(::zpds::search::LangTypeE ltyp, ::zpds::search::IndexTypeE dtyp)
{
	const google::protobuf::EnumDescriptor *l = zpds::search::LangTypeE_descriptor();
	const google::protobuf::EnumDescriptor *d = zpds::search::IndexTypeE_descriptor();
	int f = (ltyp * 1000) + dtyp;
	if ( triemap.find(f) == triemap.end() ) {
		const std::string xapath{dbpath + "/" + l->FindValueByNumber(ltyp)->name() + "_" + d->FindValueByNumber(dtyp)->name()};
		if (!boost::filesystem::exists(xapath)) boost::filesystem::create_directories(xapath);
		triemap[f] = DatabaseT(xapath, Xapian::DB_CREATE_OR_OPEN | Xapian::DB_DANGEROUS);
	}
	return triemap.at(f);
}

/* HandleIndex */
HandleIndex::HandleIndex(const std::string& path) : index(path) {}

void HandleIndex::Process(::zpds::utils::SharedTable::pointer stptr, JobQuePtr jq)
{
	while ( stptr->is_ready.Get() ) {
		std::string rstr;
		bool status= jq->Pop(rstr);
		if (status) {
			::zpds::store::ItemDataT record;
			if (!record.ParseFromString(rstr) ) continue;
			Xapian::Document doc = CreateDoc( &record );
			std::string idterm = "Q" + std::to_string(record.id());
			index.Update(record.lang(), GetIndexType(), idterm, std::move(doc) );
			if (++zcount % 100000 == 1 ) LOG(INFO) << "Thread " << std::this_thread::get_id() << " : " << zcount ;
		}
		else {
			std::this_thread::sleep_for( std::chrono::milliseconds( ZPDS_RECEIVER_SLEEP_MS ) );
			continue;
		}
	}
}

/* MergeIndex */

void MergeIndex::Process(std::string& dest, std::vector<std::string>& indexes)
{
	Xapian::Database src;
	for (auto& path :  indexes ) {
		if (!boost::filesystem::is_directory(path))
			throw zpds::InitialException(path + " dir does not exist for merging");
		src.add_database(Xapian::Database(path));
	}
	size_t flags = Xapian::DBCOMPACT_MULTIPASS | Xapian::Compactor::STANDARD;
	// size_t block_size = GLASS_DEFAULT_BLOCKSIZE;

	src.compact(dest, flags);
	src.close();
	for (auto& path :  indexes ) {
		// boost::filesystem::remove_all(path);
	}
}

/* HandleLevel */
void HandleLevel::Process(::zpds::store::KeyTypeE keytype, JobQuePtr jq)
{
	// first key of type
	std::string key = EncodePrimaryKey(keytype,0);
	usemydb::Slice start = usemydb::Slice ( key.c_str(), ZPDS_KEYID_LEN );

	usemydb::Iterator* it = getDB()->NewIterator(usemydb::ReadOptions());
	for (it->Seek(start); it->Valid(); it->Next()) {
		auto key = it->key().ToString() ;
		if (key.length() < ( ZPDS_KEYID_LEN + ZPDS_NODE_LEN )) continue;
		::zpds::store::KeyTypeE kp = (::zpds::store::KeyTypeE) std::stoull(key.substr(0,ZPDS_KEYID_LEN),0,16);
		if ( kp != keytype) break;
		while ( jq->GetSize() >= ZPDS_MAX_QUEUE_SIZE ) {
			std::this_thread::sleep_for( std::chrono::milliseconds( ZPDS_SENDER_SLEEP_MS ) );
		}
		jq->Push( it->value().ToString() );
	}
}

