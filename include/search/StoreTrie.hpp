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
	using TrieMapT = std::unordered_map< int , DatabaseT >;

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
	* DelData : delete
	*
	* @param params
	*   ::zpds::store::LookupRecordT* record
	*
	* @return
	*   none
	*/
	void DelData(::zpds::store::LookupRecordT* record);

	/**
	* AddData : add search data point
	*
	* @param params
	*   ::zpds::store::LookupRecordT* record
	*
	* @return
	*   none throws if not ok
	*/
	void AddData(::zpds::store::LookupRecordT* record);

protected:
	const std::string dbpath;
	TrieMapT triemap;

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

};

} // namespace store
} // namespace zpds
#endif  // _ZPDS_SEARCH_STORE_TRIE_HPP_

