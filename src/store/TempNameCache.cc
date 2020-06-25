/**
 * @project zapdos
 * @file src/store/TempNameCache.cc
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
 *  TempNameCache.cc : Cache of temporary unique fields impl
 *
 */
#include "store/TempNameCache.hpp"
#include "store/ServiceDefine.hh"

/**
* Constructor : default str
*
*/
zpds::store::TempNameCache::TempNameCache(::zpds::utils::SharedTable::pointer stptr) :
	zpds::store::TempNameCache::TempNameCache(stptr->maindb.Get(), ::zpds::store::K_NONODE, {}, {})
{
	tmpcache = stptr->tmpcache;
}

/**
* destructor
*
*/
zpds::store::TempNameCache::~TempNameCache()
{
	for (auto& s : inset) tmpcache->DelAssoc(s);
}

/**
* GetKey: get a key
*
*/
std::string zpds::store::TempNameCache::GetKey(::zpds::store::NodeT* record, ::zpds::store::KeyTypeE keytype, bool pre)
{
	throw ::zpds::BadCodeException("GetKey is not used in cache");
}

/**
* MakeKey : make key from keytype, name
*
*/
std::string zpds::store::TempNameCache::MakeKey(::zpds::store::KeyTypeE keytype, const std::string& indata)
{
	std::string xdata;
	switch(keytype) {
	default: {
		throw ::zpds::BadCodeException("Invalid keytype to check");
		break;
	}
	case ::zpds::store::K_CATEGORY: {
		xdata = EncodeSecondaryKey<std::string>(U_CATEGORY_NAME,indata);
		break;
	}
	case ::zpds::store::K_EXTERDATA: {
		xdata = EncodeSecondaryKey<std::string>(U_EXTERDATA_NAME,indata);
		break;
	}
	case ::zpds::store::K_USERDATA: {
		xdata = EncodeSecondaryKey<std::string>(U_USERDATA_NAME,indata);
		break;
	}
	case ::zpds::store::K_LOCALDATA: {
		xdata = EncodeSecondaryKey<std::string>(U_LOCALDATA_UNIQUEID,indata);
		break;
	}
	case ::zpds::store::K_WIKIDATA: {
		xdata = EncodeSecondaryKey<std::string>(U_WIKIDATA_UNIQUEID,indata);
		break;
	}

	}
	return xdata;
}

/**
* ReserveTag: reserve this string tag
*
*/
bool zpds::store::TempNameCache::ReserveTag(::zpds::store::KeyTypeE keytype, const std::string& indata, bool isnew)
{

	std::string&& xdata=EncodeSecondaryKey<int32_t,std::string>(U_TAGDATA_KEYTYPE_NAME,keytype,indata);
	if (isnew && CheckKeyExists(xdata)) return false;
	if (tmpcache->CheckAssoc(xdata)) return false;
	std::string temp;
	tmpcache->SetAssoc( xdata, temp);
	inset.insert(xdata);
	return true;
}

/**
* ReserveName: reserve this string
*
*/
bool zpds::store::TempNameCache::ReserveName(::zpds::store::KeyTypeE keytype, const std::string& indata, bool isnew)
{
	std::string&& xdata=MakeKey(keytype,indata);
	if (isnew && CheckKeyExists(xdata)) return false;
	if (tmpcache->CheckAssoc(xdata)) return false;
	std::string temp;
	tmpcache->SetAssoc( xdata, temp);
	inset.insert(xdata);
	return true;
}

/**
* CheckLocal: check local key exists
*
*/
bool zpds::store::TempNameCache::CheckLocal(::zpds::store::KeyTypeE keytype, const std::string& indata)
{
	std::string&& xdata=MakeKey(keytype,indata);
	return ( inset.find(xdata) != inset.end() );
}

/**
* MultiTypeLock: Lock for multiple related types concurrent updates
*
*/
bool zpds::store::TempNameCache::MultiTypeLock(::google::protobuf::uint64 locktype)
{
	std::string xdata  = EncodePrimaryKey(K_NONODE,locktype);
	if (tmpcache->CheckAssoc(xdata)) return false;
	std::string temp;
	tmpcache->SetAssoc( xdata, temp);
	inset.insert(xdata);
	return true;
}
