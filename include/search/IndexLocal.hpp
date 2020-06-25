/**
 * @project zapdos
 * @file include/search/IndexLocal.hpp
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
 *  IndexLocal.hpp : Xapian Indexing LocalData Headers
 *
 */
#ifndef _ZPDS_SEARCH_INDEX_LOCAL_HPP_
#define _ZPDS_SEARCH_INDEX_LOCAL_HPP_

#include "search/IndexBase.hpp"

namespace zpds {
namespace search {
class IndexLocal : virtual public IndexBase {

public:
	using pointer = std::shared_ptr<IndexLocal>;
	using DatabaseT = Xapian::WritableDatabase;

	using WordPairT = BaseUtils::WordPairT;
	using WordVecT = BaseUtils::WordVecT;
	using WordMapT = BaseUtils::WordMapT;

	/**
	* Constructor : default
	*
	*/
	IndexLocal();

	/**
	* make noncopyable and remove default
	*/
	IndexLocal(const IndexLocal&) = delete;
	IndexLocal& operator=(const IndexLocal&) = delete;

	/**
	* destructor
	*/
	virtual ~IndexLocal ();

protected:

	/**
	* GetIndexType : get the index type
	*
	* @return
	*   ::zpds::search::IndexTypeE indextype
	*/
	IndexTypeE GetIndexType() const override;

	/**
	* CreateDoc : create a new doc using ItemDataT
	*
	* @param params
	*   ::zpds::store::ItemDataT* record
	*
	* @return
	*   Xapian::Document 
	*/
	Xapian::Document CreateDoc(::zpds::store::ItemDataT* record) const override;

};

} // namespace search
} // namespace zpds

#endif  // _ZPDS_SEARCH_INDEX_LOCAL_HPP_

