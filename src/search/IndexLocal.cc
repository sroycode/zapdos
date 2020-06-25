/**
 * @project zapdos
 * @file src/search/IndexLocal.cc
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
 *  IndexLocal.cc : Xapian Indexing for LocalData impl
 *
 */
#include <cmath>

#include "search/IndexLocal.hpp"
#include <boost/lexical_cast.hpp>
#include "search/DistanceSlabKeyMaker.hpp"
#include "utils/SplitWith.hpp"
#include "utils/PrintWith.hpp"
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>

/**
 * Constructor : default private
 *
 */
zpds::search::IndexLocal::IndexLocal() {}

/**
 * Destructor : default
 *
 */
zpds::search::IndexLocal::~IndexLocal() {}


/**
* CreateDoc : create a new doc using ItemDataT
*
*/
Xapian::Document zpds::search::IndexLocal::CreateDoc(::zpds::store::ItemDataT* record) const
{
	uint64_t currtime = ZPDS_CURRTIME_MS;
	Xapian::Document doc;
	const std::string blank;
	bool with_stem = (record->lang()==::zpds::search::LangTypeE::EN);

	// process suggest field
	std::string suggest = ::zpds::utils::PrintWithComma::String(
	                          record->fld_name(), record->fld_area(), record->pincode(), record->city(), record->country() ) ;
	DLOG(INFO) << " After Suggest creation ms: " << ZPDS_CURRTIME_MS - currtime;
	IndexWithPart( doc, suggest, blank, XAP_PARTWORD_PREFIX, with_stem, false);

	DLOG(INFO) << " After Suggest ms: " << ZPDS_CURRTIME_MS - currtime;

	// process name field
	if (!record->fld_name().empty() ) {
		IndexWithPart( doc, record->fld_name(), XAP_NAMEFULL_PREFIX, XAP_NAMEPART_PREFIX, with_stem, false);
	}

	DLOG(INFO) << " After Name ms: " << ZPDS_CURRTIME_MS - currtime;
	// process firstword field
	{
		auto wvec = ::zpds::utils::SplitWith::Space(record->fld_name());
		if (wvec.size()>0)
			IndexWithPart( doc, wvec.at(0), XAP_BEGINFULL_PREFIX, XAP_BEGINPART_PREFIX, with_stem, false);
	}

	DLOG(INFO) << " After Firstword  ms: " << ZPDS_CURRTIME_MS - currtime;

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

	// is_in_place

	// add pincode, city and country prefix
	if (! (record->pincode().empty() || record->ccode().empty() ))
		doc.add_term( FormatPrefix( record->pincode() +record->ccode(), XAP_PINCODE_PREFIX ) );
	if (! (record->city().empty() || record->ccode().empty() ))
		doc.add_term( FormatPrefix( record->city() +record->ccode(), XAP_CITY_PREFIX ) );
	if (!record->ccode().empty() )
		doc.add_term( FormatPrefix( record->ccode(), XAP_CCODE_PREFIX ) );

	IndexGeo ( doc, record->lat(), record->lon() );

	std::string idterm = "Q" + std::to_string(record->id());
	doc.add_boolean_term(idterm);

	doc.add_value(XAP_IMPORTANCE_POS, Xapian::sortable_serialise(record->importance()));
	doc.add_value(XAP_ROCKSID_POS, std::to_string(record->id()));
	doc.add_value(XAP_UNIQUEID_POS, record->unique_id());

	DLOG(INFO) << " Before Write ms: " << ZPDS_CURRTIME_MS - currtime;

	// write
	return doc; 
}

/**
* GetIndexType : get the index type
*
*/
zpds::search::IndexTypeE zpds::search::IndexLocal::GetIndexType() const
{
	return ::zpds::search::IndexTypeE::I_LOCALDATA;
}

