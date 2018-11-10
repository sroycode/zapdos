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
zpds::search::SearchTrie::DatabaseT& zpds::search::SearchTrie::Get(::zpds::search::LangTypeE dtyp)
{
	const google::protobuf::EnumDescriptor *d = zpds::search::LangTypeE_descriptor();
	if ( triemap.find(dtyp) == triemap.end() ) {
		const std::string xapath{dbpath + "/" + d->FindValueByNumber(dtyp)->name()};
		if (!boost::filesystem::exists(xapath))
			throw zpds::BadCodeException("search dir does no exist for this language");
		triemap[dtyp] = zpds::search::SearchTrie::DatabaseT(xapath, Xapian::DB_OPEN );
	}
	return triemap.at(dtyp);
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
    QueryWordsE qtype, const std::string& extra, std::string prefix )
{
	std::ostringstream xtmp;
	for (size_t i=0; i<wordstr.size(); ++i) {
		if ( ( qtype == ALL_PARTIAL ) || ( qtype == LAST_PARTIAL && i==(wordstr.size()-1) ) ) {
			xtmp << ' ' << prefix << XAP_PARTWORD_PREFIX << wordstr.at(i);
		}
		else {
			xtmp << ' ' << prefix << wordstr.at(i);
		}
	}
	if (! extra.empty() ) xtmp << ' ' << extra;
	return xtmp.str();
}


/**
* GetQueryStringPart: set query
*
*/
std::string zpds::search::SearchTrie::GetQueryStringPart(
    QueryWordsE qtype, size_t start, size_t end, const std::string& extra, std::string prefix )
{
	std::ostringstream xtmp;
	if ( start >= wordstr.size() )
		throw ::zpds::BadCodeException("start is wrong");
	if (end >= wordstr.size() ) end = wordstr.size() -1 ;

	for (size_t i=start; i<=end; ++i) {
		if ( ( qtype == ALL_PARTIAL ) || ( qtype == LAST_PARTIAL && i==(wordstr.size()-1) ) ) {
			xtmp << ' ' << prefix << XAP_PARTWORD_PREFIX << wordstr.at(i);
		}
		else {
			xtmp << ' ' << prefix << wordstr.at(i);
		}
	}
	if (! extra.empty() ) xtmp << ' ' << extra;
	return xtmp.str();
}


/**
* FindNear: check if exists nearby
*
*/
bool zpds::search::SearchTrie::FindNear(::zpds::search::SearchTrie::DatabaseT& db, ::zpds::search::UsedParamsT* qr, bool reset)
{

	auto wordstr_size = wordstr.size();
	if (reset) wordstr_size = SetQuery(qr->query());
	if (wordstr_size==0) return false;
	if (qr->cur().dont_use()) return false;

	QueryWordsE qtype = ( qr->all_partial() ) ? ALL_PARTIAL : ( qr->last_partial() ) ? LAST_PARTIAL : FULL_WORDS;

	std::ostringstream xtmp;
	if ( qr->begin_with() ) {
		xtmp << "+";
		if (( qtype == FULL_WORDS ) || (qtype==LAST_PARTIAL && wordstr_size>1))
			xtmp << XAP_BEGINFULL_PREFIX;
		else
			xtmp << XAP_BEGINPART_PREFIX;
		xtmp << wordstr.at(0) ;
		if (wordstr_size>1) xtmp << " " + GetQueryStringPart( qtype, 1, wordstr_size-1, qr->extra(), "");
	}
	else {
		xtmp << GetQueryString( qtype, qr->extra() );
	}

	unsigned int flags =
	    Xapian::QueryParser::FLAG_LOVEHATE
	    | Xapian::QueryParser::FLAG_BOOLEAN
	    ;
	Xapian::QueryParser queryparser;
	queryparser.set_database(db);
	Xapian::Enquire enquire(db);
	enquire.set_weighting_scheme(Xapian::BoolWeight());

	Xapian::Query query = queryparser.parse_query(xtmp.str(), flags);
	enquire.set_query(query);

	Xapian::LatLongCoord centre( qr->cur().lat(), qr->cur().lon() );
	zpds::search::DistanceSlabKeyMaker keymaker(XAP_LATLON_POS, XAP_IMPORTANCE_POS, centre, qr->distance_band(), qr->distance_def() );
	enquire.set_sort_by_key(&keymaker, false);

	Xapian::MSet mset;
	mset = enquire.get_mset(0,qr->items());
	if (mset.size()>0) {
		for (Xapian::MSetIterator m = mset.begin(); m != mset.end(); ++m) {
			qr->add_ids(*m);
			qr->add_sortkeys( m.get_sort_key() );
		}
	}
	return (mset.size()>0);
}

/**
* FindFull: check if exists nearby
*
*/
bool zpds::search::SearchTrie::FindFull(::zpds::search::SearchTrie::DatabaseT& db, ::zpds::search::UsedParamsT* qr,bool reset)
{

	auto wordstr_size = wordstr.size();
	if (reset) wordstr_size = SetQuery(qr->query());
	if (wordstr_size==0) return false;

	QueryWordsE qtype = ( qr->all_partial() ) ? ALL_PARTIAL : ( qr->last_partial() ) ? LAST_PARTIAL : FULL_WORDS;

	std::ostringstream xtmp;
	if ( qr->begin_with() ) {
		xtmp << "+";
		if (( qtype == FULL_WORDS ) || (qtype==LAST_PARTIAL && wordstr_size>1))
			xtmp << XAP_BEGINFULL_PREFIX;
		else
			xtmp << XAP_BEGINPART_PREFIX;
		xtmp << wordstr.at(0) ;
		if (wordstr_size>1) xtmp << " " + GetQueryStringPart( qtype, 1, wordstr_size-1, qr->extra(), "");
	}
	else {
		xtmp << GetQueryString( qtype, qr->extra() );
	}

	unsigned int flags =
	    Xapian::QueryParser::FLAG_LOVEHATE
	    | Xapian::QueryParser::FLAG_BOOLEAN
	    ;
	Xapian::QueryParser queryparser;
	queryparser.set_database(db);
	Xapian::Enquire enquire(db);
	enquire.set_weighting_scheme(Xapian::BoolWeight());

	Xapian::Query query = queryparser.parse_query(xtmp.str(), flags);
	enquire.set_query(query);
	enquire.set_sort_by_value(XAP_IMPORTANCE_POS,true);

	Xapian::MSet mset;
	mset = enquire.get_mset(0,qr->items());
	if (mset.size()>0) {
		for (Xapian::MSetIterator m = mset.begin(); m != mset.end(); ++m) {
			qr->add_ids(*m);
			qr->add_sortkeys( m.get_sort_key() );
		}
	}
	return (mset.size()>0);
}

/**
* StemQuery : stem the query
*
*/
std::string zpds::search::SearchTrie::StemQuery(std::string& input, bool notlast)
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
void zpds::search::SearchTrie::WarmCache(::zpds::search::LangTypeE lang, size_t modno, size_t outof)
{

	auto db = Get(lang);
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
	WarmCache(db,wvec);
}

/**
* WarmCache : warms up cache for a set of words
*
*/
void zpds::search::SearchTrie::WarmCache(::zpds::search::SearchTrie::DatabaseT& db, zpds::search::SearchTrie::WordVecT wvec)
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



// shreos wip
/**
* RuleSearch : do the search from rule
*
*/
bool zpds::search::SearchTrie::RuleSearch(
    ::zpds::search::UsedParamsT* qr,
    ::zpds::search::QueryOrderT* rule,
    ::zpds::store::LookupRecordListT* records,
    size_t target)
{

	::zpds::search::UsedParamsT mp = *qr;
	auto ploc = mp.mutable_cur();

	auto db = Get(mp.lang());

	// items to populate
	mp.set_items ( KeepInBound<uint64_t>(rule->rec_count(), 1, mp.items() ) );
	if (rule->distance_def() > 0 ) mp.set_distance_def( rule->distance_def() );
	if (rule->distance_band() > 0 ) mp.set_distance_band( rule->distance_band() );

	std::ostringstream extra;

	// input_type
	switch ( rule->input_type() ) {
	default:
		return false;
		break;
	case zpds::search::InputTypeE::I_QUERY:
		mp.set_last_partial(! mp.full_words() );
		break;
	}


	// limit_type
	switch ( rule->limit_type() ) {
	default:
	case zpds::search::LimitTypeE::L_NONE:
		break;
	case zpds::search::LimitTypeE::L_CCODE:
		extra << XAP_FORMAT_SPPL + FormatPrefix( ploc->ccode(), XAP_CCODE_PREFIX );
		break;
	case zpds::search::LimitTypeE::L_CITY:
		extra << XAP_FORMAT_SPPL + FormatPrefix( ploc->city() + ploc->ccode(), XAP_CITY_PREFIX );
		break;
	case zpds::search::LimitTypeE::L_PINCODE:
		extra << XAP_FORMAT_SPPL + FormatPrefix( ploc->pincode() + ploc->ccode(), XAP_PINCODE_PREFIX );
		break;
	case zpds::search::LimitTypeE::L_GEOHASH5:
		extra << XAP_FORMAT_SPPL + FormatPrefix( gh.Encode( ploc->lat(), ploc->lon(), 5), XAP_GEOHASH5_PREFIX );
		break;
	case zpds::search::LimitTypeE::L_GEOHASH7:
		extra << XAP_FORMAT_SPPL + FormatPrefix( gh.Encode( ploc->lat(), ploc->lon(), 7), XAP_GEOHASH7_PREFIX );
		break;
	case zpds::search::LimitTypeE::L_GEOHASH9:
		extra << XAP_FORMAT_SPPL + FormatPrefix( gh.Encode( ploc->lat(), ploc->lon(), 9), XAP_GEOHASH9_PREFIX );
		break;
	}

	// search_type
	switch ( rule->search_type() ) {
	default:
	case zpds::search::SearchTypeE::S_DEFAULT:
		break;
	case zpds::search::SearchTypeE::S_FULLWORD:
		mp.set_last_partial( false );
		break;
	case zpds::search::SearchTypeE::S_PARTIAL_BEGIN:
		mp.set_begin_with( true );
		break;
	case zpds::search::SearchTypeE::S_PARTIAL:
		mp.set_all_partial( true );
		break;
	case zpds::search::SearchTypeE::S_FIRSTWORD: {
		auto ss = Split( mp.query() );
		if ( ss.size() >0) {
			extra << XAP_FORMAT_SPPL;
			if ( ( mp.all_partial() ) || ( mp.last_partial() && ss.size()==1 ) )
				extra << XAP_BEGINPART_PREFIX;
			else
				extra << XAP_BEGINFULL_PREFIX;
			extra << ss.front();
		}
		break;
	}
		// end switch
	}

	// set the extra
	mp.set_extra( extra.str() );

	uint64_t currtime = ZPDS_CURRTIME_MS;
	// order_type
	switch ( rule->order_type() ) {
	default:
	case zpds::search::OrderTypeE::O_DEFAULT:
		FindFull( db, &mp, true );
		break;
	case zpds::search::OrderTypeE::O_DIST_BAND:
		FindNear( db, &mp, true );
		break;
	case zpds::search::OrderTypeE::O_DIST_ONLY:
		mp.set_distance_band ( 10 );
		FindNear( db, &mp, true );
		break;
	}

	constexpr double dsmax = XAP_DSLAB_MAX_IMPORTANCE * XAP_DSLAB_MAX_DISTANCE;

	if (mp.ids_size() != mp.sortkeys_size())
		throw ::zpds::BadCodeException("ids dont match scores");

	for (auto i =0 ; i < mp.ids_size() ; ++i ) {
		auto r = records->add_record();
		r->set_id(boost::lexical_cast<uint64_t>( db.get_document( mp.ids(i)).get_value( XAP_ROCKSID_POS )) );
		double sortkey = Xapian::sortable_unserialise ( mp.sortkeys(i));
		DLOG(INFO) << sortkey ;
		switch ( rule->order_type() ) {
		default:
		case zpds::search::OrderTypeE::O_DEFAULT:
			r->set_score( sortkey );
			break;
		case zpds::search::OrderTypeE::O_DIST_BAND:
		case zpds::search::OrderTypeE::O_DIST_ONLY:
			r->set_score( dsmax- sortkey );
			break;
		}

	}

	return true;
}

