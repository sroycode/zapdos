/**
 * @project zapdos
 * @file include/jamspell/LangModel.hpp
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
 *  LangModel.hpp :  Language Model headers
 *
 */
#ifndef _ZPDS_JAMSPELL_LANG_MODEL_HPP_
#define _ZPDS_JAMSPELL_LANG_MODEL_HPP_

#include <unordered_map>
#include <vector>
#include <utility>
#include <string>
#include <limits>

#include "jamspell/HandyPack.hpp"
#include "jamspell/robin_map.h"
#include "jamspell/JamUtils.hpp"
#include "jamspell/PerfectHash.hpp"


namespace zpds {
namespace jamspell {


constexpr uint64_t LANG_MODEL_MAGIC_BYTE = 8559322735408079685L;
constexpr uint16_t LANG_MODEL_VERSION = 9;
constexpr double LANG_MODEL_DEFAULT_K = 0.05;

using TWordId = uint32_t;
using TCount = uint32_t;

using TGram1Key = TWordId;
using TGram2Key = std::pair<TWordId, TWordId>;
using TGram3Key = std::tuple<TWordId, TWordId, TWordId>;
using TWordIds = std::vector<TWordId>;
using TIdSentences = std::vector<TWordIds>;

struct TGram2KeyHash {
public:
  std::size_t operator()(const TGram2Key& x) const {
      return (size_t)x.first ^ ((size_t)x.second << 16);
  }
};

struct TGram3KeyHash {
public:
  std::size_t operator()(const TGram3Key& x) const {
    return (size_t)std::get<0>(x) ^
            ((size_t)std::get<1>(x) << 16) ^
            ((size_t)std::get<2>(x) << 32);
  }
};

class TRobinSerializer: public handypack::TUnorderedMapSerializer<tsl::robin_map<std::wstring, TWordId>, std::wstring, TWordId> {};
class TRobinHash: public tsl::robin_map<std::wstring, TWordId> {
public:
    inline virtual void Dump(std::ostream& out) const {
        TRobinSerializer::Dump(out, *this);
    }
    inline virtual void Load(std::istream& in) {
        TRobinSerializer::Load(in, *this);
    }
};

class TLangModel {
public:
    bool Train(const std::string& fileName, const std::string& alphabetString);
    double Score(const TWords& words) const;
    double Score(const std::wstring& str) const;
    TWord GetWord(const std::wstring& word) const;
    const std::unordered_set<wchar_t>& GetAlphabet() const;
    TSentences Tokenize(const std::wstring& text) const;

    bool Dump(const std::string& modelFileName) const;
    bool Load(const std::string& modelFileName);
    void Clear();

    const TRobinHash& GetWordToId();

    TWordId GetWordId(const TWord& word);
    TWordId GetWordIdNoCreate(const TWord& word) const;
    TWord GetWordById(TWordId wid) const;
    TCount GetWordCount(TWordId wid) const;

    uint64_t GetCheckSum() const;

    HANDYPACK(WordToId, LastWordID, TotalWords, VocabSize,
              PerfectHash, Buckets, Tokenizer, CheckSum)
private:
    TIdSentences ConvertToIds(const TSentences& sentences);

    double GetGram1Prob(TWordId word) const;
    double GetGram2Prob(TWordId word1, TWordId word2) const;
    double GetGram3Prob(TWordId word1, TWordId word2, TWordId word3) const;

    TCount GetGram1HashCount(TWordId word) const;
    TCount GetGram2HashCount(TWordId word1, TWordId word2) const;
    TCount GetGram3HashCount(TWordId word1, TWordId word2, TWordId word3) const;

private:
    const TWordId UnknownWordId = std::numeric_limits<TWordId>::max();
    double K = LANG_MODEL_DEFAULT_K;
    TRobinHash WordToId;
    std::vector<const std::wstring*> IdToWord;
    TWordId LastWordID = 0;
    TWordId TotalWords = 0;
    TWordId VocabSize = 0;
    TTokenizer Tokenizer;
    std::vector<std::pair<uint16_t, uint16_t>> Buckets;
    TPerfectHash PerfectHash;
    uint64_t CheckSum;
};


} // jamspell
} // zpds
#endif  // _ZPDS_JAMSPELL_LANG_MODEL_HPP_
