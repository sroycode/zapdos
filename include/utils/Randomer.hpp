/**
 * @project zapdos
 * @file include/utils/Randomer.hpp
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
 *  Randomer.hpp : See https://gist.github.com/VjGusev/e51429658c596672458137c57efb71e3 Headers
 *
 */
#ifndef _ZPDS_UTILS_RANDOMER_HPP_
#define _ZPDS_UTILS_RANDOMER_HPP_

#include <numeric>
#include <random>
#include <vector>
#include <iostream>
#include <iterator>

namespace zpds {
namespace utils {

class Randomer {
	// random seed by default
	std::mt19937 gen_;
	std::uniform_int_distribution<size_t> dist_;

public:
	Randomer() : Randomer(0, std::numeric_limits<size_t>::max(), std::random_device{}())
	{
	}

	// convenience ctor
	explicit Randomer(size_t max)
		: Randomer(0, max, std::random_device{}())
	{
	}

	Randomer(size_t min, size_t max, unsigned int seed = std::random_device{}())
		: gen_{seed}, dist_{min, max}
	{
	}

	// if you want predictable numbers
	void SetSeed(unsigned int seed)
	{
		gen_.seed(seed);
	}

	size_t operator()()
	{
		return dist_(gen_);
	}
};

const static std::vector<char> alnumchars{
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L',
	'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
	'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
	'k', 'l', 'm', 'n','o','p', 'q', 'r', 's', 't', 'u', 'v',
	'w', 'x', 'y', 'z'};
static Randomer randomer{alnumchars.size() - 1};

} // namespace utils
} // namespace zpds
#define ZPDS_RANDOM_STRING(AAA,BBB) AAA.clear();std::generate_n(std::back_inserter(AAA), BBB, []() { return ::zpds::utils::alnumchars[::zpds::utils::randomer()]; });
#endif // _ZPDS_UTILS_RANDOMER_HPP_
