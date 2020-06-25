/**
 * @project zapdos
 * @file include/store/HandleAllNames.hpp
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
 *  HandleAllNames.hpp : AllNames Function Handler Headers
 *
 */
#ifndef _ZPDS_STORE_HANDLE_ALLNAMES_HPP_
#define _ZPDS_STORE_HANDLE_ALLNAMES_HPP_

#include "store/StoreBase.hpp"
#include "utils/SharedTable.hpp"

namespace zpds {
namespace store {

class HandleAllNames {

public:

	/**
	* GetAllNames : read all names
	*
	* @param stptr
	*   ::zpds::utils::SharedTable::pointer stptr
	*
	* @param keytype
	*   ::zpds::store::KeyTypeE keytype for tags only
	*
	* @return
	*   std::vector<std::string> vector of names
	*/
	virtual std::vector<std::string> GetAllNames(::zpds::utils::SharedTable::pointer stptr, KeyTypeE keytype)=0;

};
} // namespace store
} // namespace zpds
#endif /* _ZPDS_STORE_HANDLE_ALLNAMES_HPP_ */
