/**
 * @project zapdos
 * @file include/search/BaseUtils.hpp
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
 *  BaseUtils.hpp : Base Utils for Xapian Search
 *
 */
#ifndef _ZPDS_SEARCH_BASE_UTILS_HPP_
#define _ZPDS_SEARCH_BASE_UTILS_HPP_

#define XAP_FORMAT_SPACE " "

#include <tuple>
#include <memory>
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <utility>
#include <sstream>
#include <boost/algorithm/string.hpp>

#include <xapian.h>

#include "utils/BaseUtils.hpp"
#include "../proto/Query.pb.h"

#define XAP_IMPORTANCE_POS 0
#define XAP_ROCKSID_POS    1
#define XAP_UNIQUEID_POS   2
#define XAP_LATLON_POS     3
#define XAP_DATATYPE_POS   4

#define XAP_BEGINFULL_PREFIX "a_"
#define XAP_BEGINPART_PREFIX "b_"
#define XAP_PARTWORD_PREFIX  "c_"

#define XAP_NAMEFULL_PREFIX  "d_"
#define XAP_NAMEPART_PREFIX  "e_"

#define XAP_CCODE_PREFIX     "f_"
#define XAP_CITY_PREFIX      "g_"
#define XAP_PINCODE_PREFIX   "h_"
#define XAP_GEOCODE_PREFIX   "i_"

#define XAP_GEOHASH3_PREFIX  "j_"
#define XAP_GEOHASH5_PREFIX  "k_"
#define XAP_GEOHASH7_PREFIX  "l_"
#define XAP_GEOHASH9_PREFIX  "m_"

#define XAP_NBRHASH3_PREFIX  "n_"
#define XAP_NBRHASH5_PREFIX  "o_"

#define XAP_CATEGORY_PREFIX  "p_"
#define XAP_TAG_PREFIX       "q_"

#define XAP_MERGE_LIMIT   1
#define XAP_MIN_STEM_LEN  4
#define XAP_MAX_SHIN_LEN  2

namespace zpds {
namespace search {

class BaseUtils {
public:
	using WordPairT = std::pair<std::string,std::string>;
	using WordVecT = std::vector<std::string>;
	using WordMapT = std::unordered_map<std::string, std::string>;

	/**
	* StemQuery: stem the query
	*
	* @param input
	*   const std::string& input
	*
	* @param notlast
	*   bool not last
	*
	* @return
	*   std::string output
	*/
	std::string StemQuery(const std::string& input, bool notlast) const;

	/**
	* StemWord: stem the word
	*
	* @param input
	*   const std::string& input
	*
	* @return
	*   std::string output
	*/
	std::string StemWord(const std::string& input) const;

protected:

	/**
	* KeepInBound : keep the number in bound
	*
	*  @param val
	*     int64_t value
	*
	*  @param min
	*     int64_t value min
	*
	*  @param max
	*     int64_t value max
	*
	*  @return
	*   int64_t
	*/
	template <typename T>
	const inline T KeepInBound(T val, T min, T max) const
	{
		return (val > min) ? ( (val<max) ? val : max ) : min;
	}

	/**
	* SplitMerge : string to set of words
	*
	* @param input
	*   std::string& input
	*
	* @return
	*   WordVecT to be initialized
	*/
	WordVecT SplitMerge(const std::string& input) const;

	/**
	* NoSpace : no space
	*
	* @param orig
	*   std::string& input
	*
	* @return
	*   std::string to be initialized
	*/
	std::string NoSpace(const std::string& orig) const;

	/**
	* LcTrim : Lowercase Trim
	*
	* @param orig
	*   std::string& input
	*
	* @return
	*   std::string to be initialized
	*/
	std::string LcTrim(const std::string& orig) const;

	/**
	* Flatten : string to lowrcase nospace word
	*
	* @param input
	*   std::string& input
	*
	* @return
	*   std::string to be initialized
	*/
	std::string Flatten(const std::string& input) const;

	/**
	* FlattenCount : string to lowrcase nospace word
	*
	* @param input
	*   std::string& input
	*
	* @return
	*   std::pair<std::string,size_t> string to be initialized , count
	*/
	std::pair<std::string,size_t> FlattenCount(const std::string& input) const;

	/**
	* FormatPrefix :  to lowercase nospace word
	*
	* @param input
	*   std::string& input
	*
	* @param prefix
	*   std::string& prefix
	*
	* @return
	*   std::string to be initialized
	*/
	std::string FormatPrefix(const std::string& input, const std::string& prefix) const;

	/**
	* FormatTag :  to prefix , eq, lowercase nospace suffix
	*
	* @param name
	*   std::string& input name
	*
	* @param value
	*   std::string& value
	*
	* @return
	*   std::string to be initialized
	*/
	std::string FormatTag(const std::string& name, const std::string& value) const;

	/**
	* CleanStr : clean up tab etc characters
	*
	* @param input
	*   std::string& input
	*
	* @return
	*   std::string to be initialized
	*/
	std::string CleanStr(const std::string& input) const;

};

} // namespace search
} // namespace zpds
#endif  // _ZPDS_SEARCH_BASE_UTILS_HPP_

