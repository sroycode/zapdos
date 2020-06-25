/**
 * @project zapdos
 * @file include/search/IndexBase.hpp
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
 *  IndexBase.hpp : Xapian Indexing Abstract Base Class Headers
 *
 */
#ifndef _ZPDS_SEARCH_INDEX_BASE_HPP_
#define _ZPDS_SEARCH_INDEX_BASE_HPP_

#include <mutex>
#include <memory>
#include <functional>
#include <unordered_map>
#include <xapian.h>

#include "search/BaseUtils.hpp"
#include "store/StoreBase.hpp"
#include "utils/SharedTable.hpp"

namespace zpds {
namespace search {
class IndexBase : virtual public ::zpds::store::StoreBase, virtual public BaseUtils {

public:
	using DatabaseT = Xapian::WritableDatabase;

	using WordPairT = BaseUtils::WordPairT;
	using WordVecT = BaseUtils::WordVecT;
	using WordMapT = BaseUtils::WordMapT;

	/**
	* AddRecord : add manip data using ItemDataT
	*
	* @param stptr
	*   ::zpds::utils::SharedTable::pointer stptr
	*
	* @param params
	*   ::zpds::store::ItemDataT* record
	*
	* @return
	*   none throws if not ok
	*/
	void AddRecord(::zpds::utils::SharedTable::pointer stptr, ::zpds::store::ItemDataT* record) const;

	/**
	* DelRecord : del manip data using ItemDataT
	*
	* @param stptr
	*   ::zpds::utils::SharedTable::pointer stptr
	*
	* @param params
	*   ::zpds::store::ItemDataT* record
	*
	* @return
	*   none throws if not ok
	*/
	void DelRecord(::zpds::utils::SharedTable::pointer stptr, ::zpds::store::ItemDataT* record) const;

protected:

	/**
	* GetIndexType : get the index type
	*
	* @return
	*   ::zpds::search::IndexTypeE indextype
	*/
	virtual IndexTypeE GetIndexType() const=0;

	/**
	* CreateDoc : create a new doc using ItemDataT
	*
	* @param params
	*   ::zpds::store::ItemDataT* record
	*
	* @return
	*   Xapian::Document
	*/
	virtual Xapian::Document CreateDoc(::zpds::store::ItemDataT* record) const=0;

	/**
	* IndexFullWords : index items without part
	*
	* @param doc
	*   Xapian::Document& doc
	*
	* @param item
	*   const std::string item to index
	*
	* @param full_prefix
	*   const std::string full prefix
	*
	* @param with_stem
	*   bool if with stem
	*
	* @param zero_pos
	*   bool if with pos0
	*
	* @return
	*   none
	*/
	void IndexFullWords(Xapian::Document& doc, const std::string item, const std::string full_prefix, bool with_stem, bool zero_pos) const;

	/**
	* IndexOneWord : index one word as is
	*
	* @param doc
	*   Xapian::Document& doc
	*
	* @param item
	*   const std::string item to index
	*
	* @param full_prefix
	*   const std::string full prefix
	*
	* @return
	*   none
	*/
	void IndexOneWord(Xapian::Document& doc, const std::string item, const std::string full_prefix) const;

	/**
	* IndexWithPart : index items with part
	*
	* @param doc
	*   Xapian::Document& doc
	*
	* @param item
	*   const std::string item to index
	*
	* @param full_prefix
	*   const std::string full prefix
	*
	* @param part_prefix
	*   const std::string part prefix
	*
	* @param with_stem
	*   bool if with stem
	*
	* @param zero_pos
	*   bool if with pos0
	*
	* @return
	*   none
	*/
	void IndexWithPart(Xapian::Document& doc, const std::string item,
	                   const std::string full_prefix, const std::string part_prefix, bool with_stem, bool zero_pos) const;

	/**
	* IndexGeo : index geo
	*
	* @param doc
	*   Xapian::Document& doc
	*
	* @param double lat
	*   const double latitude
	*
	* @param double lon
	*   const double longitude
	*
	* @return
	*   none
	*/
	void IndexGeo ( Xapian::Document& doc, const double lat, const double lon ) const;

};

} // namespace search
} // namespace zpds

#endif  // _ZPDS_SEARCH_INDEX_BASE_HPP_

