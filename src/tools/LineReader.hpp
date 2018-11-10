/**
 * @project zapdos
 * @file src/tools/LineReader.hpp
 * @author  S Roychowdhury
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
 *  LineReader.hpp : read file data
 *
 */

#ifndef _ZPDS_TOOLS_LINEREADER_HPP_
#define _ZPDS_TOOLS_LINEREADER_HPP_

#include <unordered_set>
#include "utils/BaseUtils.hpp"
#include "../proto/Query.pb.h"

namespace zpds {
namespace tools {
class LineReader {
public:
	using StrVecT=std::vector<std::string>;

	/**
	* Constructor : default
	*
	*/
	LineReader();

	/**
	* make noncopyable
	*/

	LineReader(const LineReader&) = delete;
	LineReader& operator=(const LineReader&) = delete;

	/**
	* destructor
	*/
	virtual ~LineReader ();


	/**
	* ReadToLookup: load to LookupDataT
	*
	* @param strvec
	*   StrVecT input vector
	*
	* @param data
	*   ::zpds::query::LookupDataT* data
	*
	* @param qtyp
	*   int qtyp
	*
	* @param counter
	*   size_t stating counter
	*
	* @return
	*   none
	*/
	void ReadToLookup(StrVecT strvec, ::zpds::query::LookupDataT* data, int qtyp, size_t counter);

private:
	std::unordered_set<std::string> whatset;

	/**
	* ReadLookupRecord: read line vec to LookupRecordT
	*
	* @param strvec
	*   StrVecT& input vector
	*
	* @param data
	*   ::zpds::store::LookupRecordT* record
	*
	* @return
	*   none
	*/
	void ReadLookupRecord(StrVecT& strvec, ::zpds::store::LookupRecordT* record);
};
} // namespace tools
} // namespace zpds

#endif // _ZPDS_TOOLS_LINEREADER_HPP_
