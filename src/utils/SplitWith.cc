/**
 * @project zapdos
 * @file src/utils/SplitWith.cc
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
 *  SplitWith.cc : Split String impl
 *
 */
#include <locale>
#include <sstream>
#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/regex.hpp>

#include "utils/SplitWith.hpp"

/**
* LowerNoQuote: split and sanitize , remove non search chars
*
*/
std::vector<std::string> zpds::utils::SplitWith::LowerNoQuote(const std::string& input)
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

/**
* Space : split by space characters
*
*/
std::vector<std::string> zpds::utils::SplitWith::Space(const std::string& input)
{
	std::vector<std::string> output;
	boost::algorithm::split(output,input,boost::algorithm::is_space());
	return output;
}

/**
* Regex : split by regex characters
*
*/
std::vector<std::string> zpds::utils::SplitWith::Regex(const std::string& input, const boost::regex reg, bool trim)
{
	std::vector<std::string> output;
	boost::algorithm::split_regex(output,input,reg);
	if (trim) std::for_each(output.begin(), output.end(), boost::bind(&boost::trim<std::string>, _1, std::locale() ));
	return output;
}
