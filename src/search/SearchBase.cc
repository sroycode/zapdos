/**
 * @project zapdos
 * @file src/search/SearchBase.cc
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
 *  SearchBase.cc : Xapian Search Abstract Base Class impl
 *
 */
#include "search/SearchBase.hpp"
#include <boost/lexical_cast.hpp>
#include "search/DistanceSlabKeyMaker.hpp"
#include "store/ExterCredService.hpp"

#include "utils/SplitWith.hpp"
#include "utils/PrintWith.hpp"
#include "utils/SortHelpers.hpp"
#include "search/GeoHashHelper.hpp"

const ::zpds::search::GeoHashHelper gh;
const ::zpds::store::ExterCredService ucserv;

/**
* SanitParams : sanitize user params
*
*/
void zpds::search::SearchBase::SanitParams(::zpds::search::UsedParamsT* params)
{
	for (auto i=0; i < params->filter_categories_size() ; ++i) {
		params->set_filter_categories( i, SanitNSLower( params->filter_categories(i) ) );
	}

	for (auto i=0; i < params->filter_tags_size() ; ++i) {
		auto tag = params->mutable_filter_tags(i);
		tag->set_name( SanitNSLower( tag->name() ) );
	}

}

/**
* EstimateExec: estimate execution time
*
*/
uint64_t zpds::search::SearchBase::EstimateExec(::zpds::search::SearchBase::DatabaseT& db, std::string& input)
{
	uint64_t est= XAP_CAN_BEGIN_THRESHOLD + 1;
	std::vector<std::string> arr = ::zpds::utils::SplitWith::Space( input );
	for (auto& word : arr) {
		if (word.at(0) != '+') continue;
		auto word_freq = db.get_termfreq( word.substr(1) );
		if (word_freq < est ) est = word_freq;
	}
	return est;
}


/**
* GetQuerySize : get size of current query
*
*/
size_t zpds::search::SearchBase::GetQuerySize()
{
	return wordstr.size();
}

/**
* SetQuery : reset the wordstr
*
*/
size_t zpds::search::SearchBase::SetQuery(const std::string& query_)
{
	wordstr = SplitMerge(query_);
	return wordstr.size();
}

/**
* GetQueryString: set query
*
*/
std::string zpds::search::SearchBase::GetQueryString(
    QueryWordsE qtype, const std::string& extra,
    const std::string& full_prefix, const std::string& part_prefix)
{

	std::ostringstream xtmp;
	for (int i=0; i<wordstr.size(); ++i) {
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
bool zpds::search::SearchBase::FindNear(::zpds::search::UsedParamsT* qr, bool reset)
{

	auto db = Get(qr->lang(), qr->dtyp() );
	auto wordstr_size = wordstr.size();
	if (qr->location().dont_use()) return false;
	if (reset) wordstr_size = SetQuery(qr->query());

	if ( (! qr->noname() ) && ( wordstr_size==0) ) return false;

	QueryWordsE qtype = ( qr->all_partial() ) ? ALL_PARTIAL : ( qr->last_partial() ) ? LAST_PARTIAL : FULL_WORDS;

	std::string full_prefix { XAP_FORMAT_PLUS };
	std::string part_prefix { XAP_FORMAT_PLUS XAP_PARTWORD_PREFIX };
	if ( qr->use_name() ) {
		full_prefix = std::string(XAP_FORMAT_PLUS XAP_NAMEFULL_PREFIX);
		part_prefix = std::string(XAP_FORMAT_PLUS XAP_NAMEPART_PREFIX);
	}

	std::ostringstream xtmp;
	xtmp << GetQueryString( qtype, qr->extra(), full_prefix, part_prefix);

	if ( qr->filter_categories_size() > 0 ) {
		for (auto i=0; i < qr->filter_categories_size() ; ++i) {
			xtmp << XAP_FORMAT_SPPL << XAP_CATEGORY_PREFIX << qr->filter_categories(i);
		}
	}

	if ( qr->filter_tags_size() > 0 ) {
		for (auto i=0; i < qr->filter_tags_size() ; ++i) {
			xtmp << XAP_FORMAT_SPPL << XAP_TAG_PREFIX << FormatTag(qr->filter_tags(i).name(), qr->filter_tags(i).value() );
		}
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

	Xapian::LatLongCoord centre( qr->location().lat(), qr->location().lon() );
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
* FindFull: check if exists
*
*/
bool zpds::search::SearchBase::FindFull(::zpds::search::UsedParamsT* qr,bool reset)
{

	auto db = Get(qr->lang(), qr->dtyp() );
	auto wordstr_size = wordstr.size();
	if (reset) wordstr_size = SetQuery(qr->query());
	if (wordstr_size==0) return false;

	QueryWordsE qtype = ( qr->all_partial() ) ? ALL_PARTIAL : ( qr->last_partial() ) ? LAST_PARTIAL : FULL_WORDS;

	std::string full_prefix { XAP_FORMAT_PLUS };
	std::string part_prefix { XAP_FORMAT_PLUS XAP_PARTWORD_PREFIX };
	if ( qr->use_name() ) {
		full_prefix = std::string(XAP_FORMAT_PLUS XAP_NAMEFULL_PREFIX);
		part_prefix = std::string(XAP_FORMAT_PLUS XAP_NAMEPART_PREFIX);
	}

	std::ostringstream xtmp;
	xtmp << GetQueryString( qtype, qr->extra(), full_prefix, part_prefix);

	if ( qr->filter_categories_size() > 0 ) {
		for (auto i=0; i < qr->filter_categories_size() ; ++i) {
			xtmp << XAP_FORMAT_SPPL << XAP_CATEGORY_PREFIX << qr->filter_categories(i);
		}
	}

	if ( qr->filter_tags_size() > 0 ) {
		for (auto i=0; i < qr->filter_tags_size() ; ++i) {
			xtmp << XAP_FORMAT_SPPL << XAP_TAG_PREFIX << FormatTag(qr->filter_tags(i).name(), qr->filter_tags(i).value() );
		}
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
			qr->add_ids( boost::lexical_cast<uint64_t>( m.get_document().get_value( XAP_ROCKSID_POS )) );
			qr->add_sortkeys( m.get_sort_key() );
		}
	}
	return (mset.size()>0);
}

/**
* RuleSearch : do the search from rule, populate ids to cresp
*
*/
bool zpds::search::SearchBase::RuleSearch (
    ::zpds::search::UsedParamsT* params, const ::zpds::search::QueryRuleT* rule)
{

	// items to populate
	params->set_items ( KeepInBound<uint64_t>( rule->max_records(), 1, params->items() ) );
	if (rule->distance_def() > 0 ) params->set_distance_def( rule->distance_def() );
	if (rule->distance_band() > 0 ) params->set_distance_band( rule->distance_band() );

	DLOG(INFO) << params->DebugString();

	std::ostringstream extra;

	// input_type
	switch ( rule->input_type() ) {
	default:
		return false;
		break;
	case zpds::search::InputTypeE::I_DEFAULT:
		break;
	case zpds::search::InputTypeE::I_ONEWORD:
		if ( params->no_of_words() > 1 ) return false;
		if ( rule->oneword_length()>0 && params->query().length() > rule->oneword_length() ) return false;
		break;
	}

	// limit_type
	switch ( rule->limit_type() ) {
	default:
	case zpds::search::LimitTypeE::L_NONE:
		break;
	case zpds::search::LimitTypeE::L_CCODE:
		if ( params->location().ccode().empty() ) return false;
		extra << XAP_FORMAT_SPPL + FormatPrefix( params->location().ccode(), XAP_CCODE_PREFIX );
		break;
	case zpds::search::LimitTypeE::L_CITY:
		if ( params->location().city().empty() || params->location().ccode().empty() ) return false;
		extra << XAP_FORMAT_SPPL + FormatPrefix( params->location().city() + params->location().ccode(), XAP_CITY_PREFIX );
		break;
	case zpds::search::LimitTypeE::L_PINCODE:
		if ( params->location().pincode().empty() || params->location().ccode().empty() ) return false;
		extra << XAP_FORMAT_SPPL + FormatPrefix( params->location().pincode() + params->location().ccode(), XAP_PINCODE_PREFIX );
		break;
	case zpds::search::LimitTypeE::L_GEOHASH3:
		if ( params->location().dont_use() ) return false;
		extra << XAP_FORMAT_SPPL + FormatPrefix( params->location().geohash().substr(0,3), XAP_GEOHASH3_PREFIX );
		break;
	case zpds::search::LimitTypeE::L_GEOHASH5:
		if ( params->location().dont_use() ) return false;
		extra << XAP_FORMAT_SPPL + FormatPrefix( params->location().geohash().substr(0,5), XAP_GEOHASH5_PREFIX );
		break;
	case zpds::search::LimitTypeE::L_GEOHASH7:
		if ( params->location().dont_use() ) return false;
		extra << XAP_FORMAT_SPPL + FormatPrefix( params->location().geohash().substr(0,7), XAP_GEOHASH7_PREFIX );
		break;
	case zpds::search::LimitTypeE::L_GEOHASH9:
		if ( params->location().dont_use() ) return false;
		extra << XAP_FORMAT_SPPL + FormatPrefix( params->location().geohash().substr(0,9), XAP_GEOHASH9_PREFIX );
		break;
	case zpds::search::LimitTypeE::L_NBRHASH3:
		if ( params->location().dont_use() ) return false;
		extra << XAP_FORMAT_SPPL + FormatPrefix( params->location().geohash().substr(0,3), XAP_NBRHASH3_PREFIX );
		break;
	case zpds::search::LimitTypeE::L_NBRHASH5:
		if ( params->location().dont_use() ) return false;
		extra << XAP_FORMAT_SPPL + FormatPrefix( params->location().geohash().substr(0,5), XAP_NBRHASH5_PREFIX );
		break;
	}

	// search_type
	switch ( rule->search_type() ) {
	default:
	case zpds::search::SearchTypeE::S_DEFAULT:
		break;
	case zpds::search::SearchTypeE::S_BEGINWITH: {
		auto ss = ::zpds::utils::SplitWith::Space ( params->query() );
		if ( ss.size() >0) {
			extra << XAP_FORMAT_SPPL;
			if ( ( params->all_partial() ) || ( params->last_partial() && ss.size()==1 ) )
				extra << XAP_BEGINPART_PREFIX;
			else
				extra << XAP_BEGINFULL_PREFIX;
			extra << ss.front();
		}
		break;
	}
	case zpds::search::SearchTypeE::S_PARTIAL:
		params->set_all_partial( true );
		break;
	case zpds::search::SearchTypeE::S_FULLWORD:
		params->set_last_partial( false );
		break;
	case zpds::search::SearchTypeE::S_NAME:
		params->set_use_name( true );
		break;

		// end switch
	}

	// set the extra
	params->set_extra( extra.str() );

	DLOG(INFO) << params->DebugString();

	uint64_t currtime = ZPDS_CURRTIME_MS;
	// order_type
	switch ( rule->order_type() ) {
	default:
	case zpds::search::OrderTypeE::O_DEFAULT:
		FindFull( params, true );
		break;
	case zpds::search::OrderTypeE::O_DIST_BAND:
		if (params->location().dont_use()) return false;
		FindNear( params, true );
		break;
	case zpds::search::OrderTypeE::O_DIST_ONLY:
		if (params->location().dont_use()) return false;
		params->set_distance_band ( 10 );
		FindNear( params, true );
		break;
	}

	constexpr double dsmax = XAP_DSLAB_MAX_IMPORTANCE * XAP_DSLAB_MAX_DISTANCE;

	if (params->ids_size() != params->sortkeys_size())
		throw ::zpds::BadCodeException("ids dont match sortkeys");

	for (auto i =0 ; i < params->ids_size() ; ++i ) {
		double sortkey = Xapian::sortable_unserialise ( params->sortkeys(i));
		DLOG(INFO) << sortkey ;
		switch ( rule->order_type() ) {
		default:
		case zpds::search::OrderTypeE::O_DEFAULT:
			params->add_scores( sortkey );
			break;
		case zpds::search::OrderTypeE::O_DIST_BAND:
		case zpds::search::OrderTypeE::O_DIST_ONLY:
			params->add_scores( dsmax- sortkey );
			break;
		}

	}

	if (params->ids_size() != params->scores_size())
		throw ::zpds::BadCodeException("ids dont match scores");

	return true;
}

/**
* GetExterProfile : get the exter profile
*
*/
bool zpds::search::SearchBase::GetExterProfile(
    ::zpds::utils::SharedTable::pointer stptr,
    const std::string& exter,
    ::zpds::search::QueryProfT* profile)
{
	::zpds::store::UserDataT user;
	user.set_name(exter);
	ucserv.Get(stptr, &user);
	if (user.notfound() ) return false;
	for (auto i = 0 ; i < user.profiles_size() ; ++i) {
		if ( user.profiles(i).query_type() == profile->query_type() ) {
			profile->Swap( user.mutable_profiles(i) );
			return true;
		}
	}
	return false;
}

/**
* SearchByProfile : executes basic rule Query
*
*/
void zpds::search::SearchBase::SearchByProfile(
    ::zpds::utils::SharedTable::pointer stptr,
    ::zpds::search::QueryProfT* qprof,
    ::zpds::query::SearchCompletionRespT* resp)
{
	auto qr = resp->mutable_cdata();

	if (! GetExterProfile( stptr, resp->cdata().profile(), qprof) )
		throw ::zpds::BadDataException("This search profile does not exist");

	auto cur = qr->mutable_location();

	if ( ! cur->dont_use() ) {
		cur->set_geohash( gh.Encode( cur->lat(), cur->lon(), 9) );
	}

	const uint64_t counter = KeepInBound<uint64_t>(qr->items(), 1, 100);
	qr->set_items( counter );

	std::string corrected;

	if ( ! qr->noname() ) {
		// set last partial
		qr->set_last_partial( qr->raw_query().back() != ' ' );
		// set full words if not already set
		if ( ! qr->full_words() ) qr->set_full_words(! qr->last_partial() );
		std::string q ;
		size_t wc =0 ;
		std::tie(q,wc ) = FlattenCount( qr->raw_query() );
		auto pos = q.find_last_of(XAP_FORMAT_SPACE);
		qr->set_no_of_words( wc );
		// set corrected
		if ( qr->full_words() ) {
			corrected = stptr->jamdb->Correct(qr->lang(), q);
		}
		else if (pos != std::string::npos) {
			corrected = stptr->jamdb->Correct(qr->lang(), q.substr(0,pos)) + q.substr(pos);
		}
		else {
			// dont correct anything if one word partial
		}
		if (q==corrected) corrected.clear();
		qr->set_query( StemQuery( q, (!qr->full_words()) ));
		if ( qr->no_of_words() == 0 ) return;
	}

	uint64_t rule_weight = 0;
	std::unordered_set<uint64_t> idset;
	std::map<std::string, uint64_t> idmap;

	// rules are sorted
	// repeat the search for corrected if not found
	for (auto secondq : std::vector<bool> { false, true } ) {

		if ( secondq ) {
			if ( corrected.empty() || ( idset.size() > 0 ) ) break;
			qr->set_query( StemQuery( corrected, (!qr->full_words()) ));
		}

		for (auto i = 0 ; i < qprof->rules_size() ; ++i ) {
			auto rule = qprof->mutable_rules(i);
			if (rule_weight > rule->weight() && idset.size() >= counter)
				break;
			auto nqr =*qr;
			bool status = RuleSearch(&nqr, rule);
			for (auto j = 0 ; j < nqr.ids_size() ; ++j ) {
				if (idset.find( nqr.ids(j) ) != idset.end() ) continue;
				idset.emplace( nqr.ids(j) );
				idmap.emplace(
				    EncodeSortKey<uint64_t, double, uint64_t>( rule->weight(), nqr.scores(j), nqr.ids(j) ),
				    nqr.ids(j) );
			}
			rule_weight = rule->weight();
		}

	}

	// populate from db
	auto ncounter = counter;
	auto cresp = resp->mutable_cresp();
	for (auto it = idmap.rbegin() ; it != idmap.rend() ; ++it ) {
		cresp->add_records()->set_id( it->second );
	}
}
