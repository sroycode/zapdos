/**
 * @project zapdos
 * @file include/store/CategoryService.hpp
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
 *  CategoryService.hpp : Non Abstract Base Class Category Data Headers
 *
 */
#ifndef _ZPDS_STORE_CATEGORY_SERVICE_HPP_
#define _ZPDS_STORE_CATEGORY_SERVICE_HPP_

#include "store/HandleSession.hpp"
#include "store/StoreBase.hpp"
#include "query/QueryBase.hpp"
#include "store/HandleGeneric.hpp"
#include "store/HandleAllNames.hpp"

namespace zpds {
namespace store {

class CategoryService : virtual public StoreBase, virtual public HandleSession,
	virtual public HandleGeneric, virtual public HandleAllNames  {
public:
	// using pointer = boost::scoped_ptr<CategoryService>;

	/**
	* Get : gets the category data
	*
	* @param stptr
	*   ::zpds::utils::SharedTable::pointer stptr
	*
	* @param data
	*   ::zpds::query::CategoryT* data
	*
	* @return
	*   none throws if not ok
	*/
	virtual void Get(::zpds::utils::SharedTable::pointer stptr, ::zpds::store::CategoryT* data) const;

	/**
	* Pack : shrink to bare essentials for packing into cache
	*
	* @param data
	*   ::zpds::query::CategoryT* data
	*
	* @return
	*   std::string packed string
	*/
	virtual std::string Pack(::zpds::store::CategoryT* data) const;

	/**
	* ManageDataAction : sets category data
	*
	* @param stptr
	*   ::zpds::utils::SharedTable::pointer stptr
	*
	* @param resp
	*   ::zpds::query::CategoryRespT* resp
	*
	* @return
	*   none throws if not ok
	*/
	virtual void ManageDataAction(::zpds::utils::SharedTable::pointer stptr, ::zpds::query::CategoryRespT* resp);

	/**
	* ReadDataAction : read Category
	*
	* @param stptr
	*   ::zpds::utils::SharedTable::pointer stptr
	*
	* @param resp
	*   ::zpds::query::CategoryRespT* resp
	*
	* @return
	*   none throws if not ok
	*/
	virtual void ReadDataAction(::zpds::utils::SharedTable::pointer stptr, ::zpds::query::CategoryRespT* resp);

	/**
	* GetAllNames : read all names
	*
	* @param stptr
	*   ::zpds::utils::SharedTable::pointer stptr
	*
	* @param keytype
	*   ::zpds::store::KeyTypeE keytype is for tags only , defaults to K_NONODE
	*
	* @return
	*   std::vector<std::string> vector of names
	*/
	virtual std::vector<std::string> GetAllNames(::zpds::utils::SharedTable::pointer stptr, KeyTypeE keytype=K_NONODE) override;


protected:

	/**
	* AllowedFieldsFilter : sets update variable, not needed
	*
	* @param update
	*   int32_t input update
	*
	* @return
	*   int32_t value
	*/
	virtual int32_t AllowedFieldsFilter(int32_t update) override;

};
} // namespace store
} // namespace zpds
#endif /* _ZPDS_STORE_CATEGORY_SERVICE_HPP_ */

