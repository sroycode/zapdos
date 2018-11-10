/**
 * @project zapdos
 * @file src/store/StoreBase.cc
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
 *  StoreBase.cc : Store Base Implementation
 *
 */
#include <cmath>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include "store/StoreBase.hpp"


/**
* Base80String : Get limited char string
*  : someone needs to get a faster way for this to be usable
*  refer http://www.hackersdelight.org/divcMore.pdf
*
*/
template<typename T, size_t N>
inline std::string Base80String(T dividend)
{
	const static char usechars[] = "()+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[]^_abcdefghijklmnopqrstuvwxyz";
	T modulo=0;
	std::string out(N,' '); // N  (char 32 space)
	size_t c=N;
	while (dividend > 0 && c>0) {
		modulo = (dividend - 1) % 80;
		dividend = (T)((dividend - modulo) / 80);
		out[--c] = usechars[modulo];
	}
	return out;
}

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
std::string zpds::store::StoreBase::EncodeKeyType(zpds::store::KeyTypeE keytype)
{
	std::stringstream ss;
	ss << ZPDS_FMT_ID_KEY( (unsigned short)keytype );
	return ss.str();
}

/**
* EncodePrimaryKey : make primary key with id only
*
*/
std::string zpds::store::StoreBase::EncodePrimaryKey(zpds::store::KeyTypeE keytype, ::google::protobuf::uint64 id)
{
	std::stringstream ss;
	ss << ZPDS_FMT_NODE_KEY( (unsigned short)keytype, id );
	return ss.str();
}

/**
* DecodePrimaryKey : get keytype and id from primary key
*
*/
std::pair<zpds::store::KeyTypeE,uint64_t> zpds::store::StoreBase::DecodePrimaryKey (std::string& key)
{

	if (key.length() < ( ZPDS_KEYID_LEN + ZPDS_NODE_LEN ))
		throw zpds::BadDataException("Invalid Key probed");
	zpds::store::KeyTypeE x = (zpds::store::KeyTypeE) std::stoull(key.substr(0,ZPDS_KEYID_LEN),0,16);
	uint64_t y = std::stoull(key.substr(ZPDS_KEYID_LEN,ZPDS_NODE_LEN),0,16);
	return std::make_pair(x,y);
}

