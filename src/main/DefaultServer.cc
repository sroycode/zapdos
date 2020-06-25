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
 *  DefaultServer.cc : Default Server Implementation , contains main
 *
 */
#define STRIP_FLAG_HELP 1
#define STRIP_INTERNAL_FLAG_HELP 1
#include <gflags/gflags.h>

#include <glog/logging.h>
#include <iostream>
#include <functional>

/* GFlags Start */
DEFINE_bool(h, false, "Show help");
DECLARE_bool(help);
DECLARE_bool(helpshort);

static bool IsNonEmptyMessage(const char *flagname, const std::string &value)
{
	return ( value[0] != '\0' );
}
DEFINE_string(config, "", "Config file");
DEFINE_validator(config, &IsNonEmptyMessage);
DEFINE_string(master, "", "Start as slave of master");
DEFINE_bool(warmcache, false, "Warms Cache");
DEFINE_bool(dryrun, false, "Dry run to load spellcheck etc");
DEFINE_string(jinpath, "", "overwrite jinpath from cmd line");
DEFINE_string(shared_secret, "", "overwrite shared_secret from cmd line");
DEFINE_string(default_adminpass, "", "overwrite default_adminpass from cmd line");
DEFINE_string(hrpc_thisurl, "", "overwrite hrpc/thisurl from cmd line");
DEFINE_bool(no_xapian, false, "Flag to prevent search index");
/* GFlags End */

#include "utils/BaseUtils.hpp"

#include <exception>
#include <stdexcept>
#include <boost/asio.hpp>
#include <boost/asio/signal_set.hpp>
#include "async++.h"

#include "utils/CfgFileOptions.hpp"
#include "utils/SharedTable.hpp"

#include "DefaultServer.hh"
#include "WorkServer.hpp"
#include "../http/WebServer.hpp"
#include "../hrpc/SyncServer.hpp"

#ifdef ZPDS_BUILD_WITH_CTEMPLATE
#include <ctemplate/template.h>
#endif

#include <boost/filesystem.hpp>

/** main */
int main(int argc, char *argv[])
{

	/** GFlags **/
	std::string usage("Usage:\n");
	usage += std::string(argv[0]) + " -config my.conf # starts as master\n" ;
	usage += std::string(argv[0]) + " -config my.conf --master 127.0.0.1:9093 # starts as slave\n" ;
	usage += std::string(argv[0]) + " -config my.conf --warmcache # warms cache\n" ;
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

	/** definitions */
	zpds::utils::CfgFileOptions::pointer MyCFG;
	auto wks_section = zpds::work::WorkServer::GetSection();
	auto wks_params = zpds::work::WorkServer::GetRequire();

	auto wcs_section = zpds::hrpc::SyncServer::GetSection();
	auto wcs_params = zpds::hrpc::SyncServer::GetRequire();

	auto wbs_section = zpds::http::WebServer::GetSection();
	auto wbs_params = zpds::http::WebServer::GetRequire();

	auto stptr = zpds::utils::SharedTable::create();
	stptr->io_whatever = std::make_shared<::zpds::http::io_whatever>();
	auto m_io_whatever = stptr->io_whatever;

	struct InterruptException {};

	try {
		MyCFG = zpds::utils::CfgFileOptions::create(FLAGS_config);
		/** set master status
		 *  if update only if config is not set or command is not blank
		 */

		if (!(MyCFG->Check(ZPDS_DEFAULT_STRN_SYSTEM,"master") && FLAGS_master.empty()) )
			MyCFG->Update(ZPDS_DEFAULT_STRN_SYSTEM,"master",FLAGS_master);

		// master
		auto master = MyCFG->Find<std::string>(ZPDS_DEFAULT_STRN_SYSTEM, "master");
		stptr->is_master.Set( master.empty() );
		stptr->master.Set( master );

		// if (!master.empty()) throw zpds::InitialException("Only master mode supported yet");

		// shared secret
		auto shared_secret = MyCFG->Find<std::string>(ZPDS_DEFAULT_STRN_SYSTEM, "shared_secret", true);
		if ( ! FLAGS_shared_secret.empty() ) shared_secret = FLAGS_shared_secret;
		if (shared_secret.empty()) throw zpds::InitialException("shared_secret is needed");
		stptr->shared_secret.Set( shared_secret );

		// default_adminpass
		if ( ! FLAGS_default_adminpass.empty() )
			MyCFG->Update(ZPDS_DEFAULT_STRN_WORK, "default_adminpass", FLAGS_default_adminpass);

		// hostname
		auto hostname = MyCFG->Find<std::string>(ZPDS_DEFAULT_STRN_SYSTEM, "hostname");
		if (hostname.empty()) throw zpds::InitialException("hostname is needed");
		stptr->hostname.Set( hostname );

		// lock_updates default false
		int lock_updates = MyCFG->Find<int>(ZPDS_DEFAULT_STRN_SYSTEM, "lock_updates", true); // no throw
		stptr->lock_updates.Set( lock_updates >0 );

		// max_fetch_records default 1000
		uint64_t max_fetch_records = MyCFG->Find<uint64_t>(ZPDS_DEFAULT_STRN_SYSTEM, "max_fetch_records", true); // no throw
		stptr->max_fetch_records.Set( max_fetch_records>0 ? max_fetch_records : 1000 );

		// max_user_sessions default 5
		uint64_t max_user_sessions = MyCFG->Find<uint64_t>(ZPDS_DEFAULT_STRN_SYSTEM, "max_user_sessions", true); // no throw
		stptr->max_user_sessions.Set( (max_user_sessions>0 && max_user_sessions<10) ? max_user_sessions : 5 );

		// uint64_t currtime = ZPDS_CURRTIME_MS;
		/** Local Strings START */

#ifdef ZPDS_BUILD_WITH_XAPIAN
		std::string xapath = MyCFG->Find<std::string>(ZPDS_DEFAULT_STRN_XAPIAN, "datadir");
		if (xapath.empty()) throw zpds::InitialException("xapian/xapath is needed");
		if (!boost::filesystem::exists(xapath)) boost::filesystem::create_directories(xapath);
		stptr->xapath.Set( xapath );
		stptr->xapdb = ::zpds::search::WriteIndex::Create(xapath);

		// spellcheck store jampath
		std::string jampath = MyCFG->Find<std::string>(ZPDS_DEFAULT_STRN_XAPIAN, "jampath");
		if (jampath.empty()) throw zpds::InitialException("xapian/jampath is needed");
		if (!boost::filesystem::exists(jampath)) boost::filesystem::create_directories(jampath);
		DLOG(INFO) << "Jampath Created";

		// spellcheck source jinpath is optional
		std::string jinpath = MyCFG->Find<std::string>(ZPDS_DEFAULT_STRN_XAPIAN, "jinpath",true);
		if ( ! FLAGS_jinpath.empty() ) jinpath = FLAGS_jinpath;
		stptr->jamdb = std::make_shared<::zpds::jamspell::StoreJam>(jampath, jinpath);

		// no_xapian flag
		stptr->no_xapian.Set ( FLAGS_no_xapian );
#endif

		// datadir
		std::string datadir = MyCFG->Find<std::string>(ZPDS_DEFAULT_STRN_WORK, "datadir");
		if (datadir.empty()) throw zpds::InitialException("datadir is needed");
		if (!boost::filesystem::exists(datadir)) boost::filesystem::create_directories(datadir);

#ifdef ZPDS_USE_SEPARATE_LOGDB
		// logdatadir
		std::string logdatadir = MyCFG->Find<std::string>(ZPDS_DEFAULT_STRN_WORK, "logdatadir");
		if (logdatadir.empty()) throw zpds::InitialException("logdatadir is needed");
		if (!boost::filesystem::exists(logdatadir)) boost::filesystem::create_directories(logdatadir);
#endif

		// hrpc_thisurl
		if ( ! FLAGS_hrpc_thisurl.empty() ) {
			MyCFG->Update("hrpc", "thisurl", FLAGS_hrpc_thisurl );
		}

#ifdef ZPDS_BUILD_WITH_CTEMPLATE
		std::string templatedir = MyCFG->Find<std::string>(ZPDS_DEFAULT_STRN_SYSTEM, "templatedir");
		ctemplate::Template::SetTemplateRootDirectory(templatedir);
#endif

		// exit in dryrun - just for spellchecker creation or one init
		if (FLAGS_dryrun) throw InterruptException();

		/** Workserver Params OK */
		for (auto& p : wks_params) p=MyCFG->Find<std::string>(wks_section,p);

		/** Syncserver Params OK */
		for (auto& p : wcs_params) p=MyCFG->Find<std::string>(wcs_section,p);

		/** Webserver Params OK */
		for (auto& p : wbs_params) p=MyCFG->Find<std::string>(wbs_section,p);

		/** Logging, this point onwards stderr is not there */
		if (MyCFG->Check(ZPDS_DEFAULT_STRN_SYSTEM, "accesslog")) {
			google::SetLogDestination(
			    google::GLOG_INFO, MyCFG->Find<std::string>(ZPDS_DEFAULT_STRN_SYSTEM, "accesslog").c_str() );
		}
		if (MyCFG->Check(ZPDS_DEFAULT_STRN_SYSTEM, "errorlog")) {
			google::SetLogDestination(
			    google::GLOG_ERROR, MyCFG->Find<std::string>(ZPDS_DEFAULT_STRN_SYSTEM, "errorlog").c_str() );
			google::SetLogDestination(
			    google::GLOG_FATAL, MyCFG->Find<std::string>(ZPDS_DEFAULT_STRN_SYSTEM, "errorlog").c_str() );
		}

		/** If not exited Start All Servers */

		auto wks = zpds::work::WorkServer::create(stptr->share());
		wks->init(wks_params);

		auto wcs = zpds::hrpc::SyncServer::create(m_io_whatever,stptr->share());
		wcs->init(wcs_params);

		auto wbs = zpds::http::WebServer::create(m_io_whatever,stptr->share());
		wbs->init(wbs_params);

		DLOG(INFO) << "STARTING" << std::endl;
		::zpds::http::io_whatever::work m_work(*m_io_whatever);
		boost::asio::signal_set m_signals(*m_io_whatever,SIGINT,SIGTERM,SIGHUP);
		// m_signals.async_wait(std::bind(&boost::asio::io_whatever::stop, m_io_whatever));
		m_signals.async_wait(
		[&] (const boost::system::error_code& e, int signal_no) {
			// wks->stop();
			LOG(INFO) << "Interrupted: " << e.value() << " signal " << signal_no;
			// stop receiving
			stptr->is_ready.Set( false );
			// wait for received writes to end
			std::this_thread::sleep_for( std::chrono::milliseconds( 500 ) );
			// confirm received writes ended
			auto tmp_csize = stptr->tmpcache->AssocSize();
			while (tmp_csize > 0 ) {
				LOG(INFO) << "Writes going on, wait : " << tmp_csize;
				std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ) );
				tmp_csize = stptr->tmpcache->AssocSize();
			}
			wcs->stop();
			wbs->stop();
			m_io_whatever->stop();
		}
		);
		stptr->is_ready.Set( true );
		m_io_whatever->run();
		/** Interrupted */
		DLOG(INFO) << "IO Stopped" << std::endl;

	}
	catch(InterruptException) {
		LOG(INFO) << "Exited Dryrun";
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

