/**
 * @project zapdos
 * @file include/search/SearchTrie.hpp
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
 *  SearchTrie :  search in xapian
 *
 */
#ifndef _ZPDS_SEARCH_SEARCH_TRIE_HPP_
#define _ZPDS_SEARCH_SEARCH_TRIE_HPP_

#include "search/TrieBase.hpp"
#include "store/StoreBase.hpp"
#include "../proto/Search.pb.h"
#include "../proto/Query.pb.h"

#define XAP_FORMAT_SPPL " +"
#define XAP_FORMAT_SUPL " "
#define XAP_CAN_BEGIN_THRESHOLD 100000

#define XAP_TOLERANCE_ORIGINAL 10
#define XAP_TOLERANCE_CORRECTED_LOW 50
#define XAP_TOLERANCE_CORRECTED_HIGH 1000

namespace zpds {
namespace store {
using TextRecordT=LookupRecordT;
using TextRecordListT=LookupRecordListT;
using ExtraAttribT=LookupRecordT;
using ExtraAttribListT=LookupRecordListT;
}
}

namespace zpds {
namespace search {
class SearchTrie : virtual public ::zpds::store::StoreBase, virtual public TrieBase {

public:
	using DatabaseT = Xapian::Database;
	using TrieMapT = std::unordered_map< int, DatabaseT >;

	using WordPairT = TrieBase::WordPairT;
	using WordVecT = TrieBase::WordVecT;
	using WordMapT = TrieBase::WordMapT;

	enum QueryWordsE {
		FULL_WORDS   = 0,
		LAST_PARTIAL = 1,
		ALL_PARTIAL  = 2
	};

	/**
	* Constructor : default
	*
	* @param dbpath
	*   const std::string& dbpath
	*
	*/
	SearchTrie(const std::string& dbpath);

	/**
	* make noncopyable and remove default
	*/
	SearchTrie() = delete;
	SearchTrie(const SearchTrie&) = delete;
	SearchTrie& operator=(const SearchTrie&) = delete;

	/**
	* destructor
	*/
	virtual ~SearchTrie ();


	/**
	* WarmCache: cache warm by query
	*
	* @param lang
	*   ::zpds::search::LangTypeE lang
	*
	* @param lang
	*   ::zpds::search::DataTypeE dtyp
	*
	* @param modno
	*   size_t  modulus no
	*
	* @param outof
	*   size_t  outof these many parts
	*
	* @return
	*   none
	*/
	void WarmCache(::zpds::search::LangTypeE lang, ::zpds::search::DataTypeE dtyp, size_t modno=0, size_t outof=0);

	/**
	* FindNear: check if exists nearby
	*
	* @param qr
	*   ::zpds::search::UsedParamsT* params
	*
	* @param reset
	*   bool reset wordstr
	*
	* @return
	*   bool if at least one match
	*/
	bool FindNear(::zpds::search::UsedParamsT* qr, bool reset);

	/**
	* FindFull: check if exists
	*
	* @param qr
	*   ::zpds::search::UsedParamsT* query
	*
	* @param reset
	*   bool reset wordstr
	*
	* @return
	*   bool if at least one match
	*/
	bool FindFull(::zpds::search::UsedParamsT* qr, bool reset);

protected:
	const std::string dbpath;
	TrieMapT triemap;
	WordVecT wordstr;

	/**
	* Get: get the storage instance
	*
	* @param ltyp
	*   ::zpds::search::LangTypeE ltyp
	*
	* @param dtyp
	*   ::zpds::search::DataTypeE dtyp
	*
	* @return
	*   DatabaseT&
	*/
	DatabaseT& Get(::zpds::search::LangTypeE ltyp, ::zpds::search::DataTypeE dtyp);

	/**
	* SetQuery : reset the wordstr
	*
	* @param query
	*   const std::string& query
	*
	* @return
	*   size_t
	*/
	size_t SetQuery(const std::string& query_);

	/**
	* GetQuerySize : get size of current query
	*
	* @return
	*   size_t
	*/
	size_t GetQuerySize();

	/**
	* GetQueryString: set query
	*
	* @param qtype
	*   QueryWordsE enum type
	*
	* @param extra
	*   const std::string& extra input
	*
	* @param full_prefix
	*   const std::string& full prefix
	*
	* @param part_prefix
	*   const std::string& part prefix
	*
	* @param start
	*   const size_t start default 0
	*
	* @param end
	*   const size_t end default whole
	*
	* @return
	*   std::string
	*/
	std::string GetQueryString(
	    QueryWordsE qtype, const std::string& extra,
	    const std::string& full_prefix, const std::string& part_prefix,
	    const size_t start=0,
	    const size_t end = (wordstr.size() > 0 ) ? wordstr.size()-1, 0
	);

	/**
	* EstimateExec: estimate time based on keyword freq
	*
	* @param db
	*   DatabaseT& db
	*
	* @param input
	*   std::string& input
	*
	* @return
	*   uint64_t estimate
	*/
	uint64_t EstimateExec(DatabaseT& db, std::string& input);

	/**
	* DoWarmCache: cache warm by query
	*
	* @param db
	*   DatabaseT& db
	*
	* @param wvec
	*   WordVecT wvec
	*
	* @return
	*   none
	*/
	void DoWarmCache(DatabaseT& db, WordVecT wvec);

};

} // namespace store
} // namespace zpds
#endif  // _ZPDS_SEARCH_SEARCH_TRIE_HPP_

