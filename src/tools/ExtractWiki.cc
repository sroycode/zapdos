/**
 * @project zapdos
 * @file src/tools/ExtractWiki.cc
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
 *  ExtractWiki.cc : zpds_extractwiki : extract wiki data to format of zpds_adddata
 *
 */
#define ZPDS_DEFAULT_EXE_NAME "zpds_extractwiki"
#define ZPDS_DEFAULT_EXE_VERSION "1.0.0"
#define ZPDS_DEFAULT_EXE_COPYRIGHT "Copyright (c) 2018-2020 S Roychowdhury"


#include "async++.h"
#include "safe_ostream.hpp"

#include <memory>
#include <mutex>
#include <iostream>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

#include <boost/lexical_cast.hpp>
#include "utils/BaseUtils.hpp"

#include "utils/SplitWith.hpp"
#include "nlohmann.hpp"
#include "CountryCodes.hpp"

using StrVec=std::vector<std::string>;
using json = nlohmann::json;
using MutexPtr=std::shared_ptr<std::mutex>;


MutexPtr m_ptr{new std::mutex};
/**
 * LineReader : line reader
 *
 */
void LineReader (std::string& line, MutexPtr m);


/** main */
int main(int argc, char *argv[])
{

	if (argc!=2) {
		std::cerr << "The extracts wikidata json.  Sample usage: " << argv[0] << " /path/to/all.json" << std::endl;
		exit (1);
	}
	try {
		std::stringstream sstr;
		std::ifstream file(argv[1]);
		if(!file.is_open())
			throw zpds::ConfigException("Cannot Open file");
		std::string line;
		bool multi=false;
		while(std::getline(file,line)) {
			if(line.length() < 10) continue;
			bool line_comma = ( line.at( line.length() -1 ) == ',' );
			if (!line_comma) {
				sstr << line;
				multi=true;
			}
			else {
				line.pop_back();
				if ( multi ) {
					sstr << line;
					line = sstr.str();
					// clear the stream
					sstr.str("");
					sstr.clear();
				}
				async::spawn( std::bind(&LineReader, line , m_ptr) );
			}

		}

	}
	catch(zpds::BaseException& e) {
		std::cerr << e.what() << std::endl;
	}
	catch(std::exception& e) {
		std::cerr << "Error " << e.what() << std::endl;
	}
	catch(...) {
		std::cerr << "Error UNKNOWN" << std::endl;
	}

	return 0;
}

#define JSONH_OBJ_EN_ARRAY(AAA,BBB) \
	auto v = data["/" #AAA "/en"_json_pointer]; if (v.is_array()) {for(auto& i:v) { if( i["language"]=="en") ndata[ #BBB ]=i["value"]; }}
#define JSONH_OBJ_OBJP(AAA,BBB) \
	auto v = data["/" #AAA ""_json_pointer]; if (v.is_object()) { for(auto it=v.begin();it!=v.end();++it) ndata[ #BBB ].push_back(it.key()); }
#define JSONH_STR(AAA,BBB) \
	auto v = data["/" #AAA ""_json_pointer]; if (v.is_string()) { ndata[ #BBB ]= v; } 

void LineReader(std::string& line, MutexPtr m)
{

	try {
		auto data = json::parse(line);
		json ndata;
		// int n_aliases=0;

		{ JSONH_OBJ_EN_ARRAY(aliases,alias) }
		{ JSONH_OBJ_OBJP(claims,claims) }

		{ JSONH_STR(descriptions/en/value, description) }
		{ JSONH_STR(labels/en/value,label) }
		{ JSONH_STR(id, unique_id) }
		{ JSONH_STR(sitelinks/enwiki/title , title) }
		{ JSONH_STR(sitelinks/commonswiki/title , ctitle) }

		std::lock_guard<std::mutex>(*m);
		::felicity::safe_cout << ndata.dump() << "\n";
	}
	catch (json::exception& e) {
		std::cerr << "EXCEPTION: " << e.what();
	}
	catch (std::exception& e) {
	}

}
#undef  JSONH_OBJ_EN_ARRAY
#undef  JSONH_OBJ_OBJP
#undef  JSONH_STR
