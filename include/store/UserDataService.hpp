/**
 * @project zapdos
 * @file include/store/UserDataService.hpp
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
 *  UserDataService.hpp : Non Abstract Base Class handles User Data Manipulation Headers
 *
 */
#ifndef _ZPDS_STORE_USERDATA_SERVICE_HPP_
#define _ZPDS_STORE_USERDATA_SERVICE_HPP_

#include "query/QueryBase.hpp"
#include "store/HandleTagData.hpp"
#include "store/HandleSession.hpp"
#include "store/HandleGeneric.hpp"
#include "store/HandleStringData.hpp"

#include "store/ServiceDefine.hh" // will be referred from query

namespace zpds {
namespace store {

class UserDataService : virtual public HandleTagData, virtual public HandleSession, virtual public HandleGeneric,
	virtual public HandleStringData   {
public:
	// using pointer = boost::scoped_ptr<UserDataService>;

	/**
	* ManageDataAction : sets userdata
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
	virtual void ManageDataAction(::zpds::utils::SharedTable::pointer stptr, ::zpds::query::UserDataRespT* resp);

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
	virtual void ResetPassAction(::zpds::utils::SharedTable::pointer stptr, ::zpds::query::ResetRespT* resp);

	/**
	* ReadDataAction : read externaldata
	*
	* @param stptr
	*   ::zpds::utils::SharedTable::pointer stptr
	*
	* @param resp
	*   ::zpds::query::ExterDataRespT* resp
	*
	* @return
	*   none throws if not ok
	*/
	virtual void ReadDataAction(::zpds::utils::SharedTable::pointer stptr, ::zpds::query::UserDataRespT* resp);

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
	virtual int32_t AllowedFieldsFilter(int32_t update) override;

	/**
	* GetKeyType : handles keytype for tags
	*
	* @return
	*   KeyTypeE keytype primary key
	*/
	virtual KeyTypeE GetKeyType() const;

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
	*   ::zpds::store::UserDataT* data
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
	virtual bool PrepareData(::zpds::utils::SharedTable::pointer stptr,
	                         ::zpds::store::ExterDataT* updater,
	                         ::zpds::store::UserDataT* data,
	                         int32_t action, int32_t fields, bool merge);

	/**
	* ReadData : sets userdata for read
	*
	* @param stptr
	*   ::zpds::utils::SharedTable::pointer stptr
	*
	* @param reader
	*   ::zpds::store::UserDataT* reader
	*
	* @param data
	*   ::zpds::store::UserDataT* data
	*
	* @param fields
	*   int32_t fields fields use ZPDS_UDFLD_* with logical
	*
	* @return
	*   none
	*/
	virtual void ReadData(::zpds::utils::SharedTable::pointer stptr,
	                      ::zpds::store::UserDataT* reader,
	                      ::zpds::store::UserDataT* data, int32_t fields);

	/**
	* HandleQueryProfiles : handle profiles for a user
	*
	* @param idata
	*   ::google::protobuf::RepeatedPtrField< ::zpds::search::QueryProfT >* input profile data
	*
	* @param odata
	*   ::google::protobuf::RepeatedPtrField< ::zpds::search::QueryProfT >* output profile
	*
	* @param merge
	*   bool merge flag
	*
	* @return
	*   bool if changed
	*/
	bool HandleQueryProfiles(
	    ::google::protobuf::RepeatedPtrField< ::zpds::search::QueryProfT >* idata,
	    ::google::protobuf::RepeatedPtrField< ::zpds::search::QueryProfT >* odata,
	    bool merge);

	/**
	* SetDefaultProfile : handles setting default profile
	*
	* @param prof
	*   ::google::protobuf::RepeatedPtrField< ::zpds::search::QueryProfT >* input profiles
	*
	* @return
	*   none
	*/
	void SetDefaultProfile(::google::protobuf::RepeatedPtrField< ::zpds::search::QueryProfT >* prof);

};
} // namespace store
} // namespace zpds
#endif /* _ZPDS_STORE_USERDATA_SERVICE_HPP_ */
