/**
 * @project zapdos
 * @file include/utils/SplitWith.hpp
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
 *  SplitWith.hpp : Split String Utilities Headers
 *
 */
#ifndef _ZPDS_UTILS_SPLIT_WITH_HPP_
#define _ZPDS_UTILS_SPLIT_WITH_HPP_

#include <string>
#include <boost/regex.hpp>
#include <vector>

namespace zpds {
namespace utils {

class SplitWith {
public:
	/**
	* LowerNoQuote: split and sanitize , remove non search chars
	*
	* @param input
	*   const std::string& input string to sanit
	*
	* @return
	*   std::vector<std::string>
	*/
	static std::vector<std::string> LowerNoQuote(const std::string& input);

	/**
	* Space : split by space characters
	*
	* @param input
	*   const std::string& input string to split
	*
	* @return
	*   std::vector<std::string>
	*/
	static std::vector<std::string> Space(const std::string& input);

	/**
	* Regex : split by regex characters
	*
	* @param input
	*   const std::string& input string to split
	*
	* @return
	*   std::vector<std::string>
	*/
	static std::vector<std::string> Regex(const std::string& input, const boost::regex reg, bool trim=false);

};

} // namespace utils
} // namespace zpds

#endif // _ZPDS_UTILS_SPLIT_WITH_HPP_
