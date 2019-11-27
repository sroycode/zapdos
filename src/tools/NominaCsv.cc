/**
 * @project zapdos
 * @file src/tools/NominaCsv.cc
 * @author  S Roychowdhury
 * @version 1.0.0
 *
 * @section LICENSE
 *
 * Copyright (c) 2018-2019 S Roychowdhury
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
 *  NominaCsv.cc : nominatin to csv
 *
 */

#define ZPDS_DEFAULT_EXE_NAME "zpds_nomina"
#define ZPDS_DEFAULT_EXE_VERSION "1.0.0"
#define ZPDS_DEFAULT_EXE_COPYRIGHT "Copyright (c) 2018-2019 S Roychowdhury"


#define STRIP_FLAG_HELP 1
#include <gflags/gflags.h>

/* GFlags Settings Start */
DEFINE_bool(h, false, "Show help");
DECLARE_bool(help);
DECLARE_bool(helpshort);
static bool IsNonEmptyMessage(const char *flagname, const std::string &value)
{
	return ( value[0] != '\0' );
}

// if only name
DEFINE_bool(onlyname, false, "if set prints only name");
// where for data
DEFINE_string(where, "TRUE", "data filters default TRUE");
// DEFINE_validator(range, &IsNonEmptyMessage);
// nominatim postgres url
DEFINE_string(pgparams, "dbname=nominatim user=nominatim host=127.0.0.1 port=6432", "nominatim pg");
DEFINE_validator(pgparams, &IsNonEmptyMessage);
/* GFlags Settings End */

#include <async++.h>
#include "utils/BaseUtils.hpp"
#include <iomanip>
#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <sstream>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <array>
#include "cpr/cpr.h"

#include <pqxx/pqxx>
#include "DataField.hpp"

#define TRY_CATCH_LOOP(X) try { X } catch(...){}


/** main */
int main(int argc, char *argv[])
{

	/** GFlags **/
	std::string usage(
	    "The program converts range of nominatim data to csv for zpds . Sample usage:\n"
	    + std::string(argv[0])
	    + " -pgparams dbname=nominatim user=nominatim host=127.0.0.1 port=6432 \n"
	    + " -onlyname -pgparams dbname=nominatim user=nominatim host=127.0.0.1 port=6432 \n"
	    + "\n"
	);

	gflags::SetUsageMessage(usage);
	gflags::SetVersionString(ZPDS_DEFAULT_EXE_VERSION);

	// read command line
	gflags::ParseCommandLineNonHelpFlags(&argc, &argv, true);
	if (FLAGS_help || FLAGS_h) {
		FLAGS_help = false;
		FLAGS_helpshort = true;
	}
	gflags::HandleCommandLineHelpFlags();

	/** Logging **/
	google::InitGoogleLogging(argv[0]);

	if ( FLAGS_where.empty() ) {
		std::cerr << "Error: Need filters" << std::endl;
		std::cerr << usage << std::endl;
		return 0;
	}

	try {

		pqxx::connection pgconn(FLAGS_pgparams.c_str());
		pqxx::connection pgconn2(FLAGS_pgparams.c_str());
		const auto pgparams2 = FLAGS_pgparams.c_str();
		std::vector<uint64_t> ids;
		pqxx::work txn(pgconn);

		// from placex get no of ids
		uint64_t expected=0;
		std::ostringstream xtmp;
		// xtmp << "SELECT COUNT(place_id) as pcount  FROM placex WHERE " << FLAGS_where << ";";
		xtmp << "SELECT CAST (reltuples AS bigint)  FROM pg_class WHERE relname = 'placex';";
		auto cres = txn.exec(xtmp.str());
		for (auto row : cres ) {
			expected = row[0].as<uint64_t>();
			break;
		}

		LOG(INFO) << "Expecting " << expected << " records" << std::endl;

		// from placex get ids and then only extract those that have name one by one
		xtmp.str();
		xtmp << "SELECT place_id, name->'name' as name FROM placex WHERE " << FLAGS_where << ";";
		auto res = txn.exec(xtmp.str());
		uint64_t count=0;
		for (auto row : res ) {
			if (++count % 10000==1)
				LOG(INFO) << "Doing " << count << "/" << expected << std::endl;
			if ( row[1].is_null() ) continue;
			if (FLAGS_onlyname) {
				std::cout << row[1].c_str() << std::endl;
			}
			else {
				uint64_t rowdata = row[0].as<uint64_t>();
				auto task1 = async::spawn([rowdata] {
					try
					{
						thread_local pqxx::connection pgconn3(FLAGS_pgparams.c_str());
						::zpds::tools::DataFieldT d { pgconn3, rowdata };
						d.Print(true);
					}
					catch(std::exception& e)
					{
						std::cerr << "Error in data : " << rowdata << " : " << e.what() << std::endl;
					}
				});
			}
		}

	}
	catch(zpds::BaseException& e) {
		std::cerr << "Error: " << e.what() << std::endl;
	}
	catch(std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
	}
	catch(...) {
		std::cerr << "Error: UNKNOWN " << std::endl;
	}

	gflags::ShutDownCommandLineFlags();
	return 0;
}

