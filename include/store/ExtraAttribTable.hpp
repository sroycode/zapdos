/**
 * @project zapdos
 * @file include/store/ExtraAttribTable.hpp
 * @author  S Roychowdhury < sroycode at gmail dot com>
 * @version 1.0.0
 *
 * @section LICENSE
 *
 * Copyright (c) 2018-2019 S Roychowdhury
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
 *  ExtraAttribTable.hpp :   Headers for ExtraAttribTable  extraattrib
 *
 */
#ifndef _ZPDS_STORE_EXTRA_ATTRIB_TABLE_HPP_
#define _ZPDS_STORE_EXTRA_ATTRIB_TABLE_HPP_

#include "StoreTable.hpp"

namespace zpds {
namespace store {
class ExtraAttribTable : public StoreTable<ExtraAttribT> {
public:
	using StoreTable<ExtraAttribT>::dbpointer;
	using StoreTable<ExtraAttribT>::StoreTable;
	using StoreTable<ExtraAttribT>::MapT;

	/**
	* Constructor
	*
	* @param trydb
	*   dbpointer shared pointer to db
	*
	*/
	ExtraAttribTable(dbpointer trydb);

	/**
	* Destructor
	*
	*/
	virtual ~ExtraAttribTable() {}

	/**
	* make noncopyable and remove default
	*/

	ExtraAttribTable() = delete;
	ExtraAttribTable(const std::string, const size_t) = delete;
	ExtraAttribTable(const ExtraAttribTable&) = delete;
	ExtraAttribTable& operator=(const ExtraAttribTable&) = delete;


protected:

	/**
	* GetKey: get a key
	*
	* @param record
	*   ExtraAttribT* record
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
	std::string GetKey(ExtraAttribT* record, KeyTypeE keytype, bool pre) override;

};
} // namespace store
} // namespace zpds
#endif /* _ZPDS_STORE_EXTRA_ATTRIB_TABLE_HPP_ */
