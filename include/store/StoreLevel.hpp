/**
 * @project zapdos
 * @file include/store/StoreLevel.hpp
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
 *  StoreLevel.hpp : Abstract Base Class for LevelDB Headers
 *
 */
#ifndef _ZPDS_STORE_STORELEVEL_HPP_
#define _ZPDS_STORE_STORELEVEL_HPP_

#include "store/StoreBase.hpp"

#ifdef ZPDS_BUILD_WITH_LEVELDB
#include <leveldb/db.h>
#include <leveldb/write_batch.h>
namespace usemydb = ::leveldb;
#elif ZPDS_BUILD_WITH_ROCKSDB
#include <rocksdb/db.h>
namespace usemydb = ::rocksdb;
#endif

namespace zpds {
namespace store {
class StoreLevel : virtual public StoreBase {
public:
#ifdef ZPDS_BUILD_WITH_LEVELDB
	using dbpointer=std::shared_ptr<leveldb::DB>;
#elif ZPDS_BUILD_WITH_ROCKSDB
	using dbpointer=std::shared_ptr<rocksdb::DB>;
#endif

	/**
	* Constructor
	*
	* @param trydb
	*   dbpointer shared pointer to db
	*
	*/
	StoreLevel(dbpointer trydb);

	/**
	* make noncopyable and remove default
	*/

	StoreLevel(const StoreLevel&) = delete;
	StoreLevel& operator=(const StoreLevel&) = delete;

	/**
	* Initialize: init main db
	*
	* @param datadir
	*   std::string& data directory to store
	*
	* @param cache_in_mb
	*   size_t cache size in MB
	*
	* @param last_pkey
	*   uint64_t& last primary key
	*
	* @param last_lkey
	*   uint64_t& last log key
	*
	* @return
	*   none
	*/
	void Initialize(const std::string& datadir, const size_t cache_in_mb, uint64_t& last_pkey, uint64_t& last_lkey);

	/**
	* getDB: Get shared pointer to DB
	*
	* @return
	*   dbpointer
	*/
	dbpointer getDB();

protected:
	dbpointer db;

};
} // namespace store
} // namespace zpds
#endif /* _ZPDS_STORE_STORELEVEL_HPP_ */
