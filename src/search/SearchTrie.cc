/**
 * @project zapdos
 * @file src/search/SearchTrie.cc
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
 *  SearchTrie :  search in xapian impl
 *
 */

#include <unordered_set>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

#include "search/SearchTrie.hpp"
#include "search/DistanceSlabKeyMaker.hpp"
#include "utils/StringHelpers.hpp"

/**
* Constructor : with query
*
*/
zpds::search::SearchTrie::SearchTrie(const std::string& dbpath_) : dbpath(dbpath_)
{
	if (dbpath.empty()) throw zpds::BadCodeException("dbpath cannot be blank");
}

/**
* destructor
*/
zpds::search::SearchTrie::~SearchTrie () {}

/**
* Get: get the storage instance
*
*/
zpds::search::SearchTrie::DatabaseT& zpds::search::SearchTrie::Get(::zpds::search::LangTypeE ltyp, ::zpds::search::DataTypeE dtyp)
{
	const google::protobuf::EnumDescriptor *l = zpds::search::LangTypeE_descriptor();
	const google::protobuf::EnumDescriptor *d = zpds::search::DataTypeE_descriptor();
	int f = ltyp * 1000 + dtyp;
	if ( triemap.find(f) == triemap.end() ) {
		const std::string xapath{dbpath + "/" + l->FindValueByNumber(ltyp)->name() + "_" + d->FindValueByNumber(dtyp)->name()};
		if (!boost::filesystem::exists(xapath))
			throw ::zpds::BadCodeException("search dir does no exist for this language");
		triemap[f] = zpds::search::SearchTrie::DatabaseT(xapath, Xapian::DB_OPEN );
	}
	return triemap.at(f);
}

/**
* GetQuerySize : get size of current query
*
*/
size_t zpds::search::SearchTrie::GetQuerySize()
{
	return wordstr.size();
}

/**
* SetQuery : reset the wordstr
*
*/
size_t zpds::search::SearchTrie::SetQuery(const std::string& query_)
{
	wordstr = SplitMerge(query_);
	return wordstr.size();
}

/**
* GetQueryString: set query
*
*/
std::string zpds::search::SearchTrie::GetQueryString(
    QueryWordsE qtype, const std::string& extra,
    const std::string& full_prefix, const std::string& part_prefix)
{
	std::ostringstream xtmp;
	for (size_t i=0; i<wordstr.size(); ++i) {
		if ( ( qtype == ALL_PARTIAL ) || ( qtype == LAST_PARTIAL && i==(wordstr.size()-1) ) ) {
			xtmp << ' ' << part_prefix << wordstr.at(i);
		}
		else {
			xtmp << ' ' << full_prefix << wordstr.at(i);
		}
	}
	if (! extra.empty() ) xtmp << ' ' << extra;
	return xtmp.str();
}


/**
* FindNear: check if exists nearby
*
*/
bool zpds::search::SearchTrie::FindNear(::zpds::search::UsedParamsT* qr, bool reset)
{

	auto db = Get(qr->lang(), qr->dtyp() );
	auto wordstr_size = wordstr.size();
	if (qr->cur().dont_use()) return false;
	if (reset) wordstr_size = SetQuery(qr->query());

	if ( (! qr->noname() ) && ( wordstr_size==0) ) return false;

	QueryWordsE qtype = ( qr->all_partial() ) ? ALL_PARTIAL : ( qr->last_partial() ) ? LAST_PARTIAL : FULL_WORDS;

	std::string full_prefix = std::string("+");
	std::string part_prefix = std::string("+") + XAP_PARTWORD_PREFIX;
	if ( qr->use_name() ) {
		full_prefix = std::string("+") + XAP_NAMEFULL_PREFIX;
		part_prefix = std::string("+") + XAP_NAMEPART_PREFIX;
	}
	if ( qr->use_tags() ) {
		full_prefix = std::string("+") + XAP_TAG_PREFIX;
		part_prefix = std::string("+") + XAP_TAG_PREFIX;
	}
	std::string xtmp = GetQueryString( qtype, qr->extra(), full_prefix, part_prefix);

	unsigned int flags =
	    Xapian::QueryParser::FLAG_LOVEHATE
	    | Xapian::QueryParser::FLAG_BOOLEAN
	    ;
	Xapian::QueryParser queryparser;
	queryparser.set_database(db);
	Xapian::Enquire enquire(db);
	enquire.set_weighting_scheme(Xapian::BoolWeight());

	Xapian::Query query = queryparser.parse_query(xtmp, flags);
	enquire.set_query(query);

	Xapian::LatLongCoord centre( qr->cur().lat(), qr->cur().lon() );
	zpds::search::DistanceSlabKeyMaker keymaker(XAP_LATLON_POS, XAP_IMPORTANCE_POS, centre, qr->distance_band(), qr->distance_def() );
	enquire.set_sort_by_key(&keymaker, false);

	Xapian::MSet mset;
	mset = enquire.get_mset(0,qr->items());
	if (mset.size()>0) {
		for (Xapian::MSetIterator m = mset.begin(); m != mset.end(); ++m) {
			qr->add_ids( boost::lexical_cast<uint64_t>( m.get_document().get_value( XAP_ROCKSID_POS )) );
			qr->add_sortkeys( m.get_sort_key() );
		}
	}
	return (mset.size()>0);
}

/**
* FindFull: check if exists nearby
*
*/
bool zpds::search::SearchTrie::FindFull(::zpds::search::UsedParamsT* qr,bool reset)
{

	auto db = Get(qr->lang(), qr->dtyp() );
	auto wordstr_size = wordstr.size();
	if (reset) wordstr_size = SetQuery(qr->query());
	if (wordstr_size==0) return false;

	QueryWordsE qtype = ( qr->all_partial() ) ? ALL_PARTIAL : ( qr->last_partial() ) ? LAST_PARTIAL : FULL_WORDS;

	std::string full_prefix = std::string("+");
	std::string part_prefix = std::string("+") + XAP_PARTWORD_PREFIX;
	if ( qr->use_name() ) {
		full_prefix = std::string("+") + XAP_NAMEFULL_PREFIX;
		part_prefix = std::string("+") + XAP_NAMEPART_PREFIX;
	}
	if ( qr->use_tags() ) {
		full_prefix = std::string("+") + XAP_TAG_PREFIX;
		part_prefix = std::string("+") + XAP_TAG_PREFIX;
	}
	std::string xtmp = GetQueryString( qtype, qr->extra(), full_prefix, part_prefix);

	unsigned int flags =
	    Xapian::QueryParser::FLAG_LOVEHATE
	    | Xapian::QueryParser::FLAG_BOOLEAN
	    ;
	Xapian::QueryParser queryparser;
	queryparser.set_database(db);
	Xapian::Enquire enquire(db);
	enquire.set_weighting_scheme(Xapian::BoolWeight());

	Xapian::Query query = queryparser.parse_query(xtmp, flags);
	enquire.set_query(query);
	enquire.set_sort_by_value(XAP_IMPORTANCE_POS,true);

	Xapian::MSet mset;
	mset = enquire.get_mset(0,qr->items());
	if (mset.size()>0) {
		for (Xapian::MSetIterator m = mset.begin(); m != mset.end(); ++m) {
			qr->add_ids( boost::lexical_cast<uint64_t>( m.get_document().get_value( XAP_ROCKSID_POS )) );
			qr->add_sortkeys( m.get_sort_key() );
		}
	}
	return (mset.size()>0);
}


/**
* EstimateExec: estimate execution time
*
*/
uint64_t zpds::search::SearchTrie::EstimateExec(::zpds::search::SearchTrie::DatabaseT& db, std::string& input)
{
	uint64_t est= XAP_CAN_BEGIN_THRESHOLD + 1;
	std::vector<std::string> arr = SplitBySpace( input );
	for (auto& word : arr) {
		if (word.at(0) != '+') continue;
		auto word_freq = db.get_termfreq( word.substr(1) );
		if (word_freq < est ) est = word_freq;
	}
	return est;
}

/**
* WarmCache : warms up cache for db
*
*/
void zpds::search::SearchTrie::WarmCache(::zpds::search::LangTypeE lang, ::zpds::search::DataTypeE dtyp, size_t modno, size_t outof)
{

	auto db = Get(lang, dtyp );
	zpds::search::SearchTrie::WordVecT wvec ;

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
void zpds::search::SearchTrie::DoWarmCache(::zpds::search::SearchTrie::DatabaseT& db, zpds::search::SearchTrie::WordVecT wvec)
{

	WordVecT pvec { "", XAP_BEGINFULL_PREFIX, XAP_BEGINPART_PREFIX, XAP_PARTWORD_PREFIX};

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
			std::string xtmp = std::string("+") + prefix + LcTrim(word);
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

