/**
 * @project zapdos
 * @file include/store/TempNameCache.hpp
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
 *  TempNameCache.hpp : Cache of temporary unique fields Headers
 *
 */
#ifndef _ZPDS_STORE_TEMP_NAME_CACHE_HPP_
#define _ZPDS_STORE_TEMP_NAME_CACHE_HPP_

#include "store/StoreTable.hpp"
#include "utils/SharedTable.hpp"
#include <unordered_set>

#define ZPDS_LOCK_SLAVE_UPDATES 999
#define ZPDS_LOCK_CATEGORY_HOST 1001

namespace zpds {
namespace store {
class TempNameCache : public StoreTable<NodeT> {
public:
	using StoreTable<NodeT>::StoreTable;

	/**
	* Constructor : default str
	*
	* @param stptr
	*   ::zpds::utils::SharedTable::pointer stptr
	*
	*/
	TempNameCache(::zpds::utils::SharedTable::pointer stptr);

	/** no copy */
	TempNameCache() = delete;
	TempNameCache(const TempNameCache&) = delete;
	TempNameCache& operator=(const TempNameCache&) = delete;

	/**
	* destructor
	*
	*/
	~TempNameCache();

	/**
	* GetKey: get a key
	*
	* @param record
	*   NodeT* record
	*
	* @param keytype
	*   KeyTypeE key type for index
	*
	* @param pre
	*   bool non unique keys
	*
	* @return
	*   std::string key
	*/
	std::string GetKey(NodeT* record, KeyTypeE keytype, bool pre) override;

	/**
	* CheckLocal: check if this key is locally duplicate
	*
	* @param keytype
	*   ::zpds::store::KeyTypeE keytype
	*
	* @param indata
	*   const std::string& input string to check
	*
	* @return
	*   bool if not found and reserved
	*/
	bool CheckLocal(KeyTypeE keytype, const std::string& indata);

	/**
	* ReserveName: reserve this string should be new
	*
	* @param keytype
	*   ::zpds::store::KeyTypeE keytype
	*
	* @param indata
	*   const std::string& input string to check
	*
	* @param isnew
	*   bool flag if new, default false
	*
	* @return
	*   bool if not found and reserved
	*/
	bool ReserveName(KeyTypeE keytype, const std::string& indata, bool isnew=false);

	/**
	* MultiTypeLock: Lock for multiple types concurrent updates
	*
	* @param locktype
	*   ::google::protobuf::uint64  locktype to lock
	*
	* @return
	*   bool if not found and reserved
	*/
	bool MultiTypeLock(::google::protobuf::uint64 locktype);

private:
	::zpds::store::CacheContainer::pointer tmpcache;
	std::set<std::string> inset;

	/**
	* MakeKey: make a key
	*
	* @param keytype
	*   KeyTypeE key type for index
	*
	* @param indata
	*   const std::string& input string to check
	*
	* @return
	*   std::string key
	*/
	std::string MakeKey(KeyTypeE keytype, const std::string& indata);

};
} // namespace store
} // namespace zpds
#endif /* _ZPDS_STORE_TEMP_NAME_CACHE_HPP_ */
