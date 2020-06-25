/**
 * @project zapdos
 * @file src/tools/AddData.cc
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
 *  AddData.cc : zpds_adddata : Data Addition from csv and json
 *
 */
#define ZPDS_DEFAULT_EXE_NAME "zpds_adddata"
#define ZPDS_DEFAULT_EXE_VERSION "1.0.0"
#define ZPDS_DEFAULT_EXE_COPYRIGHT "Copyright (c) 2018-2020 S Roychowdhury"

#define STRIP_FLAG_HELP 1
#define STRIP_INTERNAL_FLAG_HELP 1
#include <gflags/gflags.h>

/* GFlags Settings Start */
DEFINE_bool(h, false, "Show help");
DECLARE_bool(help);
DECLARE_bool(helpshort);
static bool IsNonEmptyMessage(const char *flagname, const std::string &value)
{
	return (!value.empty());
}

/*
static bool IsValidDtype(const char *flagname, const std::string &value)
{
	return ( value=="localtsv" || value=="wikitsv" || value=="localjson" || value=="wikijson" );
}
*/

DEFINE_string(accesslog, "zpds_adddata_access.log", "access log");
DEFINE_validator(accesslog, &IsNonEmptyMessage);

DEFINE_string(errorlog, "zpds_adddata_error.log", "error log");
DEFINE_validator(errorlog, &IsNonEmptyMessage);

DEFINE_string(dtype, "", "data type");
// DEFINE_validator(dtype, &IsValidDtype);

DEFINE_string(infile, "", "input file for records");
DEFINE_validator(infile, &IsNonEmptyMessage);

DEFINE_string(username, "", "user name to update for");
DEFINE_string(sessionkey, "", "user sessionkey");
DEFINE_string(jurl, "http://localhost:9093", "local url");

DEFINE_uint64(chunk, 10000, "chunk size");
DEFINE_bool(print, false, "print data");
DEFINE_bool(update, false, "update data");

DEFINE_uint64(startline, 1, "starting line");
DEFINE_uint64(stopline, 10000000000UL, "ending line");

/* GFlags Settings End */

#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <fstream>
#include <boost/algorithm/string.hpp>
#include "query/ProtoJson.hpp"

#include <boost/lexical_cast.hpp>
#include "utils/BaseUtils.hpp"
#include "../proto/Store.pb.h"
#include "../proto/Query.pb.h"

#include "http/HttpClient.hpp"
#include "utils/SplitWith.hpp"
#include "nlohmann.hpp"
#include "CountryCodes.hpp"

using StrVec=std::vector<std::string>;
using StrSet=std::unordered_set<std::string>;
using json = nlohmann::json;

/**
 * ReadLocalTsv: read line vec to localdata
 *
 */
bool ReadLocalTsv(std::string& line, ::zpds::store::ItemDataT* record);

/**
 * ReadWikiTsv: read line vec to wikidata
 *
 */
bool ReadWikiTsv(std::string& line, ::zpds::store::ItemDataT* record);

/**
 * ReadLocalJson: read json line vec to locatdata
 *
 */
bool ReadLocalJson(std::string& line, ::zpds::store::ItemDataT* record);

/**
 * ReadLocalJson: read json line vec to wikidata
 *
 */
bool ReadWikiJson(std::string& line, ::zpds::store::ItemDataT* record);

/** main */
int main(int argc, char *argv[])
{

	/** GFlags **/
	std::string usage(
	    "The program loads data for types.  Sample usage:\n"
	    + std::string(argv[0])
	    + " -dtype localtsv -infile file.txt -chunk 5000 -username myuser -sessionkey mypass -jurl http://localhost:9093 -update\n"
	    + "Fields: (23)\n"
	    + " origin, unique_id, importance,"
	    + " ccode, scode, city, country, state, fld_name, fld_area, address, pincode,\n"
	    + " accuracy, lat, lon, rating, landmark, local_name"
	    + " osm_id, osm_key, osm_value, osm_type, geometry"
	    + "\n OR \n"
	    + " -dtype wikitsv -infile file.txt -chunk 5000 -username myuser -sessionkey mypass -jurl http://localhost:9093 -update\n"
	    + "Fields: (17)\n"
	    + " origin, unique_id, importance,"
	    + " ccode, scode, city, country, state, address,\n"
	    + " accuracy, lat, lon, rating, local_name"
	    + " title, summary, details"
	    + "\n OR \n"
	    + " -dtype localjson -infile photon.json -chunk 5000 -username myuser -sessionkey mypass -jurl http://localhost:9093 -update\n"
	    + "\n OR \n"
	    + " -dtype wikijson -infile wikidata.json -chunk 5000 -username myuser -sessionkey mypass -jurl http://localhost:9093 -update\n"
	    + "\n"
	);

	gflags::SetUsageMessage(usage);
	gflags::SetVersionString(ZPDS_DEFAULT_EXE_VERSION);

	// read command line
	gflags::ParseCommandLineFlags(&argc, &argv, true);
	if (FLAGS_help || FLAGS_h) {
		FLAGS_help = false;
		FLAGS_helpshort = true;
	}
	gflags::HandleCommandLineHelpFlags();

	/** Logging **/
	google::InitGoogleLogging(argv[0]);
	google::SetLogDestination( google::GLOG_INFO, FLAGS_accesslog.c_str() );
	google::SetLogDestination( google::GLOG_ERROR, FLAGS_errorlog.c_str() );


	// get url endpoint
	std::string service;
	if ( FLAGS_dtype=="localtsv" || FLAGS_dtype=="localjson" ) {
		service="localdata";
	}
	else if ( FLAGS_dtype=="wikitsv" || FLAGS_dtype=="wikijson" ) {
		service="wikidata";
	}
	else {
		throw zpds::BadCodeException("Undefined service");
	}

	std::string endpoint = "/_admin/api/v1/" + service + "/upsert";

	StrSet countries;

	try {
		if ( FLAGS_update && ( FLAGS_username.empty() || FLAGS_sessionkey.empty() ) )
			throw zpds::ConfigException("Update needs username , sessionkey");

		auto hostport = ::zpds::utils::SplitWith::Regex(FLAGS_jurl,boost::regex(":"),true);
		if (hostport.size()!=3)
			throw zpds::ConfigException("jurl is of form http://host:port");
		hostport.erase(hostport.begin());
		hostport[0]=hostport.at(0).substr(2);

		zpds::query::ItemDataRespT data;
		data.set_username( FLAGS_username);
		data.set_sessionkey( FLAGS_sessionkey);

		using HttpClient = ::zpds::http::HttpClient<::zpds::http::HTTP>;
		using RespPtr=std::shared_ptr< HttpClient::Response >;

		HttpClient client(hostport.at(0), std::stoi( hostport.at(1)));
		RespPtr response;

		::zpds::http::CaseInsensitiveMultimap header {
			{"Content-Type", "application/json"}
		};

		std::ifstream file(FLAGS_infile.c_str());
		if(!file.is_open())
			throw zpds::ConfigException("Cannot Open file");
		std::string line;
		size_t counter=0;
		size_t allcounter=0;
		while(std::getline(file,line)) {
			++allcounter;
			if (allcounter%1000000==1) LOG(INFO) << "Counted Records: " << allcounter;
			if (allcounter < FLAGS_startline) continue;
			if (allcounter > FLAGS_stopline) break;
			// boost::algorithm::trim(line);
			if(! line.length()) continue;

			::zpds::store::ItemDataT record;
			bool status = false;
			try {
				if ( FLAGS_dtype=="localtsv")
					status = ReadLocalTsv(line, &record);
				else if ( FLAGS_dtype=="wikitsv")
					status = ReadWikiTsv(line, &record);
				else if ( FLAGS_dtype=="localjson")
					status = ReadLocalJson(line, &record);
				else if ( FLAGS_dtype=="wikijson")
					status = ReadWikiJson(line, &record);

				if ( ! record.country().empty() ) {
					auto it = ccodes.find( record.country() );
					if (it != ccodes.end() ) {
						record.set_ccode( it->second );
					}
					else {
						if ( countries.find(record.country()) == countries.end() ) {
							LOG(INFO) << "No ccode for " << record.country() ;
							countries.insert(record.country());
						}
					}
				}
			}
			catch (std::exception& e) {}

			if (!status) {
				DLOG(INFO) << line << std::endl;
				DLOG(INFO) << "Ignored Line at : " << allcounter << std::endl;
				continue;
			}
			++counter;
			data.add_payloads()->Swap(&record);

			if (counter % FLAGS_chunk ==0) {
				if (FLAGS_print) {
					std::cout << data.DebugString() << std::endl;
				}
				if (FLAGS_update) {
					LOG(INFO) << "Sending Chunk: Counter is : " << counter << std::endl;
					std::string out;
					::zpds::query::pb2json(&data,out,false);
					response = client.request("POST", endpoint, out, header);
					if (std::stoi(response->status_code) != 200)
						throw ::zpds::BadDataException("Could not POST");
					LOG(INFO) << "Response: " << response->content.string() << std::endl;
				}
				data.Clear();
				data.set_username( FLAGS_username);
				data.set_sessionkey( FLAGS_sessionkey);
			}
		}
		file.close();
		if (counter % FLAGS_chunk !=0) {
			if (FLAGS_print) {
				std::cout << data.DebugString() << std::endl;
			}
			if (FLAGS_update) {
				LOG(INFO) << "Sending last Chunk: Counter is : " << counter << std::endl;
				std::string out;
				::zpds::query::pb2json(&data,out,false);
				response = client.request("POST", endpoint, out, header);
				if (std::stoi(response->status_code) != 200)
					throw ::zpds::BadDataException("Could not POST");
				LOG(INFO) << "Response: " << response->content.string() << std::endl;
			}
			data.Clear();
			data.set_username( FLAGS_username);
			data.set_sessionkey( FLAGS_sessionkey);
		}

	}
	catch(zpds::BaseException& e) {
		LOG(ERROR) << "Error: " << e.what() << std::endl;
	}
	catch(std::exception& e) {
		LOG(ERROR) << "Error: " << e.what() << std::endl;
	}
	catch(...) {
		LOG(ERROR) << "Error: UNKNOWN " << std::endl;
	}

	gflags::ShutDownCommandLineFlags();
	return 0;
}

/**
 * ReadTsvLine: read a line to vector
 *
 */
StrVec ReadTsvLine(std::string& line)
{
	StrVec  strvec;
	boost::split( strvec, line, boost::is_any_of("\t"));
	for (auto i=0; i<strvec.size(); ++i) {
		boost::algorithm::trim(strvec[i]);
		if (strvec[i]=="NULL" || strvec[i]=="null") strvec[i]="";
	}
	return strvec;

}


#define LOCAL_HANDLE_STRING(NNN,AAA) if (!strvec[NNN].empty()) record->set_##AAA( strvec[NNN] );
#define LOCAL_HANDLE_NUMBER(NNN,TTT,AAA) if (!strvec[NNN].empty()) record->set_##AAA( boost::lexical_cast<TTT> ( strvec[NNN] ) );

/**
 * ReadLocalTsv: read line vec to ER
 *
 */
bool ReadLocalTsv(std::string& line, ::zpds::store::ItemDataT* record)
{

	auto strvec = ReadTsvLine(line);

	if (strvec.size()!=23) return false;

	LOCAL_HANDLE_STRING(0,origin)
	LOCAL_HANDLE_STRING(1,unique_id)

	if (record->unique_id().length()==0) return false;

	LOCAL_HANDLE_NUMBER(2,double,importance)

	LOCAL_HANDLE_STRING(3,ccode)
	LOCAL_HANDLE_STRING(4,scode)
	LOCAL_HANDLE_STRING(5,city)
	LOCAL_HANDLE_STRING(6,country)
	LOCAL_HANDLE_STRING(7,state)
	LOCAL_HANDLE_STRING(8,fld_name)
	LOCAL_HANDLE_STRING(9,fld_area)
	LOCAL_HANDLE_STRING(10,address)
	LOCAL_HANDLE_STRING(11,pincode)

	LOCAL_HANDLE_NUMBER(12,uint64_t,accuracy)
	LOCAL_HANDLE_NUMBER(13,double,lat)
	LOCAL_HANDLE_NUMBER(14,double,lon)
	LOCAL_HANDLE_NUMBER(15,double,rating)
	LOCAL_HANDLE_STRING(16,landmark)
	LOCAL_HANDLE_STRING(17,landmark)

	LOCAL_HANDLE_NUMBER(18,int64_t,osm_id)
	LOCAL_HANDLE_STRING(19,osm_key)
	LOCAL_HANDLE_STRING(20,osm_value)
	LOCAL_HANDLE_STRING(21,osm_type)
	LOCAL_HANDLE_STRING(22,geometry)

	return true;
}


/**
 * ReadWikiTsv: read line vec to TR
 *
 */
bool ReadWikiTsv(std::string& line, ::zpds::store::ItemDataT* record)
{

	auto strvec = ReadTsvLine(line);
	if (strvec.size()!=17) return false;

	LOCAL_HANDLE_STRING(0,origin)
	LOCAL_HANDLE_STRING(1,unique_id)

	if (record->unique_id().length()==0) return false;

	LOCAL_HANDLE_NUMBER(2,double,importance)

	LOCAL_HANDLE_STRING(3,ccode)
	LOCAL_HANDLE_STRING(4,scode)
	LOCAL_HANDLE_STRING(5,city)
	LOCAL_HANDLE_STRING(6,country)
	LOCAL_HANDLE_STRING(7,state)
	LOCAL_HANDLE_STRING(8,address)

	LOCAL_HANDLE_NUMBER(9,uint64_t,accuracy)
	LOCAL_HANDLE_NUMBER(10,double,lat)
	LOCAL_HANDLE_NUMBER(11,double,lon)
	LOCAL_HANDLE_NUMBER(12,double,rating)
	LOCAL_HANDLE_STRING(13,local_name)

	LOCAL_HANDLE_STRING(14,title)
	LOCAL_HANDLE_STRING(15,summary)
	LOCAL_HANDLE_STRING(16,details)

	return true;
}

#undef LOCAL_HANDLE_STRING
#undef LOCAL_HANDLE_NUMBER

#define JSON_HANDLE_SOURCE_OBJ(BBB,AAA) \
	if (data["/_source/" #AAA "/en"_json_pointer].is_string()) \
		record->set_##BBB( data["/_source/" #AAA "/en"_json_pointer].get<std::string>() ); \
	else if (data["/_source/" #AAA "/default"_json_pointer].is_string()) \
		record->set_##BBB( data["/_source/" #AAA "/default"_json_pointer].get<std::string>() );

#define JSON_HANDLE_STRING(BBB,AAA) \
	if (data[ #AAA ].is_string()) \
		record->set_##BBB( data[ #AAA ].get<std::string>() );

#define JSON_HANDLE_NUMBER(BBB,AAA,CCC) \
	if (data[ #AAA ].is_number()) \
		record->set_##BBB( data[ #AAA ].get<CCC>() );

#define JSON_HANDLE_SOURCE_COORDINATE(BBB,AAA) \
	if (data["/_source/coordinate/" #AAA ""_json_pointer].is_number()) \
		record->set_##BBB( data["/_source/coordinate/" #AAA ""_json_pointer].get<double>() );

#define JSON_HANDLE_SOURCE_STRING(BBB,AAA) \
	if (data["/_source/" #AAA ""_json_pointer].is_string()) \
		record->set_##BBB( data["/_source/" #AAA ""_json_pointer].get<std::string>() );

#define JSON_HANDLE_SOURCE_NUMBER(BBB,AAA,CCC) \
	if (data["/_source/" #AAA ""_json_pointer].is_number()) \
		record->set_##BBB( data["/_source/" #AAA ""_json_pointer].get<CCC>() );

/**
 * ReadLocalJson: read json line vec to ER
 *
 */
bool ReadLocalJson(std::string& line, ::zpds::store::ItemDataT* record)
{

	auto data = json::parse(line);

	JSON_HANDLE_STRING(unique_id,_id)
	JSON_HANDLE_NUMBER(importance,importance,uint64_t)
	JSON_HANDLE_SOURCE_OBJ(city,city)
	JSON_HANDLE_SOURCE_OBJ(country,country)
	JSON_HANDLE_SOURCE_OBJ(state,state)
	JSON_HANDLE_SOURCE_OBJ(fld_name,name)
	JSON_HANDLE_SOURCE_OBJ(fld_area,street)
	JSON_HANDLE_SOURCE_STRING(pincode,pincode)

	JSON_HANDLE_SOURCE_COORDINATE(lat,lat)
	JSON_HANDLE_SOURCE_COORDINATE(lon,lon)

	JSON_HANDLE_SOURCE_NUMBER(osm_id,osm_id,int64_t)
	JSON_HANDLE_SOURCE_STRING(osm_key,osm_key)
	JSON_HANDLE_SOURCE_STRING(osm_value,osm_value)
	JSON_HANDLE_SOURCE_STRING(osm_type,osm_type)

	// Not handled
	// origin : string
	// ccode : string
	// scode : string
	// address : string
	// landmark : string
	// rating : double
	// geometry : string

	return true;
}

/**
 * ReadLocalJson: read json line vec to wikidata
 *
 */
bool ReadWikiJson(std::string& line, ::zpds::store::ItemDataT* record)
{

	auto data = json::parse(line);

	JSON_HANDLE_STRING(summary,description)
	JSON_HANDLE_STRING(unique_id,unique_id)

	if ( record->summary().empty() || record->unique_id().empty() )
		return false;

	JSON_HANDLE_STRING(title,title)
	if ( record->title().empty() )
		JSON_HANDLE_STRING(title,label)

		return (! record->title().empty() );
}


#undef JSON_HANDLE_SOURCE_OBJ
#undef JSON_HANDLE_SOURCE_COORDINATE
#undef JSON_HANDLE_SOURCE_STRING
#undef JSON_HANDLE_SOURCE_NUMBER
#undef JSON_HANDLE_STRING
#undef JSON_HANDLE_NUMBER
