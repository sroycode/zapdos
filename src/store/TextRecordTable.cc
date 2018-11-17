/**
 * @project zapdos
 * @file src/store/TextRecordTable.cc
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
 *  TextRecordTable.cc :   Implementation for TextRecordTable  textrecord
 *
 */
#include <store/TextRecordTable.hpp>
// template class zpds::store::StoreTable<zpds::store::TextRecordT>;

/**
* Constructor
*
*/
zpds::store::TextRecordTable::TextRecordTable( zpds::store::TextRecordTable::dbpointer trydb )
	: zpds::store::TextRecordTable::TextRecordTable(
	trydb,
	K_TEXTRECORD,
	{ U_TEXTRECORD_STYP_LANG_UNIQUEID },
	{ I_TEXTRECORD_STYP_LANG_LASTUP }
	)

{}
/**
* GetKey: get a secondary key
*
*/
std::string zpds::store::TextRecordTable::GetKey(zpds::store::TextRecordT* record, zpds::store::KeyTypeE keytype, bool pre)
{

	std::string key;
	switch (keytype) {
	default:
	case K_TEXTRECORD:
		key = EncodePrimaryKey(PrimaryKey,record->id());
		break;
	case U_TEXTRECORD_STYP_LANG_UNIQUEID : {
		key = EncodeSecondaryKey<int32_t,int32_t,std::string>(keytype , record->styp(),record->lang(),record->uniqueid()  );
		break;
	}
	case I_TEXTRECORD_STYP_LANG_LASTUP : {
		key = (pre) ?
		      EncodeSecondaryKey<int32_t,int32_t,uint64_t>(keytype , record->styp(),record->lang(),record->updated_at() )
			    : EncodeSecondaryKey<int32_t,int32_t,uint64_t,uint64_t>(keytype , record->styp(),record->lang(),record->updated_at(),record->id() );
		break;
	}
	}
	return key;
}