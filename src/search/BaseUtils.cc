/**
 * @project zapdos
 * @file src/search/BaseUtils.cc
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
 *  BaseUtils.cc : Common Search Utils impl
 *
 */
#include <sstream>
#include <boost/algorithm/string.hpp>
#include <iomanip>
#include <cmath>
#include <utility>

#include "search/BaseUtils.hpp"
#include "utils/SplitWith.hpp"

#include "search/KrovetzStemmer.hpp"
thread_local ::stem::KrovetzStemmer stemmer;

/**
* StemQuery : stem the query
*
*/
std::string zpds::search::BaseUtils::StemQuery(const std::string& input, bool notlast) const
{

	std::ostringstream xtmp;
	// bool space=false;
	auto words = ::zpds::utils::SplitWith::LowerNoQuote(input);
	size_t xc = words.size();
	for ( auto it = words.begin() ; it != words.end() ; ++it) {
		bool to_stem = (it->length() >= XAP_MIN_STEM_LEN );
		--xc;
		if ( notlast && xc==0) to_stem=false;
		if (to_stem) {
			char word[26];
			strcpy(word, it->substr(0,25).c_str() );
			const char* nword = stemmer.kstem_stemmer(word);
			xtmp << std::string(nword);
		}
		else {
			xtmp << *it;
		}
		if (xc>0) xtmp << XAP_FORMAT_SPACE;
	}
	return xtmp.str();
}

/**
* StemWord : stem the word
*
*/
std::string zpds::search::BaseUtils::StemWord(const std::string& input) const
{
	char word[26];
	strcpy(word, input.substr(0,25).c_str() );
	const char* nword = stemmer.kstem_stemmer(word);
	return std::string(nword);
}

/**
 * SplitMerge : string to set of words
 *
 */
::zpds::search::BaseUtils::WordVecT zpds::search::BaseUtils::SplitMerge(const std::string& input) const
{
	auto wordstr = ::zpds::utils::SplitWith::LowerNoQuote(input);
	WordVecT::iterator it = wordstr.begin();
	// size_t counter=0;
	while(it != wordstr.end()) {
		if (it==wordstr.begin()) {
			++it;
			continue;
		}
		WordVecT::iterator jt = it-1;
		if ( ( *it == *jt ) || ( it->length() <= XAP_MERGE_LIMIT && jt->length() <= XAP_MERGE_LIMIT ) ) {
			*jt = *jt + *it;
			it = wordstr.erase(it);
		}
		else ++it;
	}
	return wordstr;
}

/**
* NoSpace : no space
*
*/
std::string zpds::search::BaseUtils::NoSpace(const std::string& orig) const
{
	if (orig.empty()) return std::string();
	std::ostringstream xtmp;
	std::locale loc;
	for (auto i = orig.cbegin(), n = orig.cend(); i != n; ++i) {
		auto c = (*i);
		// if alnum lowercase into string
		if (std::isalnum(c)) xtmp << std::tolower(c,loc);
	}
	return xtmp.str();
}

/**
* LcTrim : Lowercase Trim
*
*/
std::string zpds::search::BaseUtils::LcTrim(const std::string& orig) const
{
	std::string s = orig;
	boost::algorithm::to_lower(s);
	boost::algorithm::trim(s);
	return s;
}

/**
* Flatten : string to lowrcase nospace word
*
*/
std::string zpds::search::BaseUtils::Flatten(const std::string& input) const
{
	std::string s;
	std::tie(s,std::ignore)=FlattenCount(input);
	return s;
}

/**
* FlattenCount : string to lowrcase nospace word
*
*/
std::pair<std::string,size_t> zpds::search::BaseUtils::FlattenCount(const std::string& input) const
{
	size_t wc=0;
	std::ostringstream xtmp;
	std::locale loc;
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
			++wc;
			xtmp << ' ';
			last_is_char=false;
		}
	}
	if (last_is_char) ++wc;
	// parsing end
	return std::make_pair( xtmp.str(), wc );
}

/**
* FormatPrefix :  to lowercase nospace word
*
*/
std::string zpds::search::BaseUtils::FormatPrefix(const std::string& input, const std::string& prefix) const
{
	return prefix + NoSpace(input);
}

/**
* FormatTag :  to lowercase nospace predicate word
*
*/
std::string zpds::search::BaseUtils::FormatTag(const std::string& name, const std::string& value) const
{
	return name + "=" + NoSpace(value);
}

/**
* CleanStr : clean up tab etc characters
*
*/
std::string zpds::search::BaseUtils::CleanStr(const std::string& input) const
{
	std::ostringstream xtmp;
	for (auto i = input.cbegin(), n = input.cend(); i != n; ++i) {
		auto c = (*i);
		if (std::isspace(c)) xtmp << ' ';
		else if (c=='\'' || c=='\"') continue;
		else xtmp << c;
	}
	// parsing end
	return xtmp.str();
}

