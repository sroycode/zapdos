/**
 * @project zapdos
 * @file include/jamspell/PerfectHash.hpp
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
 *  PerfectHash.hpp : Jamspell Perfect Hash Headers
 *
 */
#ifndef _ZPDS_JAMSPELL_PERFECT_HASH_HPP_
#define _ZPDS_JAMSPELL_PERFECT_HASH_HPP_

#include <ostream>
#include "utils/BaseUtils.hpp"

namespace zpds {
namespace jamspell {

class TPerfectHash {
public:
    TPerfectHash();
    TPerfectHash(const TPerfectHash& other) = delete;
    ~TPerfectHash();
    void Dump(std::ostream& out) const;
    void Load(std::istream& in);
    bool Init(const std::vector<std::string>& keys);
    void Clear();
    uint32_t Hash(const std::string& value) const;
    uint32_t Hash(const char* value, size_t size) const;
    uint32_t BucketsNumber() const;
private:
    void* Phf; // sort of forward declaration
};

} // jamspell
} // zpds
#endif // _ZPDS_JAMSPELL_PERFECT_HASH_HPP_
