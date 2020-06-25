/**
 * @project zapdos
 * @file include/utils/PrintWith.hpp
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
 *  PrintWith.hpp : Class to print several other DS with separator Header Headers
 *
 */
#ifndef _ZPDS_UTILS_PRINT_WITH_HPP_
#define _ZPDS_UTILS_PRINT_WITH_HPP_

#include <string>
#include <sstream>
#include <boost/algorithm/string.hpp>
#include <iomanip>
#include <vector>
#include <set>
#include <map>

namespace zpds {
namespace utils {

template<const char C, const char D >
class PrintWith {

	// catch-all default
	template <typename T> static void _PrintPipe(std::ostream& o, T t)
	{
		if(D==0) o << C << t;
		else o << C << D << t;
	}

	// vector is overloaded
	template<typename T> static void _PrintPipe(std::ostream& o, std::vector<T> t)
	{
		for (auto& i : t ) {
			_PrintPipe(o, i);
		}
	}

	// map is overloaded
	template<typename T, typename TT> static void _PrintPipe(std::ostream& o, std::map<T,TT> t)
	{
		for (auto& i : t ) {
			_PrintPipe(o, i.first) ;
			_PrintPipe(o, i.second);
		}
	}

	// set is overloaded
	template<typename T> static void _PrintPipe(std::ostream& o, std::set<T> t)
	{
		for (auto& i : t ) {
			_PrintPipe(o, i);
		}
	}


	// recursive control function
	template<typename T, typename... Args> static void _PrintPipe(std::ostream& o, T t, Args... args)
	{
		_PrintPipe(o, t);
		_PrintPipe(o, args...);
	}

public:
	/**
	* String : print concatenated string C separated
	*
	*/
	template<typename... Args> static std::string String(Args... args)
	{
		std::ostringstream oss;
		_PrintPipe(oss, args...);
		auto&& E=1+(D>0);
		return ( oss.str().length()>E ) ? oss.str().substr(E) : std::string();
	}

	/**
	* Full : print full concatenated string C separated includind leading separator
	*
	*/
	template<typename... Args> static std::string Full(Args... args)
	{
		std::ostringstream oss;
		_PrintPipe(oss, args...);
		return oss.str();
	}
};

// PrintWith shortcuts
using PrintWithPipe  = PrintWith<'|',0>;
using PrintWithDot  = PrintWith<'.',0>;
using PrintWithSlash = PrintWith<'/',0>;
using PrintWithSpace = PrintWith<' ',0>;
using PrintWithHyphen= PrintWith<'-',0>;
using PrintWithComma= PrintWith<',',0>;
using PrintWithCrumb = PrintWith<'>','>'>;

} // namespace utils
} // namespace zpds

#endif // _ZPDS_UTILS_PRINT_WITH_HPP_
