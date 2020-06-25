/**
 * @project zapdos
 * @file src/jamspell/BloomFilter.cc
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
 *  BloomFilter.cc : Bloom filter impl for Jamspell
 *
 */
#include <cassert>

#include "jamspell/bloom_filter.hpp"
#include "jamspell/BloomFilter.hpp"
#include "jamspell/HandyPack.hpp"

namespace zpds {
namespace jamspell {

struct TBloomFilter::Impl: public bloom_filter {
    Impl(): bloom_filter() {}
    Impl(const bloom_parameters& params): bloom_filter(params) {}
    Impl(const TBloomFilter::Impl& bloomFilter): bloom_filter(bloomFilter) {}
    ~Impl() {}
    void Dump(std::ostream& out) const {
        ::zpds::handypack::Dump(out, salt_, bit_table_, salt_count_, table_size_,
                        projected_element_count_, inserted_element_count_,
                        random_seed_, desired_false_positive_probability_);
    }
    void Load(std::istream& in) {
        ::zpds::handypack::Load(in, salt_, bit_table_, salt_count_, table_size_,
                        projected_element_count_, inserted_element_count_,
                        random_seed_, desired_false_positive_probability_);
    }
};

TBloomFilter::TBloomFilter() {
    BloomFilter.reset(new TBloomFilter::Impl());
}

TBloomFilter::TBloomFilter(uint64_t elements, double falsePositiveRate) {
    bloom_parameters parameters;
    parameters.projected_element_count = elements;
    parameters.false_positive_probability = falsePositiveRate;
    parameters.random_seed = 42;
    parameters.compute_optimal_parameters();
    assert(!(!parameters));
    BloomFilter.reset(new TBloomFilter::Impl(parameters));
}

TBloomFilter::~TBloomFilter() {
}

void TBloomFilter::Insert(const std::string& element) {
    BloomFilter->insert(element);
}

bool TBloomFilter::Contains(const std::string& element) const {
    return BloomFilter->contains(element);
}

void TBloomFilter::Dump(std::ostream& out) const {
    BloomFilter->Dump(out);
}

void TBloomFilter::Load(std::istream& in) {
    BloomFilter->Load(in);
}

} // jamspell
} // zpds
