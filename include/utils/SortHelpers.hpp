/**
 * @project zapdos
 * @file include/utils/SortHelpers.hpp
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
 *  SortHelpers.hpp : Sort helpers for Standard Inclusion Headers
 *
 */
#ifndef _ZPDS_UTILS_SORT_HELPERS_HPP_
#define _ZPDS_UTILS_SORT_HELPERS_HPP_

#include <cstring>
#include <string>
#include <algorithm>
#include <sstream>
#include <boost/algorithm/string.hpp>
#include <functional>
#include <iomanip>
#include <map>
#include <cmath>
#include "utils/BaseUtils.hpp"

#define DR_BUFF_INT 10000
#define DR_BUFF_MAX std::numeric_limits<int32_t>::max()
#define DR_BUFF_MIN std::numeric_limits<int32_t>::min()

#define ZPDS_UINT64_MAX std::numeric_limits<uint64_t>::max()

/** Sort Helper functions start */

/**
 * MakeSortKey : make sort key for single item default
 *
 */
template <typename T>
static void MakeSortKey(std::ostream& o,  T t)
{
	throw zpds::BadCodeException("This type cannot be used in key");
}

/**
 * MakeSortKey : make sort key for single item uint64_t specialization
 *
 */
template<> void MakeSortKey(std::ostream& o, uint64_t t)
{
	t = ( t > DR_BUFF_MAX ) ? DR_BUFF_MAX : t;
	o << static_cast<uint64_t>( (t * DR_BUFF_INT) + std::numeric_limits<int64_t>::max() );
}

/**
 * MakeSortKey : make sort key for single item int64_t
 *
 */
template<> void MakeSortKey(std::ostream& o, int64_t t)
{
	t = ( t > DR_BUFF_MAX ) ? DR_BUFF_MAX : ( t < DR_BUFF_MIN ) ? DR_BUFF_MIN : t;
	o << static_cast<uint64_t>( ( (int64_t)t * DR_BUFF_INT) + ( (t>0) ? std::numeric_limits<int64_t>::max() : 0 ));
}

/**
 * MakeSortKey : make sort key for uint32_t specialization
 *
 */
template<> void MakeSortKey(std::ostream& o, uint32_t t)
{
	o << static_cast<uint64_t>( ( (int64_t)t * DR_BUFF_INT) + std::numeric_limits<int64_t>::max() );
}

/**
 * MakeSortKey : make sort key for int32_t specialization
 *
 */
template<> void MakeSortKey(std::ostream& o, int32_t t)
{
	o << static_cast<uint64_t>( ( (int64_t)t * DR_BUFF_INT) + ((t>0) ? std::numeric_limits<int64_t>::max() : 0 ));
}

/**
 * MakeSortKey : make sort key for single item double
 *
 */
template<> void MakeSortKey(std::ostream& o, double t)
{
	o << static_cast<uint64_t>( std::llround((long double)t * DR_BUFF_INT) + ((t>0) ? std::numeric_limits<int64_t>::max() : 0 ));
}

/**
 * MakeSortKey : make sort key for single item char
 *
 */
template<> void MakeSortKey(std::ostream& o, char t)
{
	o << t;
}

/**
 * MakeSortKey : variadic recursive for multiple sort
 *
 */
template<typename T, typename... Args>
static void MakeSortKey(std::ostream& o, T t, Args... args)
{
	MakeSortKey(o, t);
	MakeSortKey(o, args...);
}

/**
 * EncodeSortKey : encode key
 *
 */
template<typename... T>
static std::string EncodeSortKey (T... key)
{
	std::ostringstream ss;
	ss << std::setbase(16) << std::setfill ('0') << std::setw (16);
	MakeSortKey(ss, key...);
	return ss.str();
}

/** Sort Helper functions end */

#endif // _ZPDS_UTILS_SORT_HELPERS_HPP_
