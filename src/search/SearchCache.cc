/**
 * @project zapdos
 * @file src/search/SearchCache.cc
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
 *  SearchCache.cc : Xapian Search Cache impl
 *
 */
#include <unordered_set>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#include "search/SearchCache.hpp"
#include "utils/SplitWith.hpp"

/**
* Constructor : with query
*
*/
zpds::search::SearchCache::SearchCache(const std::string& dbpath_)
	: ReadIndex(dbpath_)
{
}

/**
* destructor
*/
zpds::search::SearchCache::~SearchCache () {}

/**
* WarmCache : warms up cache for db
*
*/
void zpds::search::SearchCache::WarmCache(::zpds::search::LangTypeE lang, ::zpds::search::IndexTypeE dtyp, size_t modno, size_t outof)
{

	auto db = Get(lang, dtyp );
	zpds::search::SearchCache::WordVecT wvec ;

	if (outof>modno) {
		size_t counter =0;
		for (char i = 'a'; i <= 'z'; ++i) {
			std::string s_i(1,i);
			if (++counter % outof ==modno) {
				wvec.emplace_back( s_i);
			}
			for (char j = 'a'; j <= 'z'; ++j) {
				if (++counter % outof ==modno) {
					wvec.emplace_back( s_i + std::string(1,j));
				}
			}
		}
	}
	else {
		for (char i = 'a'; i <= 'z'; ++i) {
			std::string s_i(1,i);
			wvec.emplace_back( s_i);
			for (char j = 'a'; j <= 'z'; ++j) {
				wvec.emplace_back( s_i + std::string(1,j));
			}
		}
	}
	DoWarmCache(db,wvec);
}

/**
* DoWarmCache : warms up cache for a set of words
*
*/
void zpds::search::SearchCache::DoWarmCache(::zpds::search::SearchCache::DatabaseT& db, zpds::search::SearchCache::WordVecT wvec)
{

	WordVecT pvec { XAP_BEGINFULL_PREFIX, XAP_BEGINPART_PREFIX, "", XAP_PARTWORD_PREFIX, XAP_NAMEFULL_PREFIX, XAP_NAMEPART_PREFIX};

	unsigned int flags = Xapian::QueryParser::FLAG_LOVEHATE;

	Xapian::QueryParser queryparser;
	queryparser.set_database(db);
	Xapian::Enquire enquire(db);
	enquire.set_weighting_scheme(Xapian::BoolWeight());

	size_t counter =0;
	for (auto& word : wvec) {
		++counter;
		uint64_t currtime = ZPDS_CURRTIME_MS;
		for (auto& prefix : pvec) {
			auto tword = boost::algorithm::to_lower_copy(word);
			boost::algorithm::trim(tword);
			std::string xtmp = std::string("+") + prefix + tword;
			Xapian::Query query = queryparser.parse_query(xtmp, flags);

			{
				enquire.set_query(query);
				enquire.set_sort_by_value(XAP_LATLON_POS,true);
				Xapian::MSet mset;
				mset = enquire.get_mset(0,10);
			}
			{
				enquire.set_query(query);
				enquire.set_sort_by_value(XAP_IMPORTANCE_POS,true);
				Xapian::MSet mset;
				mset = enquire.get_mset(0,10);
			}

		}
		if ((ZPDS_CURRTIME_MS - currtime) > 1000) {
			LOG(INFO) << "Step " << counter << "/" << wvec.size() << " took " << (ZPDS_CURRTIME_MS - currtime)/1000 << " s";
		}
	}
}

/**
* CompletionQueryAction : simple query service action
*
*/
void zpds::search::SearchCache::CompletionQueryAction(::zpds::utils::SharedTable::pointer stptr, ::zpds::query::SearchCompletionRespT* resp)
{
	throw ::zpds::BadCodeException("This class has no query");
}
