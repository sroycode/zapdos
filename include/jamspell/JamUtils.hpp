/**
 * @project zapdos
 * @file include/jamspell/JamUtils.hpp
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
 *  JamUtils.hpp : Jamspell utils
 *
 */
#ifndef _ZPDS_JAMSPELL_JAM_UTILS_HPP_
#define _ZPDS_JAMSPELL_JAM_UTILS_HPP_

#include <string>
#include <vector>
#include <unordered_set>
#include <locale>

#include "jamspell/HandyPack.hpp"

namespace zpds {
namespace jamspell {

struct TWord {
    TWord() = default;
    TWord(const wchar_t* ptr, size_t len)
        : Ptr(ptr)
        , Len(len)
    {
    }
    TWord(const std::wstring& w)
        : Ptr(&w[0])
        , Len(w.size())
    {
    }
    bool operator ==(const TWord& other) const {
        return (Ptr == other.Ptr && Len == other.Len);
    }
    const wchar_t* Ptr = nullptr;
    size_t Len = 0;
};

struct TWordHashPtr {
public:
  std::size_t operator()(const TWord& x) const {
      return (size_t)x.Ptr;
  }
};

using TWords = std::vector<TWord>;
using TSentences = std::vector<TWords>;

class TTokenizer {
public:
    TTokenizer();
    bool LoadAlphabet(const std::string& alphabetString);
    TSentences Process(const std::wstring& originalText) const;
    void Clear();

    const std::unordered_set<wchar_t>& GetAlphabet() const;

    HANDYPACK(Alphabet)
private:
    std::unordered_set<wchar_t> Alphabet;
    std::locale Locale;
};

std::string LoadFile(const std::string& fileName);
void SaveFile(const std::string& fileName, const std::string& data);
std::wstring UTF8ToWide(const std::string& text);
std::string WideToUTF8(const std::wstring& text);
uint64_t GetCurrentTimeMs();
void ToLower(std::wstring& text);
wchar_t MakeUpperIfRequired(wchar_t orig, wchar_t sample);
uint16_t CityHash16(const std::string& str);
uint16_t CityHash16(const char* str, size_t size);

} // jamspell
} // zpds
#endif // _ZPDS_JAMSPELL_JAM_UTILS_HPP_
