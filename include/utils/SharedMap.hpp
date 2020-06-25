/**
 * @project zapdos
 * @file include/utils/SharedMap.hpp
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
 *  SharedMap.hpp : Shared Map for SharedTable Headers
 *
 */
#ifndef _ZPDS_UTILS_SHARED_MAP_HPP_
#define _ZPDS_UTILS_SHARED_MAP_HPP_

#include <unordered_map>
#include <utility>
#include <functional>
#include <boost/thread/locks.hpp>
#include <boost/thread/shared_mutex.hpp>

namespace zpds {
namespace utils {
template<class KeyT,class ValueT>
class SharedMap {
public:
	using MapT = std::unordered_map<KeyT,ValueT>;
	using LockT = boost::shared_mutex;
	using WriteLockT = boost::unique_lock< LockT >;
	using ReadLockT = boost::shared_lock< LockT >;

	/**
	* make noncopyable and remove default
	*/

	SharedMap(const SharedMap&) = delete;
	SharedMap& operator=(const SharedMap&) = delete;

	/**
	* Constructor : default
	*
	*/
	SharedMap() {}

	/**
	* destructor
	*/
	virtual ~SharedMap () {}

	/**
	* AddOne : add one value
	*
	* @param key
	*   KeyT elem to add
	*
	* @param value
	*  ValueT elem to add
	*
	* @return
	*   none
	*/
	void AddOne(KeyT key, ValueT value)
	{
		WriteLockT writelock(mutex_);
		t_[key]=value;
	}

	/**
	* AddMany : add from map by address
	*
	* @param m
	*   const Map& map to get from
	*
	* @return
	*   none
	*/
	void AddMany(const MapT& m)
	{
		WriteLockT writelock(mutex_);
		for (auto& p : m) {
			t_[p.first]=p.second;
		}
	}

	/**
	* AddMany : add from map by lvalue ref
	*
	* @param m
	*   const Map&& map to get from
	*
	* @return
	*   none
	*/
	void AddMany(MapT&& m)
	{
		WriteLockT writelock(mutex_);
		for (auto& p : m) {
			t_[p.first]=p.second;
		}
	}

	/**
	* GetValues : get values by address
	*
	* @param key
	*   KeyT& elem to get
	*
	* @param value
	*   ValueT& elem to get
	*
	* @return
	*   bool true if exists
	*/
	bool GetValues(KeyT& key, ValueT& value)
	{
		ReadLockT readlock(mutex_);
		typename MapT::const_iterator it = t_.find(key);
		bool stat = (it!=t_.end());
		value = stat ? it.second : ValueT();
		return stat;
	}

	/**
	* GetValues : get values by lvalue ref
	*
	* @param key
	*   KeyT&& elem to get
	*
	* @param value
	*   ValueT& elem to get
	*
	* @return
	*   bool true if exists
	*/
	bool GetValues(KeyT&& key, ValueT& value)
	{
		ReadLockT readlock(mutex_);
		typename MapT::const_iterator it = t_.find(key);
		bool stat = (it!=t_.end());
		value = stat ? it.second : ValueT();
		return stat;
	}

	/**
	* GetCopy : get a copy of the map
	*
	* @return
	*   MapT map
	*/
	MapT GetCopy()
	{
		ReadLockT readlock(mutex_);
		return t_;
	}

	/**
	* GetSize: get size of set
	*
	* @return
	*   size_t size
	*/
	size_t GetSize()
	{
		ReadLockT readlock(mutex_);
		return t_.size();
	}

	/**
	* Reset: clear the set
	*
	* @return
	*   none
	*/
	void Reset()
	{
		WriteLockT writelock(mutex_);
		t_.clear();
	}

	/**
	* Swap : get by swap
	*
	* @return
	*   none
	*/
	void Swap(MapT t)
	{
		WriteLockT writelock(mutex_);
		t_.swap(t);
	}


protected:
	MapT t_;
	LockT mutex_;

};
} // namespace utils
} // namespace zpds
#endif /* _ZPDS_UTILS_SHARED_MAP_HPP_ */
