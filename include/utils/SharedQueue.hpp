/**
 * @project zapdos
 * @file include/utils/SharedQueue.hpp
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
 *  SharedQueue.hpp : Shared Queue for SharedTable Headers
 *
 */
#ifndef _ZPDS_UTILS_SHAREDQUEUE_HPP_
#define _ZPDS_UTILS_SHAREDQUEUE_HPP_

#include <tuple>
#include <queue>
#include <boost/thread/locks.hpp>
#include <boost/thread/shared_mutex.hpp>

namespace zpds {
namespace utils {
template<class KeyT>
class SharedQueue {
public:
	using QueueT = std::queue<KeyT>;
	using LockT = boost::shared_mutex;
	using WriteLockT = boost::unique_lock< LockT >;
	using ReadLockT = boost::shared_lock< LockT >;

	/**
	* make noncopyable and remove default
	*/

	SharedQueue(const SharedQueue&) = delete;
	SharedQueue& operator=(const SharedQueue&) = delete;

	/**
	* Constructor : default
	*
	*/
	SharedQueue() : counter(0) {}

	/**
	* destructor
	*/
	virtual ~SharedQueue () {}

	/**
	* Push : add one value
	*
	* @param key
	*   KeyT& elem to add
	*
	* @return
	*   none
	*/
	void Push(KeyT& key)
	{
		WriteLockT writelock(mutex_);
		t_.push(key);
		++counter;
	}

	/**
	* Push : add one lvalue
	*
	* @param key
	*   KeyT&& elem to add
	*
	* @return
	*   none
	*/
	void Push(KeyT&& key)
	{
		WriteLockT writelock(mutex_);
		t_.emplace(key);
		++counter;
	}

	/**
	* Pop : gets one value
	*
	* @param key
	*   KeyT& elem to get
	*
	* @return
	*  bool if ok
	*/
	bool Pop(KeyT& key)
	{
		WriteLockT writelock(mutex_);
		if (!t_.empty()) {
			key = t_.front();
			t_.pop();
			--counter;
			return true;
		}
		return false;
	}

	/**
	* GetSize : gets the size of queue
	*
	* @return
	*  uint64_t size
	*/
	uint64_t GetSize()
	{
		ReadLockT readlock(mutex_);
		return counter;
	}


protected:
	uint64_t counter;
	QueueT t_;
	LockT mutex_;

};
} // namespace utils
} // namespace zpds
#endif /* _ZPDS_UTILS_SHAREDQUEUE_HPP_ */
