/**
 * @project zapdos
 * @file src/tools/DumpDb.cc
 * @author  S Roychowdhury < sroycode at gmail dot com>
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
 *  DumpDb.cc : DB Dumper
 *
 */

#define ZPDS_DEFAULT_EXE_NAME "zpds_dumpdb"
#define ZPDS_DEFAULT_EXE_VERSION "1.0.0"
#define ZPDS_DEFAULT_EXE_COPYRIGHT "Copyright (c) 2018-2019 S Roychowdhury"

#include <iostream>
#include "store/StoreLevel.hpp"
#include "utils/BaseUtils.hpp"

namespace zpds {
namespace store {
class TablePrint : public StoreLevel {
public:
	using StoreLevel::dbpointer;
	using StoreLevel::StoreLevel;

	TablePrint() = delete;

	void PrintAll()
	{
		usemydb::Iterator* it = getDB()->NewIterator(usemydb::ReadOptions());
		for (it->SeekToFirst(); it->Valid(); it->Next()) {
			auto key = it->key().ToString() ;
			if (key.length() < ( ZPDS_KEYID_LEN + ZPDS_NODE_LEN )) continue;
			KeyTypeE kp = (KeyTypeE) std::stoull(key.substr(0,ZPDS_KEYID_LEN),0,16);
			if ( kp >= K_LOGNODE) continue;
			switch(kp) {

			default:	{
				std::cout << "case K_NOKEY: " << kp << std::endl;
				break;
			}
			case K_LOGNODE:	{
				::zpds::store::TransactionT x;
				bool status = x.ParseFromString(it->value().ToString());
				if (status && x.id()>0) std::cout << "case K_LOGNODE:" << std::endl << x.DebugString() << std::endl;
				break;
			}
			case K_PROFILE:	{
				::zpds::store::ProfileT x;
				bool status = x.ParseFromString(it->value().ToString());
				if (status && x.id()>0) std::cout << "case K_PROFILE:" << std::endl << x.DebugString() << std::endl;
				break;
			}
			case K_SIMPLETEMPLATE:	{
				::zpds::store::SimpleTemplateT x;
				bool status = x.ParseFromString(it->value().ToString());
				if (status && x.id()>0) std::cout << "case K_SIMPLETEMPLATE:" << std::endl << x.DebugString() << std::endl;
				break;
			}
			case K_LOOKUPRECORD:	{
				::zpds::store::LookupRecordT x;
				bool status = x.ParseFromString(it->value().ToString());
				if (status && x.id()>0) std::cout << "case K_LOOKUPRECORD:" << std::endl << x.DebugString() << std::endl;
				break;
			}

			}
		}
	}
};

} // namespace store
} // namespace zpds

/** main */
int main(int argc, char *argv[])
{

	if (argc!=2) {
		std::cerr << "The program dumps db.  Sample usage: " << std::string(argv[0]) + " /path/to/db"  << std::endl;
		exit (1);
	}
	try {
		uint64_t pk=0,lk=0;
		zpds::store::TablePrint s{NULL};
		s.Initialize(std::string(argv[1]), 10, pk,lk);
		s.PrintAll();

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

