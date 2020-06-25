/**
 * @project zapdos
 * @file src/store/StoreBase.cc
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
 *  StoreBase.cc : Store Base Implementation
 *
 */
#include <cmath>
#include <sstream>
#include <locale>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include "store/StoreBase.hpp"

/**
* Str2Uint64 : Convert the first 8 characters of a string to 64 bit
*
* @param str
*   std::string& string to change
*
* @return
*   uint64_t hash
*/
inline uint64_t Str2Uint64 (std::string str)
{
	boost::algorithm::to_upper(str);
	uint64_t t=0;
	size_t counter=8;
	for (char& i : str) {
		t+= std::pow(i,counter);
		if (--counter==0) break;
	}
	return t;
}

/**
* EncodeKeyType : make prefix key with keytype
*
*/
std::string zpds::store::StoreBase::EncodeKeyType(zpds::store::KeyTypeE keytype) const
{
	std::stringstream ss;
	ss << ZPDS_FMT_ID_KEY( (unsigned short)keytype );
	return ss.str();
}

/**
* EncodePrimaryKey : make primary key with id only
*
*/
std::string zpds::store::StoreBase::EncodePrimaryKey(zpds::store::KeyTypeE keytype, ::google::protobuf::uint64 id) const
{
	std::stringstream ss;
	ss << ZPDS_FMT_NODE_KEY( (unsigned short)keytype, id );
	return ss.str();
}

/**
* CheckPrimaryKey : check if this is primary key
*
*/
bool zpds::store::StoreBase::CheckPrimaryKey (std::string& key) const
{
	if (key.length() != ( ZPDS_KEYID_LEN + ZPDS_NODE_LEN )) return false;
	::zpds::store::KeyTypeE x = (::zpds::store::KeyTypeE) std::stoi(key.substr(0,ZPDS_KEYID_LEN),0,16);
	return (x >= ::zpds::store::K_NONODE && x <= ::zpds::store::K_MAXNODE);

}

/**
* DecodePrimaryKey : get keytype and id from primary key,
*
*/
std::pair<zpds::store::KeyTypeE,uint64_t> zpds::store::StoreBase::DecodePrimaryKey (std::string& key) const
{
	if (!CheckPrimaryKey(key))
		throw zpds::BadDataException("Invalid Key probed");
	::zpds::store::KeyTypeE x = (::zpds::store::KeyTypeE) std::stoi(key.substr(0,ZPDS_KEYID_LEN),0,16);
	uint64_t y = std::stoull(key.substr(ZPDS_KEYID_LEN,ZPDS_NODE_LEN),0,16);
	return std::make_pair(x,y);
}

/**
* SanitNSLower : lowercase , nospace word, only puntcts ( _ - . )
*
*/
std::string zpds::store::StoreBase::SanitNSLower(const std::string& orig) const
{
	if (orig.empty()) return std::string();
	std::ostringstream xtmp;
	std::locale loc;
	for (auto i = orig.cbegin(), n = orig.cend(); i != n; ++i) {
		auto c = (*i);
		// if alnum lowercase into string
		if (std::isalnum(c)) xtmp << std::tolower(c,loc);
		else if (c=='-' || c=='.' || c=='_') xtmp << c;
	}
	return xtmp.str();
}

/**
* SanitLower : lowercase, only single space and puntcts
*
*/
std::string zpds::store::StoreBase::SanitLower(const std::string& orig, bool notrim) const
{
	if (orig.empty()) return std::string();
	std::ostringstream xtmp;
	bool last_is_space = false;
	std::locale loc;
	for (auto i = orig.cbegin(), n = orig.cend(); i != n; ++i) {
		auto c = (*i);
		// if alnum or punct keep
		if (std::isalnum(c)) {
			xtmp << std::tolower(c,loc);
			last_is_space=false;
		}
		else if (std::ispunct(c)) {
			xtmp << c;
			last_is_space=false;
		}
		else if (std::isspace(c,loc)) {
			if (!last_is_space) xtmp << c;
			last_is_space=true;
		}
	}
	if (notrim) return xtmp.str();
	return boost::algorithm::trim_copy(xtmp.str());
}

/**
* SanitKeepCase : only single space and puntcts
*
*/
std::string zpds::store::StoreBase::SanitKeepCase(const std::string& orig, bool notrim) const
{
	if (orig.empty()) return std::string();
	std::ostringstream xtmp;
	bool last_is_space = false;
	std::locale loc;
	for (auto i = orig.cbegin(), n = orig.cend(); i != n; ++i) {
		auto c = (*i);
		// if alnum or punct keep
		if (std::isalnum(c) || std::ispunct(c)) {
			xtmp << c;
			last_is_space=false;
		}
		else if (std::isspace(c,loc)) {
			if (!last_is_space) xtmp << c;
			last_is_space=true;
		}
	}
	if (notrim) return xtmp.str();
	return boost::algorithm::trim_copy(xtmp.str());
}
