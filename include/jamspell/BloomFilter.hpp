/**
 * @project zapdos
 * @file include/jamspell/BloomFilter.hpp
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
 *  BloomFilter.hpp : Jamspell Bloom filter Headers for jamspell
 *
 */
#ifndef _ZPDS_JAMSPELL_BLOOM_FILTER_HPP_
#define _ZPDS_JAMSPELL_BLOOM_FILTER_HPP_

#include <memory>
#include <string>
#include "utils/BaseUtils.hpp"

namespace zpds {
namespace jamspell {

class TBloomFilter {
public:
    TBloomFilter();
    TBloomFilter(uint64_t elements, double falsePositiveRate);
    ~TBloomFilter();
    void Insert(const std::string& element);
    bool Contains(const std::string& element) const;
    void Dump(std::ostream& out) const;
    void Load(std::istream& in);
private:
    struct Impl;
    std::unique_ptr<Impl> BloomFilter;
};

} // jamspell
} // zpds
#endif  // _ZPDS_JAMSPELL_BLOOM_FILTER_HPP_
