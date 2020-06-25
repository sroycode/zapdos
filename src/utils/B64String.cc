/**
 * @project zapdos
 * @file src/utils/B64String.cc
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
 *  B64String.cc : Base 64 String handlers impl See https://stackoverflow.com/questions/10521581/base64-encode-using-boost-throw-exception
 *
 */
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/archive/iterators/insert_linebreaks.hpp>
#include <boost/archive/iterators/remove_whitespace.hpp>
#include <iostream>

#include "utils/B64String.hpp"

/**
 * Encode : base64 encode from string
 *
 */
std::string zpds::utils::B64String::Encode(const std::string& input)
{
	using namespace boost::archive::iterators;
	// typedef transform_width< binary_from_base64<remove_whitespace<std::string::const_iterator> >, 8, 6 > it_binary_t;
	typedef insert_linebreaks<base64_from_binary<transform_width<std::string::const_iterator,6,8> >, 72 > it_base64_t;
	unsigned int writePaddChars = (3-input.length()%3)%3;
	std::string base64(it_base64_t(input.begin()),it_base64_t(input.end()));
	base64.append(writePaddChars,'=');
	return base64;
}

/**
 * Decode : base64 decode from string
 *
 */
std::string zpds::utils::B64String::Decode(std::string input)
{
	using namespace boost::archive::iterators;
	typedef transform_width< binary_from_base64<remove_whitespace<std::string::const_iterator> >, 8, 6 > it_binary_t;
	// typedef insert_linebreaks<base64_from_binary<transform_width<std::string::const_iterator,6,8> >, 72 > it_base64_t;
	unsigned int paddChars = count(input.begin(), input.end(), '=');
	std::replace(input.begin(),input.end(),'=','A'); // replace '=' by base64 encoding of '\0'
	std::string result(it_binary_t(input.begin()), it_binary_t(input.end())); // decode
	result.erase(result.end()-paddChars,result.end());  // erase padding '\0' characters
	return result;
}
