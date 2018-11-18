/**
 * @project zapdos
 * @file src/tools/AddTextData.cc
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
 *  AddTextData.cc : add textrecord data from csv
 *
 */

#define ZPDS_DEFAULT_EXE_NAME "zpds_addtext"
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

DEFINE_string(infile, "", "input file for textrecord");
DEFINE_string(action, "", "input action");
DEFINE_string(user, "", "user to update for");
DEFINE_string(passkey, "", "user passkey");
DEFINE_bool(print, false, "print data");
DEFINE_bool(update, false, "update data");
// local url
DEFINE_string(jurl, "http://localhost:9091", "local url");
DEFINE_validator(jurl, &IsNonEmptyMessage);
DEFINE_uint64(chunk, 5000, "chunk size");
/* GFlags Settings End */

#include <iostream>
#include <unordered_map>
#include <fstream>
#include <boost/algorithm/string.hpp>
#include "query/ProtoJson.hpp"
#include "LineReader.hpp"
#include "cpr/cpr.h"

std::unordered_map<std::string,int> ActionMap = {
	{ "UPSERT",  1 },
	{ "UPDATE",  2 }
};

void DoAction(::zpds::query::TextDataT* data, int qtyp);

/** main */
int main(int argc, char *argv[])
{

	/** GFlags **/
	std::string usage(
	    "The program loads data for types.  Sample usage:\n"
	    + std::string(argv[0])
	    + " -action UPSERT -infile file.txt -chunk 5000 -user myuser -passkey mypass -print -update -jurl http://localhost:9091\n"
	    + "UPSERT (18): insert or update\n"
	    + "UPDATE (18): same as upsert\n"
	    + "Fields:\n"

			+ "styp uniqueid importance ccode scode city country state "
			+ "title summary details rating tags lang alias_styp alias_uniqueid lat lon"
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

	if ( FLAGS_infile.empty() || FLAGS_action.empty() ) {
		std::cerr << "Error: Need infile and action" << std::endl;
		std::cerr << usage << std::endl;
		return 0;
	}

	if (FLAGS_update && ( FLAGS_user.empty() || FLAGS_passkey.empty() ) ) {
		std::cerr << "Error: Need user and passkey" << std::endl;
		std::cerr << usage << std::endl;
		return 0;
	}

	::zpds::tools::LineReader fdata;

	try {
		auto it = ActionMap.find(FLAGS_action);
		if ( it == ActionMap.end() ) {
			throw zpds::InitialException("Invalid action - " + FLAGS_action);
		}
		int qtyp = it->second;
		zpds::query::TextDataT data;

		std::ifstream file(FLAGS_infile.c_str());
		if(!file.is_open())
			throw zpds::ConfigException("Cannot Open file");
		std::string line;
		size_t counter=0;
		while(std::getline(file,line)) {
			++counter;
			// boost::algorithm::trim(line);
			if(! line.length()) continue;
			std::vector<std::string>  strvec;
			boost::split( strvec, line, boost::is_any_of("\t"));
			for (auto i=0; i<strvec.size(); ++i) {
				boost::algorithm::trim(strvec[i]);
				if (strvec[i]=="NULL" || strvec[i]=="null") strvec[i]="";
			}
			if (strvec.size()<1) continue; // no data
			if (strvec[0].empty()) continue; // no uniqueid
			fdata.ReadToText(strvec,&data,qtyp,counter);
			if (counter % FLAGS_chunk ==0) {
				LOG(INFO) << "Sending Chunk: Counter is : " << counter << std::endl;
				DoAction(&data,qtyp);
				data.Clear();
			}
		}
		file.close();
		if (counter % FLAGS_chunk !=0) {
			LOG(INFO) << "Sending last Chunk: Counter is : " << counter << std::endl;
			DoAction(&data,qtyp);
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


void DoAction(::zpds::query::TextDataT* data, int qtyp)
{
	data->set_name( FLAGS_user);
	data->set_passkey( FLAGS_passkey);
	std::string out;
	::zpds::query::pb2json(data,out,false);

	if (FLAGS_print) {
		std::cout << out << std::endl;
	}

	if (FLAGS_update) {
		std::string url = FLAGS_jurl + "/_data/api/v1/textrecord/" + ( (qtyp>1) ? std::string("merge") : std::string("upsert") );

		auto r = cpr::Post(
		             cpr::Url{url},
		             cpr::Body{out},
		cpr::Header{ {"Content-Type", "application/json"}}
		         );
		std::cout << "Query Result: " << r.text << std::endl;
	}
}

