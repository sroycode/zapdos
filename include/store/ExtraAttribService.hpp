/**
 * @project zapdos
 * @file include/store/ExtraAttribService.hpp
 * @author  S Roychowdhury < sroycode at gmail dot com >
 * @version 1.0.0
 *
 * @section LICENSE
 *
 * Copyright (c) 2018-2019 S Roychowdhury.
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
 *  ExtraAttribService.hpp : Header for es record data service
 *
 */
#ifndef _ZPDS_STORE_EXTRA_ATTRIB_SERVICE_HPP_
#define _ZPDS_STORE_EXTRA_ATTRIB_SERVICE_HPP_

#include "query/QueryBase.hpp"
#include "store/StoreBase.hpp"
#include "store/StoreTrans.hpp"
#include "remote/GeoHashHelper.hpp"

namespace zpds {
namespace query {
using ExtraParamsT = LookupParamsT;
using ExtraDataT = LookupDataT;
using ExtraRespT = LookupRespT;
}
}

namespace zpds {
namespace store {

using ExtraAttribT = LookupRecordT;

class ExtraAttribService : public StoreBase  {
public:

	/**
	* AddDataAction : unified data addition
	*
	* @param stptr
	*   ::zpds::utils::SharedTable::pointer stptr
	*
	* @param esparams
	*   ::zpds::query::ExtraParamsT* esparams
	*
	* @return
	*   none throws if not ok
	*/
	void AddDataAction(::zpds::utils::SharedTable::pointer stptr, ::zpds::query::ExtraParamsT* esparams);

	/**
	* GetDataAction : get data
	*
	* @param stptr
	*   ::zpds::utils::SharedTable::pointer stptr
	*
	* @param esparams
	*   ::zpds::query::ExtraParamsT* esparams
	*
	*
	* @return
	*   none throws if not ok
	*/
	void GetDataAction(::zpds::utils::SharedTable::pointer stptr, ::zpds::query::ExtraParamsT* esparams);

private:
	::zpds::remote::GeoHashHelper gh;

	/**
	* UpsertExtraAttrib : ExtraAttrib data addition
	*
	* @param stptr
	*   ::zpds::utils::SharedTable::pointer stptr
	*
	* @param data
	*   ::zpds::store::ExtraAttribT* data type ExtraAttrib
	*
	* @param trans
	*   ::zpds::store::TransactionT* trans
	*
	* @return
	*   bool
	*/
	bool UpsertExtraAttrib(::zpds::utils::SharedTable::pointer stptr, ExtraAttribT* data, TransactionT* trans);

	/**
	* MergeExtraAttrib : ExtraAttrib data merging
	*
	* @param stptr
	*   ::zpds::utils::SharedTable::pointer stptr
	*
	* @param data
	*   ::zpds::store::ExtraAttribT* data type ExtraAttrib
	*
	* @param trans
	*   ::zpds::store::TransactionT* trans
	*
	* @return
	*   bool
	*/
	bool MergeExtraAttrib(::zpds::utils::SharedTable::pointer stptr, ExtraAttribT* data, TransactionT* trans);

	/**
	* MergeOne : ExtraAttrib data merging for one record
	*
	* @param mto
	*   ::zpds::store::ExtraAttribT* data type ExtraAttrib merged to
	*
	* @param mfrom
	*   ::zpds::store::ExtraAttribT* data type ExtraAttrib merged from
	*
	* @return
	*   true if changed
	*/
	bool MergeOne(ExtraAttribT* mto, ExtraAttribT* mfrom);


};
} // namespace store
} // namespace zpds
#endif /* _ZPDS_STORE_EXTRA_ATTRIB_SERVICE_HPP_ */
