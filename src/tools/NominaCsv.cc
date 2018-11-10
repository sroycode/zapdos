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
#define ZPDS_DEFAULT_EXE_COPYRIGHT "Copyright (c) 2018"


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
// range for data
DEFINE_string(range, "", "record range comma separated sets");
// DEFINE_validator(range, &IsNonEmptyMessage);
// nominatim postgres url
DEFINE_string(pgparams, "dbname=nominatim user=nominatim host=127.0.0.1 port=6432", "nominatim pg");
DEFINE_validator(pgparams, &IsNonEmptyMessage);
/* GFlags Settings End */

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
	    + " -range 164060 - 164070 , 165204 -pgparams dbname=nominatim user=nominatim host=127.0.0.1 port=6432 \n"
	    + " -range 164060 - 164070 , -onlyname -pgparams dbname=nominatim user=nominatim host=127.0.0.1 port=6432 \n"
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

	if ( FLAGS_range.empty() ) {
		std::cerr << "Error: Need range" << std::endl;
		std::cerr << usage << std::endl;
		return 0;
	}

	try {

		pqxx::connection pgconn(FLAGS_pgparams.c_str());
		pqxx::connection pgconn2(FLAGS_pgparams.c_str());
		std::vector<uint64_t> ids;
		std::vector<std::string> lvec;
		boost::split( lvec, FLAGS_range, boost::is_any_of(","));
		for (auto& line : lvec ) {
			std::vector<std::string> rvec;
			boost::split( rvec, line, boost::is_any_of("-"));
			for (auto& l : rvec ) boost::algorithm::trim(l);
			if (rvec.size()==0) continue;
			auto ifrom = boost::lexical_cast<uint64_t>( rvec.at(0) );
			auto ito = boost::lexical_cast<uint64_t>( (rvec.size()>1) ? rvec.at(1) : rvec.at(0) );
			if (ito<ifrom) continue;

			// from placex get ids
			std::ostringstream xtmp;
			xtmp << "SELECT place_id, name->'name' as name FROM placex WHERE place_id>=" << ifrom << " AND place_id <= " << ito << ";";
			pqxx::work txn(pgconn);
			auto res = txn.exec(xtmp.str());
			for (auto row : res ) {
				if ( row[1].is_null() ) continue;
				if (FLAGS_onlyname) {
					std::cout << row[1].c_str() << std::endl;
				}
				else {
					::zpds::tools::DataFieldT d { pgconn2, row[0].as<uint64_t>() };
					d.Print();
				}
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

