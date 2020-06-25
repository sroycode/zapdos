/**
 * @project zapdos
 * @file include/store/ItemDataService.hpp
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
 *  ItemDataService.hpp : Abstract Base Class for all ItemData types
 *
 */
#ifndef _ZPDS_STORE_ITEMDATA_SERVICE_HPP_
#define _ZPDS_STORE_ITEMDATA_SERVICE_HPP_

#include "query/QueryBase.hpp"
#include "store/HandleTagData.hpp"
#include "store/HandleSession.hpp"
#include "store/HandleGeneric.hpp"
#include "store/HandleStringData.hpp"

#include "store/ServiceDefine.hh" // will be referred from query

namespace zpds {
namespace store {

class ItemDataService : virtual public HandleTagData, virtual public HandleSession,
	virtual public HandleGeneric, virtual public HandleStringData {
public:
	using pointer = std::shared_ptr<ItemDataService>;

	/**
	* ManageDataAction : sets item data common for all children
	*
	* @param stptr
	*   ::zpds::utils::SharedTable::pointer stptr
	*
	* @param resp
	*   ::zpds::query::ItemDataRespT* resp
	*
	* @return
	*   none throws if not ok
	*/
	void ManageDataAction(::zpds::utils::SharedTable::pointer stptr, ::zpds::query::ItemDataRespT* resp);

	/**
	* ReadDataAction : read item data common for all children
	*
	* @param stptr
	*   ::zpds::utils::SharedTable::pointer stptr
	*
	* @param resp
	*   ::zpds::query::ItemDataRespT* resp
	*
	* @return
	*   none throws if not ok
	*/
	void ReadDataAction(::zpds::utils::SharedTable::pointer stptr, ::zpds::query::ItemDataRespT* resp);

	/**
	* Get : gets the item data
	*
	* @param stptr
	*   ::zpds::utils::SharedTable::pointer stptr
	*
	* @param data
	*   ::zpds::query::ItemDataT* data
	*
	* @return
	*   none throws if not ok
	*/
	virtual void Get(::zpds::utils::SharedTable::pointer stptr, ::zpds::store::ItemDataT* data) const;

	/**
	* Pack : shrink to bare essentials for packing into cache
	*
	* @param data
	*   ::zpds::query::ItemDataT* data
	*
	* @return
	*   std::string packed string
	*/
	virtual std::string Pack(::zpds::store::ItemDataT* data) const;


	/**
	* GetKeyType : handles keytype
	*
	* @return
	*   KeyTypeE keytype primary key
	*/
	virtual KeyTypeE GetKeyType() const=0;

	/**
	* GetOneRecord : get one record
	*
	* @param stptr
	*   ::zpds::utils::SharedTable::pointer stptr
	*
	* @param data
	*   ItemDataT* item record
	*
	* @return
	*   bool true if exists
	*/
	virtual bool GetOneRecord(::zpds::utils::SharedTable::pointer stptr, ItemDataT* data) const=0;

	/**
	* GetManyRecords : get many record
	*
	* @param stptr
	*   ::zpds::utils::SharedTable::pointer stptr
	*
	* @param data
	*   ::google::protobuf::RepeatedPtrField< ::zpds::store::ItemDataT >* data to get
	*
	* @return
	*   none
	*/
	virtual void GetManyRecords(::zpds::utils::SharedTable::pointer stptr,
	                            ::google::protobuf::RepeatedPtrField< ::zpds::store::ItemDataT >* data) const=0;


	/**
	* AddOneRecord : writes one entry
	*
	* @param stptr
	*   ::zpds::utils::SharedTable::pointer stptr
	*
	* @param data
	*   ::zpds::store::ItemDataT* data
	*
	* @param trans
	*   zpds::store::TransactionT* trans
	*
	* @param exists
	*   bool if exists
	*
	* @return
	*   bool status if ok
	*/
	virtual bool AddOneRecord(::zpds::utils::SharedTable::pointer stptr,
	                          ::zpds::store::ItemDataT* data, zpds::store::TransactionT* trans, bool exists)=0;


protected:

	/**
	* GetUniqueByPrimary : get the unique key for a keytype
	*
	* @param keytype
	*   KeyTypeE primary key
	*
	* @return
	*   KeyTypeE keytype secondary unique key
	*/
	KeyTypeE GetUniqueByPrimary(KeyTypeE keytype) const;


	/**
	* HandleRelation : handle child relation between videos
	*
	* @param stptr
	*   ::zpds::utils::SharedTable::pointer stptr
	*
	* @param keytype
	*   KeyTypeE primary key of this type
	*
	* @param idata
	*   ::google::protobuf::RepeatedPtrField< ::zpds::store::ItemDataT >* input video data
	*
	* @param odata
	*   ::google::protobuf::RepeatedPtrField< ::zpds::store::ItemDataT >* output video data
	*
	* @param merge
	*   bool merge flag
	*
	* @return
	*   bool if changed
	*/
	bool HandleRelation(::zpds::utils::SharedTable::pointer stptr, KeyTypeE keytype,
	                    ::google::protobuf::RepeatedPtrField< ::zpds::store::ItemDataT >* idata,
	                    ::google::protobuf::RepeatedPtrField< ::zpds::store::ItemDataT >* odata,
	                    bool merge);

	/**
	* HandleContacts: handle contacts
	*
	* @param idata
	*   ::google::protobuf::RepeatedPtrField< ::zpds::store::VerifiedT >* input verified data
	*
	* @param odata
	*   ::google::protobuf::RepeatedPtrField< ::zpds::store::VerifiedT >* output verified data
	*
	* @param merge
	*   bool merge flag
	*
	* @return
	*   bool true if changed
	*/
	bool HandleContacts(
	    ::google::protobuf::RepeatedPtrField< ::zpds::store::VerifiedT >* idata,
	    ::google::protobuf::RepeatedPtrField< ::zpds::store::VerifiedT >* odata,
	    bool merge);

	/**
	* HandleAnother: handle parent
	*
	* @param stptr
	*   ::zpds::utils::SharedTable::pointer stptr
	*
	* @param idata
	*   ::zpds::store::ItemDataT * input parent data
	*
	* @param odata
	*   ::zpds::store::ItemDataT * output parent data
	*
	* @param merge
	*   bool merge flag
	*
	* @return
	*   bool true if changed
	*/
	bool HandleAnother(::zpds::utils::SharedTable::pointer stptr, ItemDataT* idata, ItemDataT* odata, bool merge);

	/**
	* GetBlindByPrimaryKey: get an arbitrary record by a primary key
	*
	* @param stptr
	*   ::zpds::utils::SharedTable::pointer stptr
	*
	* @param record
	*   ItemDataT* record
	*
	* @param keytype
	*   KeyTypeE key type for record proper primary key
	*
	* @return
	*   bool if found
	*/
	bool GetBlindByPrimaryKey(
	    ::zpds::utils::SharedTable::pointer stptr,
	    ItemDataT* record, KeyTypeE keytype);

	/**
	* GetBlindBySecondaryKey: get an arbitrary record by a secondary key
	*
	* @param stptr
	*   ::zpds::utils::SharedTable::pointer stptr
	*
	* @param record
	*   ItemDataT* record
	*
	* @param keytype
	*   KeyTypeE key type for record proper primary key
	*
	* @param idonly
	*   bool gets id only if set
	*
	* @return
	*   bool if found
	*/
	bool GetBlindBySecondaryKey(
	    ::zpds::utils::SharedTable::pointer stptr,
	    ItemDataT* record, KeyTypeE keytype, bool idonly=false);

	/**
	* PrepareData : prepare a single videofile for input, common for children
	*
	* @param stptr
	*   ::zpds::utils::SharedTable::pointer stptr
	*
	* @param updater
	*   ::zpds::store::ExterDataT* updater
	*
	* @param data
	*   ::zpds::store::ItemDataT* data
	*
	* @param action
	*   int32_t action use ZPDS_UACTION_* with logical
	*
	* @param fields
	*   int32_t fields fields to modify use ZPDS_VDFLD_* with logical
	*
	* @param merge
	*   bool merge data if found
	*
	* @return
	*   returns if found
	*/
	bool PrepareData(::zpds::utils::SharedTable::pointer stptr,
	                 ::zpds::store::ExterDataT* updater,
	                 ::zpds::store::ItemDataT* data,
	                 int32_t action, int32_t fields, bool merge);

	/**
	* ReadData : reads a single videofile for extraction, common for children
	*
	* @param stptr
	*   ::zpds::utils::SharedTable::pointer stptr
	*
	* @param reader
	*   ::zpds::store::UserDataT* reader
	*
	* @param data
	*   ::zpds::store::ItemDataT* data
	*
	* @param fields
	*   int32_t fields fields use ZPDS_VDFLD_* with logical
	*
	* @return
	*   none
	*/
	void ReadData(::zpds::utils::SharedTable::pointer stptr,
	              ::zpds::store::UserDataT* reader,
	              ::zpds::store::ItemDataT* data, int32_t fields);

};

} // namespace store
} // namespace zpds
#endif /* _ZPDS_STORE_ITEMDATA_SERVICE_HPP_ */
