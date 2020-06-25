/**
 * @project zapdos
 * @file include/store/HandleGeneric.hpp
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
 *  HandleGeneric.hpp : Handle Generic Filters for Service Classes Headers
 *
 */
#ifndef _ZPDS_STORE_HANDLE_FILTERS_HPP_
#define _ZPDS_STORE_HANDLE_FILTERS_HPP_

#include "query/QueryBase.hpp"
#include "store/StoreBase.hpp"

namespace zpds {
namespace store {

class HandleGeneric {

protected:

	/**
	* WriteActionFilter : sets action variable
	*
	* @param writetype
	*   ::zpds::query::WriteTypeE writetype
	*
	* @return
	*   std::pair<int32_t,bool> value
	*/
	std::pair<int32_t,bool> WriteActionFilter(::zpds::query::WriteTypeE writetype);

	/**
	* AllowedFieldsFilter : sets update variable, must change in derived class.
	*
	* @param update
	*   int32_t input update
	*
	* @return
	*   int32_t value
	*/
	virtual int32_t AllowedFieldsFilter(int32_t update)=0;

	/**
	* ReaderTypeFilter : sets action variable
	*
	* @param readtype
	*   ::zpds::query::ReaderTypeE readtype
	*
	* @return
	*   int32_t value
	*/
	int32_t ReaderTypeFilter(::zpds::query::ReaderTypeE readtype);

};
} // namespace store
} // namespace zpds
#endif /* _ZPDS_STORE_HANDLE_FILTERS_HPP_ */
