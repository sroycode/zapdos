/**
 * @project zapdos
 * @file include/store/LookupRecordService.hpp
 * @author  S Roychowdhury < sroycode at gmail dot com >
 * @version 1.0.0
 *
 * @section LICENSE
 *
 * Copyright (c) 2017 S Roychowdhury.
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
 *  LookupRecordService.hpp : Header for es record data service
 *
 */
#ifndef _ZPDS_STORE_LOOKUP_RECORD_SERVICE_HPP_
#define _ZPDS_STORE_LOOKUP_RECORD_SERVICE_HPP_

#include "query/QueryBase.hpp"
#include "store/StoreBase.hpp"
#include "store/StoreTrans.hpp"
#include "remote/GeoHashHelper.hpp"

namespace zpds {
namespace store {

class LookupRecordService : public StoreBase  {
public:

	/**
	* AddDataAction : unified data addition
	*
	* @param stptr
	*   ::zpds::utils::SharedTable::pointer stptr
	*
	* @param esparams
	*   ::zpds::query::LookupParamsT* esparams
	*
	* @return
	*   none throws if not ok
	*/
	void AddDataAction(::zpds::utils::SharedTable::pointer stptr, ::zpds::query::LookupParamsT* esparams);

	/**
	* GetDataAction : get data
	*
	* @param stptr
	*   ::zpds::utils::SharedTable::pointer stptr
	*
	* @param esparams
	*   ::zpds::query::LookupParamsT* esparams
	*
	*
	* @return
	*   none throws if not ok
	*/
	void GetDataAction(::zpds::utils::SharedTable::pointer stptr, ::zpds::query::LookupParamsT* esparams);

	/**
	* GetIndexDataAction : get data for ES Index
	*
	* @param stptr
	*   ::zpds::utils::SharedTable::pointer stptr
	*
	* @param esparams
	*   ::zpds::query::LookupParamsT* esparams
	*
	* @return
	*   none throws if not ok
	*/
	void GetIndexDataAction(::zpds::utils::SharedTable::pointer stptr, ::zpds::query::LookupParamsT* esparams);

private:
	::zpds::remote::GeoHashHelper gh;

	/**
	* UpsertLookupRecord : LookupRecord data addition
	*
	* @param stptr
	*   ::zpds::utils::SharedTable::pointer stptr
	*
	* @param data
	*   ::zpds::store::LookupRecordT* data type LookupRecord
	*
	* @param trans
	*   ::zpds::store::TransactionT* trans
	*
	* @return
	*   bool
	*/
	bool UpsertLookupRecord(::zpds::utils::SharedTable::pointer stptr, LookupRecordT* data, TransactionT* trans);

	/**
	* MergeLookupRecord : LookupRecord data merging
	*
	* @param stptr
	*   ::zpds::utils::SharedTable::pointer stptr
	*
	* @param data
	*   ::zpds::store::LookupRecordT* data type LookupRecord
	*
	* @param trans
	*   ::zpds::store::TransactionT* trans
	*
	* @return
	*   bool
	*/
	bool MergeLookupRecord(::zpds::utils::SharedTable::pointer stptr, LookupRecordT* data, TransactionT* trans);

	/**
	* MergeOne : LookupRecord data merging for one record
	*
	* @param mto
	*   ::zpds::store::LookupRecordT* data type LookupRecord merged to
	*
	* @param mfrom
	*   ::zpds::store::LookupRecordT* data type LookupRecord merged from
	*
	* @return
	*   true if changed
	*/
	bool MergeOne(LookupRecordT* mto, LookupRecordT* mfrom);


};
} // namespace store
} // namespace zpds
#endif /* _ZPDS_STORE_LOOKUP_RECORD_SERVICE_HPP_ */
