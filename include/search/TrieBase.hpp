/**
 * @project zapdos
 * @file include/search/TrieBase.hpp
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
 *  TrieBase :  container for Trie
 *
 */
#ifndef _ZPDS_SEARCH_TRIE_BASE_HPP_
#define _ZPDS_SEARCH_TRIE_BASE_HPP_

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
#include "remote/KrovetzStemmer.hpp"
#include "remote/GeoHashHelper.hpp"

#include "../proto/Query.pb.h"

#define XAP_IMPORTANCE_POS 0
#define XAP_ROCKSID_POS    1
#define XAP_LATLON_POS     2

#define XAP_BEGINFULL_PREFIX "bf_"
#define XAP_BEGINPART_PREFIX "bp_"
#define XAP_PARTWORD_PREFIX  "p_"

#define XAP_CCODE_PREFIX     "e_"
#define XAP_CITY_PREFIX      "c_"
#define XAP_PINCODE_PREFIX   "z_"

#define XAP_GEOHASH3_PREFIX  "g3_"
#define XAP_GEOHASH5_PREFIX  "g5_"
#define XAP_GEOHASH7_PREFIX  "g7_"
#define XAP_GEOHASH9_PREFIX  "g9_"

#define XAP_NBRHASH3_PREFIX  "n3_"
#define XAP_NBRHASH5_PREFIX  "n5_"

#define XAP_TAG_PREFIX       "t_"
#define XAP_ISINP_PREFIX     "ii_"

#define XAP_NAMEFULL_PREFIX  "a1_"
#define XAP_NAMEPART_PREFIX  "a2_"

#define XAP_MERGE_LIMIT   1
#define XAP_MIN_STEM_LEN  4
#define XAP_MAX_SHIN_LEN  2

namespace zpds {
namespace search {

class TrieBase {
public:
	using WordPairT = std::pair<std::string,std::string>;
	using WordVecT = std::vector<std::string>;
	using WordMapT = std::unordered_map<std::string, std::string>;

	/**
	* Consructor
	*/
	TrieBase () = default;

	/**
	* StemQuery: stem the quer
	*
	* @param input
	*   std::string& input
	*
	* @param notlast
	*   bool not last
	*
	* @return
	*   std::string output
	*/
	std::string StemQuery(std::string& input, bool notlast);

protected:
	::zpds::remote::GeoHashHelper gh;
	stem::KrovetzStemmer stemmer;


	/**
	* SplitMerge : string to set of words
	*
	* @param input
	*   std::string& input
	*
	* @return
	*   WordVecT to be initialized
	*/
	WordVecT SplitMerge(const std::string& input);


};

} // namespace search
} // namespace zpds
#endif  // _ZPDS_SEARCH_TRIE_BASE_HPP_

