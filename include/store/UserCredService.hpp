/**
 * @project zapdos
 * @file include/store/UserCredService.hpp
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
 *  UserCredService.hpp : Non Abstract Base Class handles User Credentials handling Headers
 *
 */
#ifndef _ZPDS_STORE_USERCRED_SERVICE_HPP_
#define _ZPDS_STORE_USERCRED_SERVICE_HPP_

#include "store/StoreBase.hpp"
#include "query/QueryBase.hpp"

#include "store/CredServiceBase.hpp"
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>

namespace zpds {
namespace store {

class UserCredService : public CredServiceBase  {
public:
	// using pointer = boost::scoped_ptr<UserCredService>;

	/**
	* Get : gets the user
	*
	* @param stptr
	*   ::zpds::utils::SharedTable::pointer stptr
	*
	* @param data
	*   ::zpds::query::UserDataT* data
	*
	* @return
	*   none throws if not ok
	*/
	void Get(::zpds::utils::SharedTable::pointer stptr, ::zpds::store::UserDataT* data) const override;

	/**
	* Pack : shrink to bare essentials for packing into cache
	*
	* @param data
	*   ::zpds::query::UserDataT* data
	*
	* @return
	*   std::string packed string
	*/
	std::string Pack(::zpds::store::UserDataT* data) const override;

	/**
	* ValidateUserCategories : gets the user and categories
	*
	* @param stptr
	*   ::zpds::utils::SharedTable::pointer stptr
	*
	* @param data
	*   ::zpds::query::UserDataT* data
	*
	* @param categories
	*   ::google::protobuf::RepeatedPtrField< std::string>* mutable categories
	*
	* @return
	*   bool status if ok
	*/
	bool ValidateUserCategories(::zpds::utils::SharedTable::pointer stptr, ::zpds::store::UserDataT* data,
	                  ::google::protobuf::RepeatedPtrField< std::string>* categories) const;

protected:
};
} // namespace store
} // namespace zpds
#endif /* _ZPDS_STORE_USERCRED_SERVICE_HPP_ */
