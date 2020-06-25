/**
 * @project zapdos
 * @file src/store/HandleStringData.cc
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
 *  HandleStringData.cc : 
 *
 */
#include "store/HandleStringData.hpp"
#include "store/CategoryService.hpp"
#include <unordered_map>

const ::zpds::store::CategoryService csdata;

/**
* StringData : handle string data arrays
*
*/
bool zpds::store::HandleStringData::HandleStrings(
    ::google::protobuf::RepeatedPtrField< std::string >* idata,
    ::google::protobuf::RepeatedPtrField< std::string >* odata,
    bool merge)

{
	bool change=false;
	std::unordered_map<std::string, std::string* > kmap;
	// if not merge clear old
	if (merge) {
		for (auto i=0; i<odata->size(); ++i) {
			auto var = odata->Mutable(i);
			kmap[*var] = var;
		}
	}
	else {
		odata->Clear();
	}
	for (auto i=0; i<idata->size(); ++i) {
		auto var = idata->Mutable(i);
		if ( var->empty()  || kmap.find(*var)!=kmap.end() ) continue;
		kmap[*var] = odata->Add();
		kmap[*var]->assign(*var);
		// Check : TODO
		change=true;
	}
	return change;
}

/**
* HandleUserCategories : handles categories
*
*/
bool zpds::store::HandleStringData::HandleUserCategories(
    ::zpds::utils::SharedTable::pointer stptr,
    ::google::protobuf::RepeatedPtrField< std::string >* idata,
    ::google::protobuf::RepeatedPtrField< std::string >* odata,
    bool merge)
{
	bool change=false;
	std::unordered_map<std::string, std::string* > kmap;
	// if not merge clear old
	if (merge) {
		for (auto i=0; i<odata->size(); ++i) {
			auto var = odata->Mutable(i);
			kmap[*var] = var;
		}
	}
	else {
		odata->Clear();
	}

	for (auto i=0; i<idata->size(); ++i) {
		auto var = idata->Mutable(i);
		if ( var->empty()  || kmap.find(*var)!=kmap.end() ) continue;
		::zpds::store::CategoryT ref;
		ref.set_name(*var);
		csdata.Get(stptr,&ref);
		if (ref.notfound() || ref.is_deleted())
			throw ::zpds::BadDataException("Category absent or deleted: " + *var);
		kmap[*var] = odata->Add();
		kmap[*var]->assign(*var);
		change=true;
	}
	return change;
}
