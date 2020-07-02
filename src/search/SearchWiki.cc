/**
 * @project zapdos
 * @file src/search/SearchWiki.cc
 * @author  S Roychowdhury < sroycode at gmail dot com >
 * @version 1.0.1
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
 *  SearchWiki.cc : Xapian Search WikiData impl
 *
 */
#include <unordered_set>
#include <unordered_map>
#include <map>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#include "search/SearchWiki.hpp"
#include "utils/SplitWith.hpp"
#include "utils/PrintWith.hpp"

#include "store/WikiDataService.hpp"

const ::zpds::store::WikiDataService wdserv;

/**
* Constructor : with query
*
*/
zpds::search::SearchWiki::SearchWiki(const std::string& dbpath_)
	: ReadIndex(dbpath_)
{
}

/**
* destructor
*/
zpds::search::SearchWiki::~SearchWiki () {}

/**
* CompletionQueryAction : simple query service action
*
*/
void zpds::search::SearchWiki::CompletionQueryAction(::zpds::utils::SharedTable::pointer stptr, ::zpds::query::SearchCompletionRespT* resp)
{
	auto qr = resp->mutable_cdata();
	// qr->set_dtyp( ::zpds::search::IndexTypeE::I_WIKIDATA );
	const uint64_t counter = KeepInBound<uint64_t>(qr->items(), 1, 100);

	::zpds::search::QueryProfT qprof;

	qprof.set_query_type( ::zpds::search::QueryTypeE::QRY_COMPLETION_WIKIDATA );

	SanitParams( resp->mutable_cdata() );

	SearchByProfile(stptr, &qprof, resp);

	// populate result
	wdserv.GetManyRecords(stptr, resp->mutable_cresp()->mutable_records() );

	DLOG(INFO) << resp->DebugString();

	auto cresp = resp->mutable_cresp();

	size_t populated = 0;
	for (auto i = 0 ; i < cresp->records_size() ; ++i )
	{
		if ( cresp->records(i).notfound() ) continue; // not probable
		auto wdata = resp->mutable_wikidata()->add_records();
		wdata->set_title( cresp->records(i).title() );
		wdata->set_summary( cresp->records(i).summary() );
		wdata->set_unique_id( cresp->records(i).unique_id() );
		if (++populated >= counter) break;
	}
}
