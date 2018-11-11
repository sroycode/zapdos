/**
 * @project zapdos
 * @file include/store/PhotonService.hpp
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
 *  PhotonService.hpp : Header for completion service
 *
 */
#ifndef _ZPDS_STORE_PHOTON_SERVICE_HPP_
#define _ZPDS_STORE_PHOTON_SERVICE_HPP_

#include <memory>
#include <boost/thread/locks.hpp>

#include "query/QueryBase.hpp"
#include "store/ProfileService.hpp"
#include "search/TrieBase.hpp"

namespace zpds {
namespace store {

class PhotonService : virtual public ProfileService , virtual public ::zpds::search::TrieBase {
public:

	/**
	* Constructor private
	*
	* @param stptr_
	*   ::zpds::utils::SharedTable::pointer stptr
	*
	*/
	PhotonService( ::zpds::utils::SharedTable::pointer stptr_ );

	/**
	* make noncopyable
	*/
	PhotonService() = delete;
	PhotonService(const PhotonService&) = delete;
	PhotonService& operator=(const PhotonService&) = delete;

	/**
	* Destructor
	*
	*/
	~PhotonService();

	/**
	* GetCompleteAction
	*
	* @param params
	*   ::zpds::query::PhotonParamsT* params
	*
	* @return
	*   none
	*/
	void GetCompleteAction (::zpds::query::PhotonParamsT* params);

private:
	::zpds::utils::SharedTable::pointer stptr;
	boost::mutex mtx;

	/**
	* RuleSearch : do the search from rule
	*
	* @param mp
	*   ::zpds::search::UsedParamsT query params copy
	*
	* @param rule
	*   ::zpds::search::QueryOrderT query order rule copy
	*
	* @param records
	*   ::zpds::store::LookupRecordListT* records to fetch
	*
	* @param target
	*   size_t no of records to fetch
	*
	* @return
	*   bool if found
	*/
	bool RuleSearch(
	    ::zpds::search::UsedParamsT qr,
	    ::zpds::search::QueryOrderT rule,
	    ::zpds::store::LookupRecordListT* records,
	    size_t target) const;

};
} // namespace store
} // namespace zpds
#endif /* _ZPDS_STORE_PHOTON_SERVICE_HPP_ */
