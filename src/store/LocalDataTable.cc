/**
 * @project zapdos
 * @file src/store/LocalDataTable.cc
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
 *  LocalDataTable.cc : 
 *
 */
#include "store/LocalDataTable.hpp"

/**
* Constructor
*
*/
zpds::store::LocalDataTable::LocalDataTable( zpds::store::LocalDataTable::dbpointer trydb )
	: zpds::store::LocalDataTable::LocalDataTable(
	trydb,
	K_LOCALDATA,
	{ U_LOCALDATA_UNIQUEID },
	{  }
	)

{}
/**
* GetKey: get a secondary key
*
*/
std::string zpds::store::LocalDataTable::GetKey(zpds::store::LocalDataT* record, zpds::store::KeyTypeE keytype, bool pre)
{

	std::string key;
	switch (keytype) {
	default:
	case K_LOCALDATA:
		key = EncodePrimaryKey(PrimaryKey,record->id());
		break;
	case U_LOCALDATA_UNIQUEID : {
		key = EncodeSecondaryKey<std::string>(keytype , record->unique_id()  );
		break;
	}
	}
	return key;
}
