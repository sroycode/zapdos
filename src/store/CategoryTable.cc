/**
 * @project zapdos
 * @file src/store/CategoryTable.cc
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
 *  CategoryTable.cc :   Implementation for CategoryTable  category
 *
 */
#include <store/CategoryTable.hpp>
// template class zpds::store::StoreTable<zpds::store::CategoryT>;

/**
* Constructor
*
*/
zpds::store::CategoryTable::CategoryTable( zpds::store::CategoryTable::dbpointer trydb )
	: zpds::store::CategoryTable::CategoryTable(
	trydb,
	K_CATEGORY,
	{ U_CATEGORY_STYP_LANG_UNIQUEID },
	{ I_CATEGORY_STYP_LANG_LASTUP }
	)

{}
/**
* GetKey: get a secondary key
*
*/
std::string zpds::store::CategoryTable::GetKey(zpds::store::CategoryT* record, zpds::store::KeyTypeE keytype, bool pre)
{

	std::string key;
	switch (keytype) {
	default:
	case K_CATEGORY:
		key = EncodePrimaryKey(PrimaryKey,record->id());
		break;
	case U_CATEGORY_STYP_LANG_UNIQUEID : {
		key = EncodeSecondaryKey<int32_t,int32_t,std::string>(keytype , record->styp(),record->lang(),record->uniqueid()  );
		break;
	}
	case I_CATEGORY_STYP_LANG_LASTUP : {
		key = (pre) ?
		      EncodeSecondaryKey<int32_t,int32_t,uint64_t>(keytype , record->styp(),record->lang(),record->updated_at() )
			    : EncodeSecondaryKey<int32_t,int32_t,uint64_t,uint64_t>(keytype , record->styp(),record->lang(),record->updated_at(),record->id() );
		break;
	}
	}
	return key;
}
