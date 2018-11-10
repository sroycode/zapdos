/**
 * @project zapdos
 * @file src/jamspell/PerfectHash.cc
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
 *  PerfectHash.cc :  perfect hash impl
 *
 */
#include "jamspell/HandyPack.hpp"
#include "jamspell/phf.h"
#include "jamspell/PerfectHash.hpp"

#include <cassert>

namespace zpds {
namespace jamspell {

void TPerfectHash::Dump(std::ostream& out) const {
    const phf& perfHash = *(const phf*)Phf;
    ::zpds::handypack::Dump(out, perfHash.d_max,
                         perfHash.g_op,
                         perfHash.m,
                         perfHash.r,
                         perfHash.seed,
                         perfHash.nodiv);
    out.write((const char*)perfHash.g, perfHash.r * sizeof(uint32_t));
}

void TPerfectHash::Load(std::istream& in) {
    Clear();
    Phf = new phf();
    phf& perfHash = *(phf*)Phf;
    ::zpds::handypack::Load(in, perfHash.d_max,
                        perfHash.g_op,
                        perfHash.m,
                        perfHash.r,
                        perfHash.seed,
                        perfHash.nodiv);
    perfHash.g = (uint32_t*)calloc(perfHash.r, sizeof(uint32_t));
    in.read((char*)perfHash.g, perfHash.r * sizeof(uint32_t));
}

bool TPerfectHash::Init(const std::vector<std::string>& keys) {
    std::vector<phf_string_t> keysForPhf;
    keysForPhf.reserve(keys.size());
    for (const std::string& s: keys) {
        keysForPhf.push_back({&s[0], s.size()});
    }

    phf* tempPhf = new phf();
    phf_error_t res = PHF::init<phf_string_t, false>(tempPhf, &keysForPhf[0], keysForPhf.size(), 4, 80, 42);
    if (res != 0) {
        PHF::destroy(tempPhf);
        delete tempPhf;
        return false;
    }
    Clear();
    Phf = tempPhf;
    return true;
}

void TPerfectHash::Clear() {
    if (!Phf) {
        return;
    }
    PHF::destroy((phf*)Phf);
    delete (phf*)Phf;
}

uint32_t TPerfectHash::Hash(const std::string& value) const {
    assert(Phf && "Not initialized");
    return PHF::hash<std::string>((phf*)Phf, value);
}

uint32_t TPerfectHash::Hash(const char* value, size_t size) const {
    assert(Phf && "Not initialized");
    phf_string_t phfValue = {value, size};
    return PHF::hash<phf_string_t>((phf*)Phf, phfValue);
}

uint32_t TPerfectHash::BucketsNumber() const {
    const phf* p = (phf*)Phf;
    return p->m;
}

TPerfectHash::TPerfectHash()
    : Phf(nullptr)
{
}

TPerfectHash::~TPerfectHash() {
    Clear();
}

} // jamspell
} // zpds
