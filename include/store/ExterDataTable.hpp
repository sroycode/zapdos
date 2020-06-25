/**
 * @project zapdos
 * @file include/store/ExterDataTable.hpp
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
 *  ExterDataTable.hpp : 
 *
 */
#ifndef _ZPDS_STORE_EXTER_DATA_TABLE_HPP_
#define _ZPDS_STORE_EXTER_DATA_TABLE_HPP_

#include "StoreTable.hpp"

namespace zpds {
namespace store {
using ExterDataT=UserDataT;
class ExterDataTable : public StoreTable<ExterDataT> {
public:
	using StoreTable<ExterDataT>::dbpointer;
	using StoreTable<ExterDataT>::StoreTable;

	/**
	* Constructor
	*
	* @param trydb
	*   dbpointer shared pointer to db
	*
	*/
	ExterDataTable(dbpointer trydb);

	/**
	* Destructor
	*
	*/
	virtual ~ExterDataTable() {}

	/**
	* make noncopyable and remove default
	*/

	ExterDataTable() = delete;
	ExterDataTable(const std::string, const size_t) = delete;
	ExterDataTable(const ExterDataTable&) = delete;
	ExterDataTable& operator=(const ExterDataTable&) = delete;


protected:

	/**
	* GetKey: get a key
	*
	* @param record
	*   ExterDataT* record
	*
	* @param keytype
	*   KeyTypeE key type for index
	*
	* @param pre
	*   bool non unique keys
	*
	* @return
	*   std::string key
	*/
	std::string GetKey(ExterDataT* record, KeyTypeE keytype, bool pre) override;

};
} // namespace store
} // namespace zpds
#endif /* _ZPDS_STORE_EXTER_DATA_TABLE_HPP_ */
