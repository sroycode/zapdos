/**
 * @project zapdos
 * @file src/search/IndexBase.cc
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
 *  IndexBase.cc : Xapian Indexing Abstract Base Class impl
 *
 */
#include <cmath>

#include "search/IndexBase.hpp"
#include "utils/SplitWith.hpp"
#include "search/GeoHashHelper.hpp"

const ::zpds::search::GeoHashHelper gh;

/**
* IndexFullWords : index items with full words
*
*/
void zpds::search::IndexBase::IndexFullWords (Xapian::Document& doc,
        const std::string item, const std::string full_prefix, bool with_stem, bool zero_pos) const
{
	auto wvec = ::zpds::utils::SplitWith::LowerNoQuote(item);
	for (auto i=0; i< wvec.size(); ++i) {
		int pos = (zero_pos) ? 0 : i+1;
		// default
		doc.add_posting(full_prefix+wvec.at(i), pos);

		// handle stem for full word
		if (with_stem && ( wvec.at(i).length() >= XAP_MIN_STEM_LEN )) {
			std::string stem_word = StemWord(wvec.at(i));
			if ( wvec.at(i) != stem_word) {
				doc.add_posting(full_prefix+stem_word, pos);
			}
		}

		// handle shingles with repeat words
		if (i>0) {
			if ( ( wvec.at(i) == wvec.at(i-1)) || ( wvec.at(i-1).length() <= XAP_MAX_SHIN_LEN ) ) {
				doc.add_posting(full_prefix + wvec.at(i-1)+wvec.at(i), pos);
			}
		}

	}
}

/**
* IndexOneWord : index item as single word
*
*/
void zpds::search::IndexBase::IndexOneWord (Xapian::Document& doc,
        const std::string item, const std::string full_prefix) const
{
	doc.add_posting( full_prefix + item, 0 );
}

/**
* IndexWithPart : index items with part
*
*/
void zpds::search::IndexBase::IndexWithPart (
    Xapian::Document& doc, const std::string item,
    const std::string full_prefix, const std::string part_prefix, bool with_stem, bool zero_pos) const
{
	auto wvec = ::zpds::utils::SplitWith::LowerNoQuote(item);
	for (auto i=0; i< wvec.size(); ++i) {
		int pos = (zero_pos) ? 0 : i+1;
		// default
		doc.add_posting(full_prefix+wvec.at(i), pos);

		// handle stem for full word
		if (with_stem && ( wvec.at(i).length() >= XAP_MIN_STEM_LEN )) {
			std::string stem_word = StemWord(wvec.at(i));
			if ( wvec.at(i) != stem_word) {
				doc.add_posting(full_prefix+stem_word, pos);
			}
		}

		// handle shingles with repeat words
		if (i>0) {
			if ( ( wvec.at(i) == wvec.at(i-1)) || ( wvec.at(i-1).length() <= XAP_MAX_SHIN_LEN ) ) {
				doc.add_posting(full_prefix + wvec.at(i-1)+wvec.at(i), pos);
			}
		}

		// handle partial words
		for (auto j=0; j< wvec.at(i).length() ; ++j) {
			doc.add_posting(part_prefix + wvec.at(i).substr(0,j+1), pos);
		}
	}
}

/**
* IndexGeo : index geo
*
*/
void zpds::search::IndexBase::IndexGeo(Xapian::Document& doc, const double lat, const double lon) const
{
	// add geohash ignore if throw
	if ( lat !=0.0 || lon !=0.0 ) {
		try {
			std::string hash = gh.Encode( lat, lon, 9);

			// exact geohash
			doc.add_term( FormatPrefix( hash.substr(0,3), XAP_GEOHASH3_PREFIX ) );
			doc.add_term( FormatPrefix( hash.substr(0,5), XAP_GEOHASH5_PREFIX ) );
			doc.add_term( FormatPrefix( hash.substr(0,7), XAP_GEOHASH7_PREFIX ) );
			doc.add_term( FormatPrefix( hash.substr(0,9), XAP_GEOHASH9_PREFIX ) );

			// 3 char neighbour for city or zone range
			doc.add_term( FormatPrefix( hash.substr(0,3), XAP_NBRHASH3_PREFIX ) );
			for (auto it : gh.GetNeighbours(hash.substr(0,3),1)) doc.add_term(FormatPrefix(it, XAP_NBRHASH3_PREFIX));

			// 5 char two level neighbour for locality range
			doc.add_term( FormatPrefix( hash.substr(0,5), XAP_NBRHASH5_PREFIX ) );
			for (auto it : gh.GetNeighbours(hash.substr(0,5),1)) doc.add_term(FormatPrefix(it, XAP_NBRHASH5_PREFIX));
			for (auto it : gh.GetNeighbours(hash.substr(0,5),2)) doc.add_term(FormatPrefix(it, XAP_NBRHASH5_PREFIX));

		}
		catch (::zpds::BaseException& e) {}
	}

	Xapian::LatLongCoords coords;
	coords.append(Xapian::LatLongCoord(lat,lon));
	doc.add_value(XAP_LATLON_POS, coords.serialise());
}

/**
* AddRecord : add manip data using ItemDataT
*
*/
void zpds::search::IndexBase::AddRecord(::zpds::utils::SharedTable::pointer stptr, ::zpds::store::ItemDataT* record) const
{
	Xapian::Document doc = CreateDoc( record );
	std::string idterm = "Q" + std::to_string(record->id());

	// write
	stptr->xapdb->Update(record->lang(), GetIndexType(), idterm, std::move(doc) );
}

/**
* DelRecord : del manip data using ItemDataT
*
*/
void  zpds::search::IndexBase::DelRecord(::zpds::utils::SharedTable::pointer stptr, ::zpds::store::ItemDataT* record) const
{
	std::string idterm = "Q" + std::to_string(record->id());
	stptr->xapdb->Delete(record->lang(), GetIndexType(), idterm);
}

