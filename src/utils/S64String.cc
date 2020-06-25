/**
 * @project zapdos
 * @file src/utils/S64String.cc
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
 *  S64String.cc : Snappy Compression Headers
 *
 */
#include <snappy.h>

#include "utils/S64String.hpp"
#include "utils/B64String.hpp"

/**
 * Encode : compress from string
 *
 */
std::string zpds::utils::S64String::Encode(const std::string& input)
{
	std::string output;
	::snappy::Compress(input.data(), input.size(), &output);
	return ::zpds::utils::B64String::Encode(output);
}

/**
 * Decode : snappy uncompress from string
 *
 */
std::string zpds::utils::S64String::Decode(std::string input)
{
	std::string output;
	std::string buffer = ::zpds::utils::B64String::Decode(input);
	::snappy::Uncompress(buffer.data(), buffer.size(), &output);
	return output;
}
