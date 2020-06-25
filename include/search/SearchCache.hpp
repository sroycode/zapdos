/**
 * @project zapdos
 * @file include/search/SearchCache.hpp
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
 *  SearchCache.hpp : Xapian Search Cache for WorkServer Headers
 *
 */
#ifndef _ZPDS_SEARCH_SEARCH_CACHE_HPP_
#define _ZPDS_SEARCH_SEARCH_CACHE_HPP_

#include "search/SearchBase.hpp"

namespace zpds {
namespace search {
class SearchCache : virtual public SearchBase {

public:
	using DatabaseT = ReadIndex::DatabaseT;

	using WordPairT = BaseUtils::WordPairT;
	using WordVecT = BaseUtils::WordVecT;
	using WordMapT = BaseUtils::WordMapT;

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
	SearchCache(const std::string& dbpath);

	/**
	* make noncopyable and remove default
	*/
	SearchCache() = delete;
	SearchCache(const SearchCache&) = delete;
	SearchCache& operator=(const SearchCache&) = delete;

	/**
	* destructor
	*/
	virtual ~SearchCache ();

	/**
	* WarmCache: cache warm by query
	*
	* @param lang
	*   ::zpds::search::LangTypeE lang
	*
	* @param lang
	*   ::zpds::search::IndexTypeE dtyp
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
	void WarmCache(::zpds::search::LangTypeE lang, ::zpds::search::IndexTypeE dtyp, size_t modno=0, size_t outof=0);

	/**
	* CompletionQueryAction : simple query service action
	*
	* @param stptr
	*   ::zpds::utils::SharedTable::pointer stptr
	*
	* @param resp
	*   ::zpds::query::SearchCompletionRespT* resp
	*
	* @return
	*   none
	*/
	void CompletionQueryAction(::zpds::utils::SharedTable::pointer stptr, ::zpds::query::SearchCompletionRespT* resp) override;

protected:

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

} // namespace search
} // namespace zpds
#endif  // _ZPDS_SEARCH_SEARCH_CACHE_HPP_

