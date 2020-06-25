/**
 * @project zapdos
 * @file include/store/ExterDataService.hpp
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
 *  ExterDataService.hpp : Exter Data Manipulation Headers
 *
 */
#ifndef _ZPDS_STORE_EXTERDATA_SERVICE_HPP_
#define _ZPDS_STORE_EXTERDATA_SERVICE_HPP_

#include "store/UserDataService.hpp"

namespace zpds {
namespace store {

class ExterDataService : virtual public UserDataService {
public:
	// using pointer = boost::scoped_ptr<ExterDataService>;

	/**
	* AdminCreateAction : create admin exter user
	*
	* @param stptr
	*   ::zpds::utils::SharedTable::pointer stptr
	*
	* @param user
	*   ::zpds::store::ExterDataT* user
	*
	* @param update
	*   bool update flag
	*
	* @param is_admin
	*   bool set true if admin
	*
	* @param has_profile
	*   bool set true if has default profile
	*
	* @return
	*   none throws if not ok
	*/
	void AdminCreateAction(::zpds::utils::SharedTable::pointer stptr, ::zpds::store::ExterDataT* user,
	                       bool update, bool is_admin, bool has_profile);

	/**
	* ManageDataAction : update externaldata
	*
	* @param stptr
	*   ::zpds::utils::SharedTable::pointer stptr
	*
	* @param resp
	*   ::zpds::query::UserDataRespT* resp
	*
	* @return
	*   none throws if not ok
	*/
	void ManageDataAction(::zpds::utils::SharedTable::pointer stptr, ::zpds::query::UserDataRespT* resp) override;

	/**
	* ResetPassAction : update password
	*
	* @param stptr
	*   ::zpds::utils::SharedTable::pointer stptr
	*
	* @param resp
	*   ::zpds::query::ResetRespT* resp
	*
	* @return
	*   none throws if not ok
	*/
	void ResetPassAction(::zpds::utils::SharedTable::pointer stptr, ::zpds::query::ResetRespT* resp) override;

	/**
	* ReadDataAction : read externaldata
	*
	* @param stptr
	*   ::zpds::utils::SharedTable::pointer stptr
	*
	* @param resp
	*   ::zpds::query::UserDataRespT* resp
	*
	* @return
	*   none throws if not ok
	*/
	void ReadDataAction(::zpds::utils::SharedTable::pointer stptr, ::zpds::query::UserDataRespT* resp) override;

protected:

	/**
	* AllowedFieldsFilter : sets update variable
	*
	* @param update
	*   int32_t input update
	*
	* @return
	*   int32_t value
	*/
	int32_t AllowedFieldsFilter(int32_t update) override;

	/**
	* GetKeyType : handles keytype for tags
	*
	* @return
	*   KeyTypeE keytype primary key
	*/
	KeyTypeE GetKeyType() const override;

	/**
	* PrepareData : sets userdata
	*
	* @param stptr
	*   ::zpds::utils::SharedTable::pointer stptr
	*
	* @param updater
	*   ::zpds::store::ExterDataT* updater
	*
	* @param data
	*   ::zpds::store::ExterDataT* data
	*
	* @param action
	*   int32_t action use ZPDS_UACTION_* with logical
	*
	* @param fields
	*   int32_t fields fields to modify use ZPDS_UDFLD_* with logical
	*
	* @param merge
	*   bool merge data if found
	*
	* @return
	*   returns if found
	*/
	bool PrepareData(::zpds::utils::SharedTable::pointer stptr,
	                 ::zpds::store::ExterDataT* updater,
	                 ::zpds::store::ExterDataT* data,
	                 int32_t action, int32_t fields, bool merge) override;

	/**
	* ReadData : sets exterdata for read
	*
	* @param stptr
	*   ::zpds::utils::SharedTable::pointer stptr
	*
	* @param reader
	*   ::zpds::store::ExterDataT* reader
	*
	* @param data
	*   ::zpds::store::ExterDataT* data
	*
	* @param fields
	*   int32_t fields fields use ZPDS_UDFLD_* with logical
	*
	* @return
	*   none
	*/
	void ReadData(::zpds::utils::SharedTable::pointer stptr,
	              ::zpds::store::ExterDataT* reader,
	              ::zpds::store::ExterDataT* data, int32_t fields) override;

};
} // namespace store
} // namespace zpds
#endif /* _ZPDS_STORE_EXTERDATA_SERVICE_HPP_ */
