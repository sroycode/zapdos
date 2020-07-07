/**
 * @project zapdos
 * @file src/search/SearchLocal.cc
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
 *  SearchLocal.cc : Xapian Search LocalData impl
 *
 */
#include <unordered_set>
#include <unordered_map>
#include <map>
#include <regex>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#include "search/SearchLocal.hpp"
#include "utils/SplitWith.hpp"
#include "utils/PrintWith.hpp"

#include "store/LocalDataService.hpp"

const ::zpds::store::LocalDataService ldserv;
const std::regex bbox_regex("BOX\\(([0-9\\-\\.]*) ([0-9\\-\\.]*),([0-9\\-\\.]*) ([0-9\\-\\.]*)\\)");

/**
* Constructor : with query
*
*/
zpds::search::SearchLocal::SearchLocal(const std::string& dbpath_)
	: ReadIndex(dbpath_)
{
}

/**
* destructor
*/
zpds::search::SearchLocal::~SearchLocal () {}

/**
* CompletionQueryAction : simple query service action
*
*/
void zpds::search::SearchLocal::CompletionQueryAction(::zpds::utils::SharedTable::pointer stptr, ::zpds::query::SearchCompletionRespT* resp)
{
	auto qr = resp->mutable_cdata();
	const uint64_t counter = KeepInBound<uint64_t>(qr->items(), 1, 100);
	::zpds::search::QueryProfT qprof;

	// qr->set_dtyp( ::zpds::search::IndexTypeE::I_LOCALDATA );

	if ( qr->noname() )
	{
		qprof.set_query_type( ::zpds::search::QueryTypeE::QRY_COMPLETION_NOTOPH );
	}
	else
	{
		qprof.set_query_type( ::zpds::search::QueryTypeE::QRY_COMPLETION_PHOTON );
	}

	SanitParams( resp->mutable_cdata() );

	SearchByProfile(stptr, &qprof, resp);

	// populate result
	ldserv.GetManyRecords(stptr, resp->mutable_cresp()->mutable_records() );

	auto pdata = resp->mutable_photondata();
	auto cresp = resp->mutable_cresp();

	pdata->set_type( "FeatureCollection" );
	size_t populated = 0;
	for (auto i = 0 ; i < cresp->records_size() ; ++i )
	{
		if ( cresp->records(i).notfound() ) continue; // not probable
		auto feat = pdata->add_features() ;
		feat->set_type( "Feature" );

		auto prop = feat->mutable_properties();
		prop->set_osm_id( cresp->records(i).osm_id() );
		prop->set_osm_key( cresp->records(i).osm_key() );
		prop->set_osm_value( cresp->records(i).osm_value() );
		prop->set_osm_type( cresp->records(i).osm_type() );
		prop->set_city( cresp->records(i).city() );
		prop->set_country( cresp->records(i).country() );
		prop->set_state( cresp->records(i).state() );
		prop->set_name( ::zpds::utils::PrintWithComma::String( cresp->records(i).fld_name(), cresp->records(i).fld_area() ) );
		prop->set_address( cresp->records(i).address() );
		prop->set_postcode( cresp->records(i).pincode() );
		prop->set_area( cresp->records(i).fld_area() );

		// TODO : extents
		if ( ! cresp->records(i).geometry().empty() ) {
			try {
				std::smatch sm;
				if ( std::regex_search( cresp->records(i).geometry(), sm, bbox_regex) ) {
					if (sm.size()==5) {
						prop->add_extent( boost::lexical_cast<double>(sm.str(1) ) );
						prop->add_extent( boost::lexical_cast<double>(sm.str(2) ) );
						prop->add_extent( boost::lexical_cast<double>(sm.str(3) ) );
						prop->add_extent( boost::lexical_cast<double>(sm.str(4) ) );
					}
				}
			}
			catch (std::exception& e) {}
		}

		auto geom = feat->mutable_geometry();
		geom->set_type( "Point" );
		geom->add_coordinates( cresp->records(i).lon() );
		geom->add_coordinates( cresp->records(i).lat() );

		if (++populated >= counter) break;
	}
}
