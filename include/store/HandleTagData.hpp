/**
 * @project zapdos
 * @file include/store/HandleTagData.hpp
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
 *  HandleTagData.hpp : Class to manipulate TagDataT based Protobuf Msg Headers
 *
 */
#ifndef _ZPDS_STORE_HANDLE_TAGDATA_HPP_
#define _ZPDS_STORE_HANDLE_TAGDATA_HPP_

#include <vector>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include "store/StoreBase.hpp"
#include "utils/SharedTable.hpp"

namespace zpds {
namespace store {

class HandleTagData : virtual public StoreBase  {
public:

protected:

	/**
	* HandleUserTags : handles tags
	*
	* @param stptr
	*   ::zpds::utils::SharedTable::pointer stptr
	*
	* @param idata
	*   ::google::protobuf::RepeatedPtrField< ::zpds::store::TagDataT >* input tags
	*
	* @param odata
	*   ::google::protobuf::RepeatedPtrField< ::zpds::store::TagDataT >* output tags
	*
	* @param keytypes
	*   std::vector<::zpds::store::KeyTypeE> keytypes
	*
	* @param merge
	*   bool merge data if found
	*
	* @return
	*   bool true if changed
	*/
	bool HandleUserTags(::zpds::utils::SharedTable::pointer stptr,
	                    ::google::protobuf::RepeatedPtrField< ::zpds::store::TagDataT >* idata,
	                    ::google::protobuf::RepeatedPtrField< ::zpds::store::TagDataT >* odata,
	                    std::vector<::zpds::store::KeyTypeE> keytypes,
	                    bool merge);

	/**
	* HandleUserAttr : handles tags
	*
	* @param idata
	*   ::google::protobuf::RepeatedPtrField< ::zpds::store::TagDataT >* input attr
	*
	* @param odata
	*   ::google::protobuf::RepeatedPtrField< ::zpds::store::TagDataT >* output attr
	*
	* @param merge
	*   bool merge data if found
	*
	* @return
	*   bool true if changed
	*/
	bool HandleUserAttr(::google::protobuf::RepeatedPtrField< ::zpds::store::TagDataT >* idata,
	                    ::google::protobuf::RepeatedPtrField< ::zpds::store::TagDataT >* odata,
	                    bool merge);


};
} // namespace store
} // namespace zpds
#endif /* _ZPDS_STORE_HANDLE_TAGDATA_HPP_ */
