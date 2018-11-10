/**
 * @project zapdos
 * @file include/store/SimpleTemplateTable.hpp
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
 *  SimpleTemplateTable.hpp :   Headers for SimpleTemplateTable  simpletemplate
 *
 */
#ifndef _ZPDS_STORE_SIMPLE_TEMPLATE_TABLE_HPP_
#define _ZPDS_STORE_SIMPLE_TEMPLATE_TABLE_HPP_

#include "StoreTable.hpp"

namespace zpds {
namespace store {
class SimpleTemplateTable : public StoreTable<SimpleTemplateT> {
public:
	using StoreTable<SimpleTemplateT>::dbpointer;
	using StoreTable<SimpleTemplateT>::StoreTable;
	using StoreTable<SimpleTemplateT>::MapT;

	/**
	* Constructor
	*
	* @param trydb
	*   dbpointer shared pointer to db
	*
	*/
	SimpleTemplateTable(dbpointer trydb);

	/**
	* Destructor
	*
	*/
	virtual ~SimpleTemplateTable() {}

	/**
	* make noncopyable and remove default
	*/

	SimpleTemplateTable() = delete;
	SimpleTemplateTable(const std::string, const size_t) = delete;
	SimpleTemplateTable(const SimpleTemplateTable&) = delete;
	SimpleTemplateTable& operator=(const SimpleTemplateTable&) = delete;


protected:

	/**
	* GetKey: get a key
	*
	* @param record
	*   SimpleTemplateT* record
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
	std::string GetKey(SimpleTemplateT* record, KeyTypeE keytype, bool pre) override;

};
} // namespace store
} // namespace zpds
#endif /* _ZPDS_STORE_SIMPLE_TEMPLATE_TABLE_HPP_ */
