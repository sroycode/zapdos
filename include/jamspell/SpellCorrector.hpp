/**
 * @project zapdos
 * @file include/jamspell/SpellCorrector.hpp
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
 *  SpellCorrector.hpp : Spell Corrector
 *
 */
#ifndef _ZPDS_JAMSPELL_SPELL_CORRECTOR_HPP_
#define _ZPDS_JAMSPELL_SPELL_CORRECTOR_HPP_

#include <memory>

#include "jamspell/LangModel.hpp"
#include "jamspell/BloomFilter.hpp"

namespace zpds {
namespace jamspell {

class TSpellCorrector {
public:
    bool LoadLangModel(const std::string& modelFile);
    bool TrainLangModel(const std::string& textFile, const std::string& alphabetString, const std::string& modelFile);
    jamspell::TWords GetCandidatesRaw(const jamspell::TWords& sentence, size_t position) const;
    std::vector<std::wstring> GetCandidates(const std::vector<std::wstring>& sentence, size_t position) const;
    std::wstring FixFragment(const std::wstring& text) const;
    std::wstring FixFragmentNormalized(const std::wstring& text) const;
    void SetPenalty(double knownWordsPenaly, double unknownWordsPenalty);
    void SetMaxCandiatesToCheck(size_t maxCandidatesToCheck);
    const jamspell::TLangModel& GetLangModel() const;
private:
    void FilterCandidatesByFrequency(std::unordered_set<jamspell::TWord, jamspell::TWordHashPtr>& uniqueCandidates, jamspell::TWord origWord) const;
    jamspell::TWords Edits(const jamspell::TWord& word) const;
    jamspell::TWords Edits2(const jamspell::TWord& word, bool lastLevel = true) const;
    void Inserts(const std::wstring& w, jamspell::TWords& result) const;
    void Inserts2(const std::wstring& w, jamspell::TWords& result) const;
    void PrepareCache();
    bool LoadCache(const std::string& cacheFile);
    bool SaveCache(const std::string& cacheFile);
private:
    TLangModel LangModel;
    std::unique_ptr<TBloomFilter> Deletes1;
    std::unique_ptr<TBloomFilter> Deletes2;
    double KnownWordsPenalty = 20.0;
    double UnknownWordsPenalty = 5.0;
    size_t MaxCandiatesToCheck = 14;
};

} // jamspell
} // zpds
#endif // _ZPDS_JAMSPELL_SPELL_CORRECTOR_HPP_
