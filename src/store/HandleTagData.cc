/**
 * @project zapdos
 * @file src/store/HandleTagData.cc
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
 *  HandleTagData.cc : Class to manipulate TagDataT based Protobuf Msg impl
 *
 */
#include "store/HandleTagData.hpp"
#include "store/TagDataService.hpp"

const ::zpds::store::TagDataService tdserv;

/**
* HandleUserTags : handles tags
*
*/
bool zpds::store::HandleTagData::HandleUserTags(::zpds::utils::SharedTable::pointer stptr,
        ::google::protobuf::RepeatedPtrField< ::zpds::store::TagDataT >* idata,
        ::google::protobuf::RepeatedPtrField< ::zpds::store::TagDataT >* odata,
        std::vector<::zpds::store::KeyTypeE> keytypes,
        bool merge)
{
	std::unordered_map<std::string, ::zpds::store::TagDataT* > kmap;
	bool change=false;
	// if not merge clear old
	if (merge) {
		for (auto i=0; i<odata->size(); ++i) {
			auto var = odata->Mutable(i);
			::zpds::store::TagDataT ref;

			for (auto& ktype : keytypes) {
				ref.set_name(var->name());
				ref.set_keytype(ktype);
				tdserv.Get(stptr,&ref);
				if (!ref.notfound()) break; // found
			}

			// if this tag is not in db throw, but allow all else including not allowed
			if (ref.notfound())
				throw ::zpds::BadDataException("Old data has bad tag, corrupted");
			std::string&& usekey = ref.is_repeated() ?
			                       EncodeSecondaryKey<std::string,std::string>(K_NONODE,var->name(),var->value())
			                       : EncodeSecondaryKey<std::string>(K_NONODE,var->name());
			kmap[usekey] = var;
		}
	}
	else {
		odata->Clear();
	}
	for (auto i=0; i<idata->size(); ++i) {
		auto var = idata->Mutable(i);
		::zpds::store::TagDataT ref;

		// look up all keytypes
		for (auto& ktype : keytypes) {
			ref.set_name(var->name());
			ref.set_keytype(ktype);
			tdserv.Get(stptr,&ref);
			if (!ref.notfound()) break; // found
		}
		DLOG(INFO) << ref.DebugString() << std::endl;
		// if this tag is not in db or not allowed continue
		if (ref.notfound() || (!ref.is_allowed()) || (ref.is_deleted()) ) continue;
		std::string usekey = ref.is_repeated() ?
		                     EncodeSecondaryKey<std::string,std::string>(K_NONODE,var->name(),var->value())
		                     : EncodeSecondaryKey<std::string>(K_NONODE,var->name());
		if (kmap.find(usekey)==kmap.end()) {
			kmap[usekey] = odata->Add();
			kmap[usekey]->set_name( var->name() );
			change=true;
		}
		// overwrite the value of searchable
		kmap[usekey]->set_value( var->value() );
		kmap[usekey]->set_is_searchable( ref.is_searchable() );
	}
	return change;
}

/**
* HandleUserAttr : handles tags
*
*/
bool zpds::store::HandleTagData::HandleUserAttr(::google::protobuf::RepeatedPtrField< ::zpds::store::TagDataT >* idata,
        ::google::protobuf::RepeatedPtrField< ::zpds::store::TagDataT >* odata, bool merge)
{
	std::unordered_map<std::string, ::zpds::store::TagDataT* > kmap;
	bool change=false;
	// if not merge clear old
	if (merge) {
		for (auto i=0; i<odata->size(); ++i) {
			auto var = odata->Mutable(i);
			::zpds::store::TagDataT ref;

			std::string&& usekey = SanitLower(var->name(),true);
			kmap[usekey] = var;
		}
	}
	else {
		odata->Clear();
	}
	for (auto i=0; i<idata->size(); ++i) {
		auto var = idata->Mutable(i);
		::zpds::store::TagDataT ref;

		std::string&& usekey = SanitLower(var->name(),true);
		if (kmap.find(usekey)==kmap.end()) {
			kmap[usekey] = odata->Add();
			kmap[usekey]->set_name( var->name() );
			change=true;
		}
		kmap[usekey]->set_value( var->value() );
	}
	return change;
}

