/**
 * @project zapdos
 * @file include/utils/S64String.hpp
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
 *  S64String.hpp : Snappy Compression Headers
 *
 */
#ifndef _ZPDS_UTILS_S64STRING_HPP_
#define _ZPDS_UTILS_S64STRING_HPP_

#include <string>

namespace zpds {
namespace utils {

struct S64String {

	/**
	 * Encode : compress from string
	 *
	 * @param input
	 *   const std::string& input
	 *
	 * @return
	 *   std::string
	 */
	static std::string Encode(const std::string& input);

	/**
	 * Decode : decompress from string
	 *
	 * @param input
	 *   const std::string& input
	 *
	 * @return
	 *   std::string
	 */
	static std::string Decode(std::string input);

};

} // namespace utils
} // namespace zpds
#endif // _ZPDS_UTILS_S64STRING_HPP_
