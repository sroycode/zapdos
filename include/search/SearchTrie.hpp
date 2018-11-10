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
namespace search {
class SearchTrie : virtual public ::zpds::store::StoreBase, virtual public TrieBase {

public:
	using DatabaseT = Xapian::Database;
	using TrieMapT = std::unordered_map< int , DatabaseT >;

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
	* RuleSearch : do the search from rule
	*
	* @param qr
	*   ::zpds::search::UsedParamsT* query params
	*
	* @param rule
	*   ::zpds::search::QueryOrderT* query order rule
	*
	* @param records
	*   ::zpds::store::LookupRecordListT* records to fetch
	*
	* @param target
	*   size_t no of records to fetch
	*
	* @return
	*   bool if found
	*/
	bool RuleSearch(
	    ::zpds::search::UsedParamsT* qr,
	    ::zpds::search::QueryOrderT* rule,
	    ::zpds::store::LookupRecordListT* records,
	    size_t target);

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

	/**
	* WarmCache: cache warm by query
	*
	* @param lang
	*   ::zpds::search::LangTypeE lang
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
	void WarmCache(::zpds::search::LangTypeE lang, size_t modno=0, size_t outof=0);

protected:
	const std::string dbpath;
	TrieMapT triemap;
	WordVecT wordstr;

	/**
	* Get: get the storage instance
	*
	* @param dtyp
	*   ::zpds::search::LangTypeE dtyp
	*
	* @return
	*   DatabaseT&
	*/
	DatabaseT& Get(::zpds::search::LangTypeE dtyp);

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
	* @param prefix
	*   std::string lovehate char or logic operator default +
	*
	* @return
	*   std::string
	*/
	std::string GetQueryString(QueryWordsE qtype, const std::string& extra, std::string prefix="+" );

	/**
	* GetQueryStringPart: set query
	*
	* @param qtype
	*   QueryWordsE enum type
	*
	* @param start
	*   size_t start
	*
	* @param end
	*   size_t end
	*
	* @param extra
	*   const std::string& extra input
	*
	* @param prefix
	*   std::string lovehate char or logic operator default +
	*
	* @return
	*   std::string
	*/
	std::string GetQueryStringPart(QueryWordsE qtype, size_t start, size_t end, const std::string& extra, std::string prefix="+" );

	/**
	* FindNear: check if exists nearby
	*
	* @param db
	*   DatabaseT& db
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
	bool FindNear(DatabaseT& db, ::zpds::search::UsedParamsT* qr, bool reset);

	/**
	* FindFull: check if exists
	*
	* @param db
	*   DatabaseT& db
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
	bool FindFull(DatabaseT& db, ::zpds::search::UsedParamsT* qr, bool reset);

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
	* WarmCache: cache warm by query
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
	void WarmCache(DatabaseT& db, WordVecT wvec);

};

} // namespace store
} // namespace zpds
#endif  // _ZPDS_SEARCH_SEARCH_TRIE_HPP_

