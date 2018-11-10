/**
 * @project zapdos
 * @file include/store/CacheContainer.hpp
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
 *  CacheContainer.hpp :   Shared headers for store cache
 *
 */
#ifndef _ZPDS_STORE_CACHE_CONTAINER_HPP_
#define _ZPDS_STORE_CACHE_CONTAINER_HPP_

#include <memory>
#include "sparsepp/spp.h"
#include <utility>
#include <functional>
#include <boost/thread/locks.hpp>
#include <boost/thread/shared_mutex.hpp>

#include "store/StoreBase.hpp"

namespace zpds {
namespace store {
class CacheContainer : virtual public StoreBase {
public:
	using pointer=std::shared_ptr<CacheContainer>;
	using LockT = boost::shared_mutex;
	using WriteLockT = boost::unique_lock< LockT >;
	using ReadLockT = boost::shared_lock< LockT >;

	using AssocT = std::string;
	using AssocMapT = spp::sparse_hash_map<std::string,AssocT>;

	/**
	* create : static construction creates new first time
	*
	* @return
	*   pointer
	*/
	static pointer create()
	{
		pointer p(new CacheContainer());
		return p;
	}

	/** no copy */
	CacheContainer(const CacheContainer&) = delete;
	CacheContainer& operator=(const CacheContainer&) = delete;

	/**
	* destructor
	*/
	virtual ~CacheContainer ();

	/**
	* SetAssoc : add an assoc entry
	*
	* @param hash
	*   std::string hash to update
	*
	* @param assoc
	*   AssocT assoc
	*
	* @return
	*   none
	*/
	void SetAssoc(std::string hash, AssocT assoc);


	/**
	* GetAssoc : get assoc
	*
	* @param hash
	*   std::string hash to get
	*
	* @param assoc
	*   AssocT& assoc to get
	*
	* @return
	*   bool if ok
	*/
	bool GetAssoc(std::string hash, AssocT& assoc);

	/**
	* DelAssoc : del assoc entry
	*
	* @param hash
	*   std::string&& hash to update
	*
	* @return
	*   none
	*/
	void DelAssoc(std::string hash);

private:
	LockT mutex_;
	AssocMapT assoc_map;

	/**
	* Constructor : default private
	*
	*/
	CacheContainer();

};
} // namespace store
} // namespace zpds
#endif /* _ZPDS_STORE_CACHE_CONTAINER_HPP_ */
