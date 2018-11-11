/**
 * @project zapdos
 * @file src/search/TrieBase.cc
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
 *  TrieBase :  container for Trie implementation
 *
 */

#include "search/TrieBase.hpp"
#include "utils/StringHelpers.hpp"

/**
* StemQuery : stem the query
*
*/
std::string zpds::search::TrieBase::StemQuery(std::string& input, bool notlast)
{

	std::ostringstream xtmp;
	bool space=false;
	auto words = Split(input);
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
		if (xc>0) xtmp << " ";
	}
	return xtmp.str();
}

/**
 * SplitMerge : string to set of words
 *
 */
::zpds::search::TrieBase::WordVecT zpds::search::TrieBase::SplitMerge(const std::string& input)
{
	auto wordstr = Split(input);
	::zpds::search::TrieBase::WordVecT::iterator it = wordstr.begin();
	size_t counter=0;
	while(it != wordstr.end()) {
		if (it==wordstr.begin()) {
			++it;
			continue;
		}
		::zpds::search::TrieBase::WordVecT::iterator jt = it-1;
		if ( ( *it == *jt ) || ( it->length() <= XAP_MERGE_LIMIT && jt->length() <= XAP_MERGE_LIMIT ) ) {
			*jt = *jt + *it;
			it = wordstr.erase(it);
		}
		else ++it;
	}
	return wordstr;
}


