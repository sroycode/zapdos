/**
 * @project zapdos
 * @file include/utils/StringHelpers.hpp
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
 *  StringHelpers.hpp :   For Standard Inclusion
 *
 */
#ifndef _ZPDS_UTILS_STRING_HELPERS_HPP_
#define _ZPDS_UTILS_STRING_HELPERS_HPP_

#include <cstring>
#include <string>
#include <algorithm>
#include <sstream>
#include <boost/algorithm/string.hpp>
#include <functional>
#include <iomanip>
#include <map>
#include <cmath>
#include <utility>

template <typename T> static void I_PrintComma(std::ostream& o, T t)
{
	o << " , " << t;
}

template<> void I_PrintComma(std::ostream& o, std::string&& t)
{
	if (t.length()) o << " , " << t;
}

template<> void I_PrintComma(std::ostream& o, const char* t)
{
	if (strlen(t)) o << " , " << t;
}

template<typename T, typename... Args> static void I_PrintComma(std::ostream& o, T t, Args... args) // recursive variadic function
{
	I_PrintComma(o, t);
	I_PrintComma(o, args...);
}

template<typename... Args> static std::string PrintWithComma(Args... args)
{
	std::ostringstream oss;
	I_PrintComma(oss, args...);
	return (oss.str().length()>3) ? oss.str().substr(3) : std::string();
}

static std::string NoSpace(const std::string& orig)
{
	if (orig.empty()) return std::string();
	std::ostringstream xtmp;
	std::locale loc;
	for (auto i = orig.cbegin(), n = orig.cend(); i != n; ++i) {
		auto c = (*i);
		// if alnum lowercase into string
		if (std::isalnum(c)) xtmp << std::tolower(c,loc);
	}
	return xtmp.str();
}

static std::string LcTrim(const std::string& orig)
{
	std::string s = orig;
	boost::algorithm::to_lower(s);
	boost::algorithm::trim(s);
	return s;
}

/**
* Flatten : string to lowrcase nospace word
*
* @param input
*   std::string& input
*
* @return
*   std::string to be initialized
*/
static std::string Flatten(const std::string& input)
{
	std::ostringstream xtmp;
	std::locale loc;
	bool last_is_char=false;
	for (auto i = input.cbegin(), n = input.cend(); i != n; ++i) {
		auto c = (*i);
		// if alnum lowercase into string
		if (std::isalnum(c)) {
			xtmp << std::tolower(c,loc);
			last_is_char=true;
		}
		else if (c=='\'' || c=='\"') {
			// skip
		}
		else if ((std::isspace(c) || std::ispunct(c)) && last_is_char) {
			xtmp << ' ';
			last_is_char=false;
		}
	}
	// parsing end
	return xtmp.str();
}

/**
* FlattenCount : string to lowrcase nospace word
*
* @param input
*   std::string& input
*
* @return
*   std::pair<std::string,size_t> string to be initialized , count
*/
static std::pair<std::string,size_t> FlattenCount(const std::string& input)
{
	size_t wc=0;
	std::ostringstream xtmp;
	std::locale loc;
	bool last_is_char=false;
	for (auto i = input.cbegin(), n = input.cend(); i != n; ++i) {
		auto c = (*i);
		// if alnum lowercase into string
		if (std::isalnum(c)) {
			xtmp << std::tolower(c,loc);
			last_is_char=true;
		}
		else if (c=='\'' || c=='\"') {
			// skip
		}
		else if ((std::isspace(c) || std::ispunct(c)) && last_is_char) {
			++wc;
			xtmp << ' ';
			last_is_char=false;
		}
	}
	if (last_is_char) ++wc;
	// parsing end
	return std::make_pair( xtmp.str() , wc );
}

/**
* FormatPrefix :  to lowercase nospace word
*
* @param input
*   std::string& input
*
* @param prefix
*   std::string prefix
*
* @return
*   std::string to be initialized
*/
static std::string FormatPrefix(const std::string& input, const std::string& prefix)
{
	return prefix + NoSpace(input);
}

/**
* KeepInBound : keep the number in bound
*
*  @param val
*     int64_t value
*
*  @param min
*     int64_t value min
*
*  @param max
*     int64_t value max
*
*  @return
*   int64_t
*/
template <typename T>
inline T KeepInBound(T val, T min, T max)
{
	return (val > min) ? ( (val<max) ? val : max ) : min;
}

/**
* SplitBySpace : just split by space
*
* @param input
*   std::string& input
*
* @return
*   std::vector<std::string> vector
*/
static std::vector<std::string> SplitBySpace(const std::string& input)
{
	std::vector<std::string> arr;
	std::ostringstream xtmp;
	for (auto i = input.cbegin(), n = input.cend(); i != n; ++i) {
		auto c = (*i);
		if (std::isspace(c)) {
			if (xtmp.str().length()>0) arr.push_back(xtmp.str());
			xtmp.str(std::string());
		} else {
			xtmp << c;
		}
	}
	if (xtmp.str().length()>0) arr.push_back(xtmp.str());
	return arr;
}

/**
* Split : just split
*
* @param input
*   std::string& input
*
* @return
*   std::vector<std::string> vector
*/
static std::vector<std::string> Split(const std::string& input)
{
	std::vector<std::string> output;
	std::ostringstream xtmp;
	std::locale loc;
	size_t wc=0;
	bool last_is_char=false;
	for (auto i = input.cbegin(), n = input.cend(); i != n; ++i) {
		auto c = (*i);
		// if alnum lowercase into string
		if (std::isalnum(c)) {
			xtmp << std::tolower(c,loc);
			last_is_char=true;
		}
		else if (c=='\'' || c=='\"') {
			// skip
		}
		else if ((std::isspace(c) || std::ispunct(c)) && last_is_char) {
			output.push_back(xtmp.str());
			xtmp.str(std::string());
			last_is_char=false;
			++wc;
		}
	}
	if (xtmp.str().length()>0) {
		output.push_back(xtmp.str());
	}
	return output;
}

#endif // _ZPDS_UTILS_STRING_HELPERS_HPP_
