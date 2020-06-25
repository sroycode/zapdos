/**
 * @project zapdos
 * @file src/search/IndexWiki.cc
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
 *  IndexWiki.cc : Xapian Indexing for WikiData impl
 *
 */
#include <cmath>

#include <boost/lexical_cast.hpp>
#include "search/DistanceSlabKeyMaker.hpp"
#include "utils/SplitWith.hpp"
#include "utils/PrintWith.hpp"
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>

#include "search/IndexWiki.hpp"


/**
 * Constructor : default private
 *
 */
zpds::search::IndexWiki::IndexWiki() {}

/**
 * Destructor : default
 *
 */
zpds::search::IndexWiki::~IndexWiki() {}


/**
* CreateDoc : create a new doc using ItemDataT
*
*/
Xapian::Document zpds::search::IndexWiki::CreateDoc(::zpds::store::ItemDataT* record) const
{
	Xapian::Document doc;
	const std::string blank;
	bool with_stem = (record->lang()==::zpds::search::LangTypeE::EN);

	// process suggest field
	std::string suggest = ::zpds::utils::PrintWithSpace::String(
	                          record->title(), record->summary(), record->city(), record->country() ) ;
	IndexWithPart( doc, suggest, blank, XAP_PARTWORD_PREFIX, with_stem, false);

	// process title field
	if (!record->title().empty() ) {
		IndexWithPart( doc, record->title(), XAP_NAMEFULL_PREFIX, XAP_NAMEPART_PREFIX, with_stem, false);
	}

	// process firstword field
	{
		auto wvec = ::zpds::utils::SplitWith::Space(record->title());

		if (wvec.size()>0)
			IndexWithPart( doc, wvec.at(0), XAP_BEGINFULL_PREFIX, XAP_BEGINPART_PREFIX, with_stem, false);
	}


	// categories
	if (!record->categories().empty() ) {
		for (auto i=0; i < record->categories_size() ; ++i) {
			IndexOneWord ( doc , record->categories(i), XAP_CATEGORY_PREFIX);
		}
	}

	// tags
	if (!record->tags().empty() ) {
		for (auto i=0; i < record->tags_size() ; ++i) {
			if ( ! record->tags(i).is_searchable() ) continue;
			IndexOneWord ( doc , FormatTag( record->tags(i).name(), record->tags(i).value() ), XAP_TAG_PREFIX);
		}
	}

	// add pincode, city and country prefix if present
	if (! (record->city().empty() || record->ccode().empty() ))
		doc.add_term( FormatPrefix( record->city() +record->ccode(), XAP_CITY_PREFIX ) );
	if (!record->ccode().empty() )
		doc.add_term( FormatPrefix( record->ccode(), XAP_CCODE_PREFIX ) );

	// geo if exists
	IndexGeo ( doc, record->lat(), record->lon() );

	std::string idterm = "Q" + std::to_string(record->id());
	doc.add_boolean_term(idterm);

	doc.add_value(XAP_IMPORTANCE_POS, Xapian::sortable_serialise(record->importance()));
	doc.add_value(XAP_ROCKSID_POS, std::to_string(record->id()));
	doc.add_value(XAP_UNIQUEID_POS, record->unique_id());

	return doc;
}

/**
* GetIndexType : get the index type
*
*/
zpds::search::IndexTypeE zpds::search::IndexWiki::GetIndexType() const
{
	return ::zpds::search::IndexTypeE::I_WIKIDATA;
}
