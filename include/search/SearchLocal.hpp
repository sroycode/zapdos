/**
 * @project zapdos
 * @file include/search/SearchLocal.hpp
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
 *  SearchLocal.hpp : Xapian Search for LocalData Headers
 *
 */
#ifndef _ZPDS_SEARCH_SEARCH_LOCAL_HPP_
#define _ZPDS_SEARCH_SEARCH_LOCAL_HPP_

#include "search/SearchBase.hpp"

namespace zpds {
namespace search {
class SearchLocal : virtual public SearchBase {

public:
	using DatabaseT = ReadIndex::DatabaseT;

	using WordPairT = BaseUtils::WordPairT;
	using WordVecT = BaseUtils::WordVecT;
	using WordMapT = BaseUtils::WordMapT;

	/**
	* Constructor : default
	*
	* @param dbpath
	*   const std::string& dbpath
	*
	*/
	SearchLocal(const std::string& dbpath);

	/**
	* make noncopyable and remove default
	*/
	SearchLocal() = delete;
	SearchLocal(const SearchLocal&) = delete;
	SearchLocal& operator=(const SearchLocal&) = delete;

	/**
	* destructor
	*/
	virtual ~SearchLocal ();

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

};

} // namespace search
} // namespace zpds
#endif  // _ZPDS_SEARCH_SEARCH_LOCAL_HPP_

