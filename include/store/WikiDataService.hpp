/**
 * @project zapdos
 * @file include/store/WikiDataService.hpp
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
 *  WikiDataService.hpp : WikiData manipulation Headers
 *
 */
#ifndef _ZPDS_STORE_WIKIDATA_SERVICE_HPP_
#define _ZPDS_STORE_WIKIDATA_SERVICE_HPP_

#include "store/ItemDataService.hpp"

namespace zpds {
namespace store {

class WikiDataService : virtual public ItemDataService {
public:
	using basepointer = std::shared_ptr<ItemDataService>; // this pointer is the base class

	/**
	* CreateBase : create an instance of WikiData Service with base class ptr
	*
	*/
	static basepointer CreateBase()
	{
		return std::make_shared<WikiDataService>();
	}

	/**
	* Constructor : default for WikiData Service
	*
	*/
	WikiDataService();

	/**
	* Destructor : for WikiData Service
	*
	*/
	~WikiDataService();

	/**
	* GetOneRecord : reads one entry for WikiData
	*
	* @param stptr
	*   ::zpds::utils::SharedTable::pointer stptr
	*
	* @param data
	*   ::zpds::store::ItemDataT* data
	*
	* @return
	*   bool status if found
	*/
	bool GetOneRecord(::zpds::utils::SharedTable::pointer stptr, ::zpds::store::ItemDataT* data) const override;

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
	void GetManyRecords(
	    ::zpds::utils::SharedTable::pointer stptr,
	    ::google::protobuf::RepeatedPtrField< ::zpds::store::ItemDataT >* data) const override;


	/**
	* AddOneRecord : writes one entry for WikiData
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
	bool AddOneRecord(
	    ::zpds::utils::SharedTable::pointer stptr,
	    ::zpds::store::ItemDataT* data,
	    zpds::store::TransactionT* trans,
	    bool exists) override;

	/**
	* GetKeyType : handles keytype for WikiData
	*
	* @return
	*   KeyTypeE keytype primary key
	*/
	KeyTypeE GetKeyType() const override;

protected:

	/**
	* AllowedFieldsFilter : sets update variable for WikiData
	*
	* @param update
	*   int32_t input update
	*
	* @return
	*   int32_t value
	*/
	int32_t AllowedFieldsFilter(int32_t update) override;

};
} // namespace store
} // namespace zpds
#endif /* _ZPDS_STORE_WIKIDATA_SERVICE_HPP_ */
