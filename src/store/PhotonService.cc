/**
 * @project zapdos
 * @file src/store/PhotonService.cc
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
 *  PhotonService.cc : Implementation of Photon Service
 *
 */

#include <unordered_map>
#include <functional>
#include <sparsepp/spp.h>
#include <boost/utility/binary.hpp>

#include "async++.h"
#include "store/PhotonService.hpp"
#include "store/SimpleTemplateTable.hpp"
#include "remote/GeoHashHelper.hpp"

#include "search/SearchTrie.hpp"
#include "utils/StringHelpers.hpp"
#include "utils/SortHelpers.hpp"

#include "store/LookupRecordTable.hpp"
#include "search/DistanceSlabKeyMaker.hpp"

/**
* Constructor
*
*/
zpds::store::PhotonService::PhotonService(::zpds::utils::SharedTable::pointer stptr_)
	: stptr(stptr_)
{
}

/**
* Destructor
*
*/
zpds::store::PhotonService::~PhotonService()
{
}

// shreos wip

/**
* RuleSearch : do the search from rule
*
*/
bool zpds::store::PhotonService::RuleSearch (
    ::zpds::search::UsedParamsT mp,
    ::zpds::search::QueryOrderT rule,
    ::zpds::store::LookupRecordListT* records,
    size_t target) const
{

	::zpds::search::SearchTrie trie( stptr->xapath.Get() );
	auto ploc = mp.mutable_cur();

	// items to populate
	mp.set_items ( KeepInBound<uint64_t>( (rule.rec_count() >0 ? rule.rec_count() : 10 ), 1, mp.items() ) );
	if (rule.distance_def() > 0 ) mp.set_distance_def( rule.distance_def() );
	if (rule.distance_band() > 0 ) mp.set_distance_band( rule.distance_band() );

	DLOG(INFO) << mp.DebugString();

	std::ostringstream extra;

	// input_type
	switch ( rule.input_type() ) {
	default:
		return false;
		break;
	case zpds::search::InputTypeE::I_DEFAULT:
		break;
	case zpds::search::InputTypeE::I_ONEWORD:
		if ( mp.no_of_words() > 1 ) return false;
		if ( rule.oneword_length()>0 && mp.query().length() > rule.oneword_length() ) return false;
		break;
	}


	// limit_type
	switch ( rule.limit_type() ) {
	default:
	case zpds::search::LimitTypeE::L_NONE:
		break;
	case zpds::search::LimitTypeE::L_CCODE:
		if ( ploc->ccode().empty() ) return false;
		extra << XAP_FORMAT_SPPL + FormatPrefix( ploc->ccode(), XAP_CCODE_PREFIX );
		break;
	case zpds::search::LimitTypeE::L_CITY:
		if ( ploc->city().empty() || ploc->ccode().empty() ) return false;
		extra << XAP_FORMAT_SPPL + FormatPrefix( ploc->city() + ploc->ccode(), XAP_CITY_PREFIX );
		break;
	case zpds::search::LimitTypeE::L_PINCODE:
		if ( ploc->pincode().empty() || ploc->ccode().empty() ) return false;
		extra << XAP_FORMAT_SPPL + FormatPrefix( ploc->pincode() + ploc->ccode(), XAP_PINCODE_PREFIX );
		break;
	case zpds::search::LimitTypeE::L_GEOHASH3:
		if ( ploc->dont_use() ) return false;
		extra << XAP_FORMAT_SPPL + FormatPrefix( ploc->geohash().substr(0,3), XAP_GEOHASH3_PREFIX );
		break;
	case zpds::search::LimitTypeE::L_GEOHASH5:
		if ( ploc->dont_use() ) return false;
		extra << XAP_FORMAT_SPPL + FormatPrefix( ploc->geohash().substr(0,5), XAP_GEOHASH5_PREFIX );
		break;
	case zpds::search::LimitTypeE::L_GEOHASH7:
		if ( ploc->dont_use() ) return false;
		extra << XAP_FORMAT_SPPL + FormatPrefix( ploc->geohash().substr(0,7), XAP_GEOHASH7_PREFIX );
		break;
	case zpds::search::LimitTypeE::L_GEOHASH9:
		if ( ploc->dont_use() ) return false;
		extra << XAP_FORMAT_SPPL + FormatPrefix( ploc->geohash().substr(0,9), XAP_GEOHASH9_PREFIX );
		break;
	case zpds::search::LimitTypeE::L_NBRHASH3:
		if ( ploc->dont_use() ) return false;
		extra << XAP_FORMAT_SPPL + FormatPrefix( ploc->geohash().substr(0,3), XAP_NBRHASH3_PREFIX );
		break;
	case zpds::search::LimitTypeE::L_NBRHASH5:
		if ( ploc->dont_use() ) return false;
		extra << XAP_FORMAT_SPPL + FormatPrefix( ploc->geohash().substr(0,5), XAP_NBRHASH5_PREFIX );
		break;
	}

	// search_type
	switch ( rule.search_type() ) {
	default:
	case zpds::search::SearchTypeE::S_DEFAULT:
		break;
	case zpds::search::SearchTypeE::S_FULLWORD:
		mp.set_last_partial( false );
		break;
	case zpds::search::SearchTypeE::S_BEGIN:
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

	DLOG(INFO) << mp.DebugString();

	uint64_t currtime = ZPDS_CURRTIME_MS;
	// order_type
	switch ( rule.order_type() ) {
	default:
	case zpds::search::OrderTypeE::O_DEFAULT:
		trie.FindFull( &mp, true );
		break;
	case zpds::search::OrderTypeE::O_DIST_BAND:
		if (mp.cur().dont_use()) return false;
		trie.FindNear( &mp, true );
		break;
	case zpds::search::OrderTypeE::O_DIST_ONLY:
		if (mp.cur().dont_use()) return false;
		mp.set_distance_band ( 10 );
		trie.FindNear( &mp, true );
		break;
	}

	constexpr double dsmax = XAP_DSLAB_MAX_IMPORTANCE * XAP_DSLAB_MAX_DISTANCE;

	if (mp.ids_size() != mp.sortkeys_size())
		throw ::zpds::BadCodeException("ids dont match scores");

	for (auto i =0 ; i < mp.ids_size() ; ++i ) {
		auto r = records->add_record();
		r->set_id( mp.ids(i) );
		double sortkey = Xapian::sortable_unserialise ( mp.sortkeys(i));
		DLOG(INFO) << sortkey ;
		switch ( rule.order_type() ) {
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

/**
* GetCompleteAction : get completion wip
*
*/
void zpds::store::PhotonService::GetCompleteAction (::zpds::query::PhotonParamsT* params)
{

	auto qr = params->mutable_cdata();
	auto cur = qr->mutable_cur();

	::zpds::store::SimpleTemplateT one_t;
	one_t.set_qtyp ( ::zpds::search::QRY_COMPLETION_PHOTON );
	one_t.set_name( qr->name() );
	if (! GetSimpleTemplate(stptr, &one_t) )
		throw ::zpds::BadDataException("No QRY_COMPLETION_PHOTON Template for " + qr->name() );
	if ( one_t.ignore() )
		throw ::zpds::BadDataException("Ignored QRY_COMPLETION_PHOTON Template for " + qr->name() );

	// start housekeeping
	if ( ! cur->dont_use() ) {
		::zpds::remote::GeoHashHelper gh;
		cur->set_geohash( gh.Encode( cur->lat(), cur->lon(), 9) );
	}

	const uint64_t counter = KeepInBound<uint64_t>(qr->items(), 1, 100);
	qr->set_items( counter );

	// set last partial
	qr->set_last_partial( qr->raw_query().back() != ' ' );
	// set full words if not already set
	if ( ! qr->full_words() ) qr->set_full_words(! qr->last_partial() );


	{
		std::string q ;
		size_t wc =0 ;
		std::tie(q,wc ) = FlattenCount( qr->raw_query() );
		auto pos = q.find_last_of(" ");
		if ( qr->full_words() ) {
			q = stptr->jamdb->Correct(qr->lang() , q);
		} else if (pos != std::string::npos) {
			q = stptr->jamdb->Correct(qr->lang() , q.substr(0,pos)) + q.substr(pos);
		} else {
			// dont correct anything if one word partial
		}
		qr->set_no_of_words( wc );
		qr->set_query( StemQuery( q, (!qr->full_words()) ));
		DLOG(INFO) << q << " | " << qr->query();
	}

	if ( qr->no_of_words() == 0 ) return;

	using TagOrderMapT = std::map< uint64_t, ::zpds::search::QueryOrderT* >;
	using RecordListMapT = std::unordered_map< std::string, ::zpds::store::LookupRecordListT >;
	using RecPtrMapT = std::map< std::string, ::zpds::store::LookupRecordT* >;

	uint64_t xcount =0;

	TagOrderMapT tormap;
	RecordListMapT recmap;
	RecPtrMapT smap, donemap;

	DLOG(INFO) << qr->DebugString();

	uint64_t rule_weight = 0;

	// merge rules by weight n order
	auto qprof = one_t.mutable_qprof();
	for (auto i = 0 ; i < qprof->orders_size() ; ++i ) {
		auto rule = qprof->mutable_orders(i);
		auto calc = (rule->weight() * qprof->orders_size()) + (qprof->orders_size() - i);
		tormap[calc] = rule;
		DLOG(INFO) << "rule weight=" << rule->weight() << " , calc=" << calc << ", i=" << i;
		// populate rule_weight
		rule_weight = (rule->weight() > rule_weight ) ? rule->weight() : rule_weight ;
	}

	::zpds::store::LookupRecordListT reclist;
	::zpds::store::LookupRecordTable sth_table{stptr->maindb.Get()};

	for (auto it = tormap.rbegin() ; it != tormap.rend() ; ++it ) {

		auto calc = it->first;
		auto rule = it->second;
		DLOG(INFO) << "Processing: " << calc << ", weight=" << rule->weight() << " , xcount=" << xcount;

		// sanity checks
		if ( (rule->weight()==0) || (rule_weight != rule->weight() && xcount >= counter ) ) {
			DLOG(INFO) << "Breaking on rule weight=" << rule->weight() << " , xcount=" << xcount;
			break;
		}

		// populate rec if not exists
		if ( recmap.find( rule->rec_tagid() ) == recmap.end() ) {
			reclist.Clear();
			bool status = RuleSearch( *qr, *rule, &reclist, counter);
			// populate from local
			for (auto rid = 0 ; rid < reclist.record_size() ; ++ rid ) {
				auto score = reclist.record(rid).score();
				if (! sth_table.GetOne(reclist.mutable_record(rid),K_LOOKUPRECORD) ) {
					reclist.mutable_record(rid)->set_notfound(true);
					continue;
				}
				reclist.mutable_record(rid)->set_score( score );
			}

			recmap[ rule->rec_tagid() ].Swap( &reclist);
		}

		if (rule->weight() == 0) continue;

		// now populate the main
		auto& whatlist = recmap[ rule->rec_tagid() ];
		for (auto i = 0 ; i < whatlist.record_size() ; ++i) {
			auto mwhat = whatlist.mutable_record(i);

			// check if key exists
			std::string mkey = EncodeSortKey<int32_t, std::string>( mwhat->styp(), mwhat->uniqueid()  );
			auto it = donemap.find(mkey);
			if ( it != donemap.end()) {
				it->second->add_matched_queries( rule->rec_tagid() );
				continue;
			}

			// check if alias key exists
			if ( mwhat->alias_styp() != ::zpds::search::SourceTypeE::S_NONE ) {
				std::string nkey = EncodeSortKey<int32_t, std::string>( mwhat->alias_styp(), mwhat->alias_uniqueid()  );
				it = donemap.find(nkey);
				if ( it != donemap.end()) {
					it->second->add_matched_queries( rule->rec_tagid() );
					continue;
				}
			}

			++xcount;
			std::string usekey = EncodeSortKey( rule->weight(), mwhat->score(), DR_BUFF_MAX-xcount );
			mwhat->add_matched_queries( rule->rec_tagid() );

			smap.emplace( std::move(usekey), mwhat );
			donemap.emplace( std::move(mkey), mwhat );
		}
		// set the rule weight
		rule_weight = rule->weight();

		// over

	}

	// populate result
	auto cresp = params->mutable_cresp();
	cresp->set_type( "FeatureCollection" );
	size_t populated = 0;
	for ( auto it = smap.rbegin() ; it != smap.rend() ; ++it) {
		auto feat = cresp->add_features() ;
		feat->set_type( "Feature" );

		auto prop = feat->mutable_properties();
		prop->set_osm_id( it->second->osm_id() );
		prop->set_osm_key( it->second->osm_key() );
		prop->set_osm_value( it->second->osm_value() );
		prop->set_osm_type( it->second->osm_type() );
		prop->set_city( it->second->city() );
		prop->set_country( it->second->country() );
		prop->set_state( it->second->state() );
		prop->set_name( PrintWithComma( it->second->fld_name() , it->second->fld_area() ) );
		prop->set_address( it->second->address() );
		prop->set_postcode( it->second->pincode() );

		// TODO : extents

		auto geom = feat->mutable_geometry();
		geom->set_type( "Point" );
		geom->add_coordinates( it->second->lon() );
		geom->add_coordinates( it->second->lat() );

		if (++populated >= counter) break;
	}

}
