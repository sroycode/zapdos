/**
 * @project zapdos
 * @file include/store/HandleSession.hpp
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
 *  HandleSession.hpp : Session Checker Headers
 *
 */
#ifndef _ZPDS_STORE_HANDLE_SESSION_HPP_
#define _ZPDS_STORE_HANDLE_SESSION_HPP_

#include "store/StoreBase.hpp"
#include "utils/SharedTable.hpp"

namespace zpds {
namespace store {

class HandleSession : virtual public StoreBase  {
public:

protected:
	/**
	* CheckSession : handle valid session
	*
	* @param stptr
	*   ::zpds::utils::SharedTable::pointer stptr
	*
	* @param data
	*   ::zpds::store::UserDataT* data in
	*
	* @param onlyadmin
	*   bool only allow admin aka exter , dafault false
	*
	* @return
	*   bool ok
	*/
	bool CheckSession(::zpds::utils::SharedTable::pointer stptr, ::zpds::store::UserDataT* data, bool onlyadmin=false);

};
} // namespace store
} // namespace zpds
#endif /* _ZPDS_STORE_HANDLE_SESSION_HPP_ */
