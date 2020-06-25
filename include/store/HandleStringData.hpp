/**
 * @project zapdos
 * @file include/store/HandleStringData.hpp
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
 *  HandleStringData.hpp : Handle String Data fields and lists Headers
 *
 */
#ifndef _ZPDS_STORE_HANDLE_STRING_DATA_HPP_
#define _ZPDS_STORE_HANDLE_STRING_DATA_HPP_

#include "utils/SharedTable.hpp"

#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>

namespace zpds {
namespace store {

class HandleStringData {

public:

	/**
	* HandleStrings : handle string data arrays
	*
	* @param idata
	*   ::google::protobuf::RepeatedPtrField< std::string >* input strings
	*
	* @param odata
	*   ::google::protobuf::RepeatedPtrField< std::string >* input strings
	*
	* @param merge
	*   bool merge flag
	*
	* @return
	*   bool true if changed
	*/
	bool HandleStrings(
	    ::google::protobuf::RepeatedPtrField< std::string >* idata,
	    ::google::protobuf::RepeatedPtrField< std::string >* odata,
	    bool merge);

	/**
	* HandleUserCategories : handles categories
	*
	* @param stptr
	*   ::zpds::utils::SharedTable::pointer stptr
	*
	* @param idata
	*   ::google::protobuf::RepeatedPtrField< std::string >* input cat strings
	*
	* @param odata
	*   ::google::protobuf::RepeatedPtrField< std::string >* output cat strings
	*
	* @param merge
	*   bool merge data if found
	*
	* @return
	*   bool if changed
	*/
	bool HandleUserCategories(
	    ::zpds::utils::SharedTable::pointer stptr,
	    ::google::protobuf::RepeatedPtrField< std::string >* idata,
	    ::google::protobuf::RepeatedPtrField< std::string >* odata,
	    bool merge);


};
} // namespace store
} // namespace zpds
#endif /* _ZPDS_STORE_HANDLE_STRING_DATA_HPP_ */
