/**
 * @project zapdos
 * @file include/store/StoreBase.hpp
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
 *  StoreBase.hpp : Store Base Class Headers
 *
 */
#ifndef _ZPDS_STORE_STOREBASE_HPP_
#define _ZPDS_STORE_STOREBASE_HPP_

#define ZPDS_MEGABYTE 1024 * 1024LL
#define ZPDS_STRN_BLANK ""
#define ZPDS_MIN_TIME 1450000000
#define ZPDS_MAX_TIME 2000000000


#define ZPDS_KEYID_LEN  2

#ifdef ZPDS_USE_BASE80_KEYS
#define ZPDS_NODE_LEN  10
#else
#define ZPDS_NODE_LEN  16
#endif

#define ZPDS_STRING_MAXLEN  1024
#define ZPDS_LEN_NODE_KEY  ZPDS_KEYID_LEN + ZPDS_NODE_LEN

#define ZPDS_MACRO_HEX(A)       std::setbase(16) << std::setfill ('0') << std::setw ( A )
#define ZPDS_MACRO_STR(A)       std::setfill (' ') << std::setw ( A )

#define ZPDS_FMT_ID_KEY(A)      ZPDS_MACRO_HEX(ZPDS_KEYID_LEN) << A

#ifdef ZPDS_USE_BASE80_KEYS
#define ZPDS_FMT_NODE_ONLY(A)   ZPDS_MACRO_STR(ZPDS_NODE_LEN) << Base80String<uint64_t,ZPDS_NODE_LEN>(A)
#else
#define ZPDS_FMT_NODE_ONLY(A)   ZPDS_MACRO_HEX(ZPDS_NODE_LEN) << A
#endif

#define ZPDS_FMT_NODE_KEY(A,B)  ZPDS_FMT_ID_KEY(A) << ZPDS_FMT_NODE_ONLY(B)

#define ZPDS_FMT_SEP "\x1E"
// #define ZPDS_FMT_SEP ":"
/* UINT STRN SINT is defined here so can be changed independent of node if needed */
#define ZPDS_FMT_UINT_ONLY(A)   ZPDS_MACRO_HEX(ZPDS_NODE_LEN) << A
#define ZPDS_FMT_STRN_ONLY(A)    A
#define ZPDS_FMT_SINT_ONLY(A)   ZPDS_MACRO_HEX(ZPDS_NODE_LEN) << A

#define ZPDS_FMT_UINT_KEY(A,B)   ZPDS_FMT_ID_KEY(A) << ZPDS_FMT_UINT_ONLY(B)
#define ZPDS_FMT_STRN_KEY(A,B)   ZPDS_FMT_ID_KEY(A) << ZPDS_FMT_STRN_ONLY(B)
#define ZPDS_FMT_SINT_KEY(A,B)   ZPDS_FMT_ID_KEY(A) << ZPDS_FMT_SINT_ONLY(B)

#define ZPDS_MAX_INTVAL std::numeric_limits<int64_t>::max()

#include <iomanip>
#include <limits>
#include <sstream>
#include "utils/BaseUtils.hpp"
#include "../proto/Store.pb.h"
#include "store/StoreAlias.hh"

#ifdef ZPDS_USE_BASE80_KEYS
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
#endif

namespace zpds {
namespace store {
class StoreBase {

public:

	/**
	* EncodeKeyType : make prefix key with keytype
	*
	* @param
	*   KeyTypeE keytype
	*
	* @return
	*   std::string Key
	*/
	std::string EncodeKeyType (KeyTypeE keytype) const;

	/**
	* EncodePrimaryKey : make primary key with id only
	*
	* @param
	*   KeyTypeE keytype
	*
	* @param
	*   ::google::protobuf::uint64 id
	*
	* @return
	*   std::string Key
	*/
	std::string EncodePrimaryKey (KeyTypeE keytype, ::google::protobuf::uint64 id) const;

	/**
	* CheckPrimaryKey : check if this is primary key
	*
	* @param key
	*   std::string& key
	*
	* @return
	*   bool if yes
	*/
	bool CheckPrimaryKey (std::string& key) const;

	/**
	* DecodePrimaryKey : get keytype and id from primary key
	*
	* @param
	*   std::string& key
	*
	* @return
	*   std::pair<KeyType,uint64_t> keytype, id
	*/
	std::pair<KeyTypeE,uint64_t> DecodePrimaryKey (std::string& key) const;

	/**
	* EncodeSecondaryKey : make secondary key with several variables
	*
	* @param
	*   KeyTypeE keytype
	*
	* @param
	*   T key
	*
	* @return
	*   std::string Key
	*/
	template<typename... T>
	std::string EncodeSecondaryKey (KeyTypeE keytype, T... key) const;

protected:

	/**
	* SanitNSLower : lowercase , nospace word, only puntcts ( _ - . )
	*
	* @param orig
	*   std::string input
	*
	* @return
	*   std::string to be returned
	*/
	std::string SanitNSLower(const std::string& orig) const;

	/**
	* SanitLower : lowercase , punct and single space 
	*
	* @param orig
	*   std::string& input
	*
	* @param notrim
	*   bool to trim flag default false
	*
	* @return
	*   std::string to be returned
	*/
	std::string SanitLower(const std::string& orig, bool notrim=false) const;

	/**
	* SanitKeepCase : keep alnum, punct and single space 
	*
	* @param orig
	*   std::string& input
	*
	* @param notrim
	*   bool to trim flag default false
	*
	* @return
	*   std::string to be returned
	*/
	std::string SanitKeepCase(const std::string& orig, bool notrim=false) const;

};
} // namespace store
} // namespace zpds

// specializations

/**
 * MakeSuffix : make suffix key for single item default
 *
 */
template <typename T>
static void MakeSuffix(std::ostream& o,  T t)
{
	throw ::zpds::BadCodeException("This type cannot be used in key");
}


/**
 * MakeSuffix : make suffix key for single item string&& specialization
 *
 */
template<> void MakeSuffix(std::ostream& o, std::string&& t)
{
	o << ZPDS_FMT_SEP << ZPDS_FMT_STRN_ONLY(t);
}

/**
 * MakeSuffix : make suffix key for single item string specialization
 *
 */
template<> void MakeSuffix(std::ostream& o, std::string t)
{
	o << ZPDS_FMT_SEP << ZPDS_FMT_STRN_ONLY(t);
}

/**
 * MakeSuffix : make suffix key for single item uint64_t specialization
 *
 */
template<> void MakeSuffix(std::ostream& o, uint64_t t)
{
	o << ZPDS_FMT_SEP << ZPDS_FMT_UINT_ONLY(t);
}

/**
 * MakeSuffix : make suffix key for single item uint32_t specialization
 *
 */
template<> void MakeSuffix(std::ostream& o, uint32_t t)
{
	MakeSuffix<uint64_t>(o, (uint64_t) t );
}

/**
 * MakeSuffix : make suffix key for single item int64_t
 *
 */
template<> void MakeSuffix(std::ostream& o, int64_t t)
{
	if (t<0) MakeSuffix<uint64_t>(o, (uint64_t) ZPDS_MAX_INTVAL - (uint64_t) std::abs(t) );
	else MakeSuffix<uint64_t>(o, (uint64_t) ZPDS_MAX_INTVAL + (uint64_t) std::abs(t) );
}

/**
 * MakeSuffix : make suffix key for single item int32_t
 *
 */
template<> void MakeSuffix(std::ostream& o, int32_t t)
{
	if (t<0) MakeSuffix<uint64_t>(o, (uint64_t) ZPDS_MAX_INTVAL - (uint64_t) std::abs(t) );
	else MakeSuffix<uint64_t>(o, (uint64_t) ZPDS_MAX_INTVAL + (uint64_t) std::abs(t) );
}

/**
 * MakeSuffix : variadic recursive for multiple suffix
 *
 */
template<typename T, typename... Args>
static void MakeSuffix(std::ostream& o, T t, Args... args)
{
	MakeSuffix(o, t);
	MakeSuffix(o, args...);
}

/**
 * MakePrefix : each prefix item default
 *
 */
template <typename T>
static void MakePrefix(std::ostream& o, zpds::store::KeyTypeE k,  T t)
{
	throw ::zpds::BadCodeException("This type cannot be used in key");
}

/**
 * MakePrefix : each prefix item type std::string
 *
 */
template<> void MakePrefix(std::ostream& o, zpds::store::KeyTypeE k, std::string t)
{
	o << ZPDS_FMT_STRN_KEY( (unsigned short)k, t);
}

/**
 * MakePrefix : each prefix item type std::string&&
 *
 */
template<> void MakePrefix(std::ostream& o, zpds::store::KeyTypeE k, std::string&& t)
{
	o << ZPDS_FMT_STRN_KEY( (unsigned short)k, t);
}

/**
 * MakePrefix : each prefix item type uint64_t
 *
 */
template<> void MakePrefix(std::ostream& o, zpds::store::KeyTypeE k, uint64_t t)
{
	o << ZPDS_FMT_UINT_KEY( (unsigned short)k, t);
}

/**
 * MakePrefix : each prefix item type uint32_t
 *
 */
template<> void MakePrefix(std::ostream& o, zpds::store::KeyTypeE k, uint32_t t)
{
	MakePrefix<uint64_t>(o, k, (uint64_t) t );
}

/**
 * MakePrefix : each prefix item type int64_t
 *
 */
template<> void MakePrefix(std::ostream& o, zpds::store::KeyTypeE k, int64_t t)
{
	if (t<0) MakePrefix<uint64_t>(o, k, (uint64_t) ZPDS_MAX_INTVAL - (uint64_t) std::abs(t) );
	else MakePrefix<uint64_t>(o, k, (uint64_t) ZPDS_MAX_INTVAL + (uint64_t) std::abs(t) );
}

/**
 * MakePrefix : each prefix item type int32_t
 *
 */
template<> void MakePrefix(std::ostream& o, zpds::store::KeyTypeE k, int32_t t)
{
	if (t<0) MakePrefix<uint64_t>(o, k, (uint64_t) ZPDS_MAX_INTVAL - (uint64_t) std::abs(t) );
	else MakePrefix<uint64_t>(o, k, (uint64_t) ZPDS_MAX_INTVAL + (uint64_t) std::abs(t) );
}

/**
 * MakePrefix : variadic recursive for multiple prefix and suffix
 *
 */
template<typename T, typename... Args>
static void MakePrefix(std::ostream& o, zpds::store::KeyTypeE k, T t, Args... args)
{
	MakePrefix(o, k, t);
	MakeSuffix(o, args...);
}

namespace zpds {
namespace store {

/**
 * EncodeSecondaryKey : encode secondary key
 *
 */
template<typename... T>
std::string StoreBase::EncodeSecondaryKey (zpds::store::KeyTypeE keytype, T... key) const
{
	std::ostringstream ss;
	MakePrefix(ss, keytype, key...);
	return ss.str();
}

} // namespace store
} // namespace zpds

#endif /* _ZPDS_STORE_STOREBASE_HPP_ */
