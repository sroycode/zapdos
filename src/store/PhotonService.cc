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

	if ( ! qr->full_words() ) qr->set_full_words( qr->raw_query().back() == ' ' );

	::zpds::search::SearchTrie trie( stptr->xapath.Get() );

	{
		std::string q ;
		size_t wc =0 ;
		std::tie(q,wc ) = FlattenCount( qr->raw_query() );
		auto pos = q.find_last_of(" ");
		if (pos == std::string::npos) {
			q = stptr->jamdb->Correct(qr->lang() , q);
		}
		else {
			q = stptr->jamdb->Correct(qr->lang() , q.substr(0,pos)) + q.substr(pos);
		}
		qr->set_no_of_words( wc );
		qr->set_query( trie.StemQuery( q, (!qr->full_words()) ));
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
			bool status = trie.RuleSearch( qr, rule, &reclist, counter);
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
	size_t populated = 0;
	for ( auto it = smap.rbegin() ; it != smap.rend() ; ++it) {
		it->second->Swap( cresp->add_record() );
		if (++populated >= counter) break;
	}

}
