/**
 * @project zapdos
 * @file include/search/StoreTrie.hpp
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
 *  StoreTrie :  container for Trie
 *
 */
#ifndef _ZPDS_SEARCH_STORE_TRIE_HPP_
#define _ZPDS_SEARCH_STORE_TRIE_HPP_

#include <boost/thread/locks.hpp>
#include <boost/thread/shared_mutex.hpp>
#include "store/StoreBase.hpp"
#include "search/TrieBase.hpp"

#include <memory>
#include <functional>
#include <unordered_map>
#include <xapian.h>

namespace zpds {
namespace search {
class StoreTrie : virtual public ::zpds::store::StoreBase, virtual public TrieBase {

public:
	using DatabaseT = Xapian::WritableDatabase;
	using TrieMapT = std::unordered_map< int, DatabaseT >;

	using pointer = std::shared_ptr<StoreTrie>;

	using WordPairT = TrieBase::WordPairT;
	using WordVecT = TrieBase::WordVecT;
	using WordMapT = TrieBase::WordMapT;

	/**
	* Constructor : default
	*
	* @param dbpath_
	*   std::string dbpath
	*
	*/
	StoreTrie(std::string dbpath_);

	/**
	* make noncopyable and remove default
	*/
	StoreTrie() = delete;
	StoreTrie(const StoreTrie&) = delete;
	StoreTrie& operator=(const StoreTrie&) = delete;

	/**
	* destructor
	*/
	virtual ~StoreTrie ();

	/**
	* AddLocalData : add search data point
	*
	* @param params
	*   ::zpds::store::LookupRecordT* record
	*
	* @return
	*   none throws if not ok
	*/
	void AddLocalData(::zpds::store::LookupRecordT* record);

	/**
	* AddTextData : add search data point for text records
	*
	* @param params
	*   ::zpds::store::TextRecordT* record
	*
	* @return
	*   none throws if not ok
	*/
	void AddTextData(::zpds::store::TextRecordT* record);

	/**
	* DelLocalData : delete
	*
	* @param params
	*   ::zpds::store::LookupRecordT* record
	*
	* @return
	*   none
	*/
	void DelLocalData(::zpds::store::LookupRecordT* record);

	/**
	* DelTextData : delete
	*
	* @param params
	*   ::zpds::store::TextRecordT* record
	*
	* @return
	*   none
	*/
	void DelTextData(::zpds::store::TextRecordT* record);

protected:
	const std::string dbpath;
	TrieMapT triemap;

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
	* IndexFullWords : index items with part
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
	void IndexFullWords(Xapian::Document& doc, const std::string item, const std::string full_prefix, bool with_stem, bool zero_pos);

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
	                   const std::string full_prefix, const std::string part_prefix, bool with_stem, bool zero_pos);

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
	void IndexGeo ( Xapian::Document& doc, const double lat, const double lon );

};

} // namespace store
} // namespace zpds
#endif  // _ZPDS_SEARCH_STORE_TRIE_HPP_

