/**
 * @project zapdos
 * @file src/store/CacheContainer.cc
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
 *  CacheContainer.cc : Cache Container impl
 *
 */
#include "store/CacheContainer.hpp"

/**
* Constructor : default private
*
*/
zpds::store::CacheContainer::CacheContainer () {}

/**
* destructor
*/
zpds::store::CacheContainer::~CacheContainer () {}

/**
* SetAssoc : add an assoc entry
*
*/
void zpds::store::CacheContainer::SetAssoc(std::string hash, zpds::store::CacheContainer::AssocT assoc, bool only_if_exists)
{
	WriteLockT writelock(mutex_);
	DLOG(INFO) << "Added Assoc Cache: " << hash;
	if (only_if_exists) {
		if ( assoc_map.find(hash) == assoc_map.end() ) return;
	}
	assoc_map[hash] = assoc;
}

/**
* GetAssoc : get assoc
*
*/
bool zpds::store::CacheContainer::GetAssoc(std::string hash, zpds::store::CacheContainer::AssocT& assoc)
{
	ReadLockT readlock(mutex_);
	auto it = assoc_map.find(hash);
	if (it != assoc_map.end()) {
		assoc = it->second;
		return true;
	}
	return false;
}

/**
* DelAssoc : del assoc entry
*
*/
void zpds::store::CacheContainer::DelAssoc(std::string hash)
{
	WriteLockT writelock(mutex_);
	DLOG(INFO) << "Delete Assoc Cache: " << hash;
	auto it = assoc_map.find(hash);
	if (it != assoc_map.end())
		assoc_map.erase(it);
}

/**
* CheckAssoc : get if assoc
*
*/
bool zpds::store::CacheContainer::CheckAssoc(std::string hash)
{
	ReadLockT readlock(mutex_);
	return (assoc_map.find(hash) != assoc_map.end());
}

/**
* AssocSize : get assoc size
*
*/
size_t zpds::store::CacheContainer::AssocSize()
{
	ReadLockT readlock(mutex_);
	return assoc_map.size();
}
