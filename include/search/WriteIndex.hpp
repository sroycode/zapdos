/**
 * @project zapdos
 * @file include/search/WriteIndex.hpp
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
 *  WriteIndex.hpp : Xapian Writable indexes. Note this is ONLY for updates
 *
 */
#ifndef _ZPDS_SEARCH_WRITE_INDEX_HPP_
#define _ZPDS_SEARCH_WRITE_INDEX_HPP_

#include <mutex>
#include <memory>
#include <functional>
#include <unordered_map>
#include <xapian.h>

#include "utils/BaseUtils.hpp"
#include "../proto/Search.pb.h"

namespace zpds {
namespace search {
class WriteIndex {

public:

	using DatabaseT = Xapian::WritableDatabase;
	using TrieMapT = std::unordered_map< int, DatabaseT >;

	using pointer = std::shared_ptr<WriteIndex>;

	/**
	* Create : create WriteIndex
	*
	* @param dbpath_
	*   std::string dbpath
	*
	* @return
	*   std::shared_ptr<WriteIndex>
	*
	*/
	static pointer Create(std::string dbpath_)
	{
		return std::make_shared<WriteIndex>(std::move(dbpath_));
	}

	/**
	* Constructor : default
	*
	* @param dbpath_
	*   std::string dbpath
	*
	*/
	WriteIndex(std::string dbpath_);

	/**
	* make noncopyable and remove default
	*/
	WriteIndex() = delete;
	WriteIndex(const WriteIndex&) = delete;
	WriteIndex& operator=(const WriteIndex&) = delete;

	/**
	* destructor
	*/
	virtual ~WriteIndex ();

	/**
	* CommitData : commits data
	*
	* @return
	*   none throws if not ok
	*/
	void CommitData();

	/**
	* Update: update a document
	*
	* @param ltyp
	*   ::zpds::search::LangTypeE ltyp
	*
	* @param dtyp
	*   ::zpds::search::IndexTypeE dtyp
	*
	* @param idterm
	*   const std::string& idterm to update
	*
	* @param doc
	*   ::Xapian::Document&& doc moved
	*
	* @return
	*   none
	*/
	virtual void Update(::zpds::search::LangTypeE ltyp, ::zpds::search::IndexTypeE dtyp,
	                    const std::string& idterm, ::Xapian::Document&& doc);

	/**
	* Delete: delete a document
	*
	* @param ltyp
	*   ::zpds::search::LangTypeE ltyp
	*
	* @param dtyp
	*   ::zpds::search::IndexTypeE dtyp
	*
	* @param idterm
	*   const std::string& idterm to update
	*
	* @return
	*   none
	*/
	void Delete(::zpds::search::LangTypeE ltyp, ::zpds::search::IndexTypeE dtyp, const std::string& idterm);

protected:
	std::mutex update_lock;
	const std::string dbpath;
	TrieMapT triemap;

	/**
	* Get: get the storage instance
	*
	* @param ltyp
	*   ::zpds::search::LangTypeE ltyp
	*
	* @param dtyp
	*   ::zpds::search::IndexTypeE dtyp
	*
	* @return
	*   DatabaseT&
	*/
	virtual DatabaseT& Get(::zpds::search::LangTypeE ltyp, ::zpds::search::IndexTypeE dtyp);

};

} // namespace search
} // namespace zpds

#endif  // _ZPDS_SEARCH_WRITE_INDEX_HPP_

