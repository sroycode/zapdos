/**
 * @project zapdos
 * @file src/search/StoreTrie.cc
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
 *  StoreTrie :  container for Trie implementation
 *
 */

#include <cmath>

#include "search/StoreTrie.hpp"
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include "utils/StringHelpers.hpp"
#include "search/DistanceSlabKeyMaker.hpp"


/**
 * Constructor : default private
 *
 */
zpds::search::StoreTrie::StoreTrie(std::string dbpath_) : dbpath(dbpath_)
{
	if (dbpath.empty()) throw zpds::InitialException("dbpath cannot be blank");
	const google::protobuf::EnumDescriptor *l = zpds::search::LangTypeE_descriptor();
	const google::protobuf::EnumDescriptor *d = zpds::search::DataTypeE_descriptor();
	for (auto i=0 ; i < l->value_count() ; ++i ) {
		for (auto j=0 ; j < d->value_count() ; ++j ) {
			Get(::zpds::search::LangTypeE(i), ::zpds::search::DataTypeE(j) );
			LOG(INFO) << "Created xap index : " << l->value(i)->name() << "_" << d->value(j)->name();
		}
	}

}

/**
 * Destructor : default
 *
 */
zpds::search::StoreTrie::~StoreTrie()
{
	for (auto it = triemap.begin() ; it != triemap.end() ; ++it) {
		it->second.close();
	}
}

/**
* Get: get the storage instance
*
*/
zpds::search::StoreTrie::DatabaseT& zpds::search::StoreTrie::Get(::zpds::search::LangTypeE ltyp, ::zpds::search::DataTypeE dtyp)
{
	const google::protobuf::EnumDescriptor *l = zpds::search::LangTypeE_descriptor();
	const google::protobuf::EnumDescriptor *d = zpds::search::DataTypeE_descriptor();
	int f = (ltyp * 1000) + dtyp;
	if ( triemap.find(f) == triemap.end() ) {
		const std::string xapath{dbpath + "/" + l->FindValueByNumber(ltyp)->name() + "_" + d->FindValueByNumber(dtyp)->name()};
		if (!boost::filesystem::exists(xapath)) boost::filesystem::create_directory(xapath);
		triemap[f] = zpds::search::StoreTrie::DatabaseT(xapath, Xapian::DB_CREATE_OR_OPEN );
	}
	return triemap.at(f);
}


/**
 * DelLocalData : delete local record , not supported
 *
 */
void zpds::search::StoreTrie::DelLocalData(::zpds::store::LookupRecordT* record) {}

/**
 * DelTextData : delete text record , not supported
 *
 */
void zpds::search::StoreTrie::DelTextData(::zpds::store::TextRecordT* record) {}

/**
* IndexFullWords : index items with full words
*
*/
void zpds::search::StoreTrie::IndexFullWords (Xapian::Document& doc,
	const std::string item, const std::string full_prefix , bool with_stem, bool zero_pos)
{
	auto wvec = Split(item);
	for (auto i=0; i< wvec.size(); ++i) {
		int pos = (zero_pos) ? 0 : i+1;
		// default
		doc.add_posting(full_prefix+wvec.at(i), pos);

		// handle stem for full word
		if (with_stem && ( wvec.at(i).length() >= XAP_MIN_STEM_LEN )) {
			char word[26];
			strcpy(word, wvec.at(i).substr(0,25).c_str() );
			const char* nword = stemmer.kstem_stemmer(word);
			std::string stem_word = std::string(nword);
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
* IndexWithPart : index items with part
*
*/
void zpds::search::StoreTrie::IndexWithPart (
	Xapian::Document& doc, const std::string item,
	const std::string full_prefix, const std::string part_prefix , bool with_stem, bool zero_pos)
{
	auto wvec = Split(item);
	for (auto i=0; i< wvec.size(); ++i) {
		int pos = (zero_pos) ? 0 : i+1;
		// default
		doc.add_posting(full_prefix+wvec.at(i), pos);

		// handle stem for full word
		if (with_stem && ( wvec.at(i).length() >= XAP_MIN_STEM_LEN )) {
			char word[26];
			strcpy(word, wvec.at(i).substr(0,25).c_str() );
			const char* nword = stemmer.kstem_stemmer(word);
			std::string stem_word = std::string(nword);
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
void zpds::search::StoreTrie::IndexGeo(Xapian::Document& doc, const double lat, const double lon)
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
* AddLocalData : add Lookup data
*
*/
void zpds::search::StoreTrie::AddLocalData(::zpds::store::LookupRecordT* record)
{

	switch ( record->styp() ) {
	case ::zpds::search::S_OSM:
	case ::zpds::search::S_MYLOCAL:
		break;
	default:
		return;
		break;
	}


	Xapian::Document doc;
	const std::string blank;
	bool with_stem = (record->lang()==::zpds::search::LangTypeE::EN);

	// process suggest field
	std::string suggest = PrintWithComma( record->fld_name(), record->fld_area(), record->pincode(), record->city(), record->country() ) ;
	IndexWithPart( doc , suggest , blank, XAP_PARTWORD_PREFIX , with_stem, false);

	// process name field
	if (!record->fld_name().empty() ) {
		IndexWithPart( doc , record->fld_name() , XAP_NAMEFULL_PREFIX, XAP_NAMEPART_PREFIX , with_stem, false);
	}

	// process firstword field
	{
		auto wvec = Split(record->fld_name());
		if (wvec.size()>0) 
			IndexWithPart( doc , wvec.at(0), XAP_BEGINFULL_PREFIX , XAP_BEGINPART_PREFIX , with_stem, false);
	}

	// tags
	if (!record->tags().empty() ) {
		IndexFullWords ( doc , record->tags(), XAP_TAG_PREFIX , false , true);
	}

	// is_in_place
	if (!record->is_in_place().empty() ) {
		IndexFullWords ( doc , record->is_in_place(), XAP_ISINP_PREFIX , with_stem , true);
	}

	// add pincode, city and country prefix
	if (!record->pincode().empty() && !record->ccode().empty())
		doc.add_term( FormatPrefix( record->pincode() +record->ccode(), XAP_PINCODE_PREFIX ) );
	if (!record->city().empty() && !record->ccode().empty())
		doc.add_term( FormatPrefix( record->city() +record->ccode(), XAP_CITY_PREFIX ) );
	if (!record->ccode().empty() )
		doc.add_term( FormatPrefix( record->ccode(), XAP_CCODE_PREFIX ) );

	IndexGeo ( doc , record->lat(), record->lon() );

	std::string idterm = "Q" + std::to_string(record->id());
	doc.add_boolean_term(idterm);

	doc.add_value(XAP_IMPORTANCE_POS, Xapian::sortable_serialise(record->importance()));
	doc.add_value(XAP_ROCKSID_POS, std::to_string(record->id()));

	// for (auto it = doc.termlist_begin() ; it != doc.termlist_end() ; ++it ) LOG(INFO) << *it;

	// write
	Get(record->lang(), ::zpds::search::DataTypeE::LOCAL).replace_document(idterm, doc);
}

/**
* AddTextData : add Text data
*
*/
void zpds::search::StoreTrie::AddTextData(::zpds::store::TextRecordT* record)
{

	switch ( record->styp() ) {
	case ::zpds::search::S_WIKI:
	case ::zpds::search::S_MYTEXT:
		break;
	default:
		return;
		break;
	}

	Xapian::Document doc;
	const std::string blank;
	bool with_stem = (record->lang()==::zpds::search::LangTypeE::EN);

	// process suggest field
	std::string suggest = PrintWithComma( record->title(), record->summary(), record->city(), record->country() ) ;
	IndexWithPart( doc , suggest , blank, XAP_PARTWORD_PREFIX , with_stem, false);

	// process title field
	if (!record->title().empty() ) {
		IndexWithPart( doc , record->title() , XAP_NAMEFULL_PREFIX, XAP_NAMEPART_PREFIX , with_stem, false);
	}

	// process firstword field
	{
		auto wvec = Split(record->title());
		if (wvec.size()>0) 
			IndexWithPart( doc , wvec.at(0), XAP_BEGINFULL_PREFIX , XAP_BEGINPART_PREFIX , with_stem, false);
	}

	// tags
	if (!record->tags().empty() ) {
		IndexFullWords ( doc , record->tags(), XAP_TAG_PREFIX , false , true);
	}

	// add pincode, city and country prefix if present
	if (!record->city().empty() && !record->ccode().empty())
		doc.add_term( FormatPrefix( record->city() +record->ccode(), XAP_CITY_PREFIX ) );
	if (!record->ccode().empty() )
		doc.add_term( FormatPrefix( record->ccode(), XAP_CCODE_PREFIX ) );

	// geo of exists
	IndexGeo ( doc , record->lat(), record->lon() );

	std::string idterm = "Q" + std::to_string(record->id());
	doc.add_boolean_term(idterm);

	doc.add_value(XAP_IMPORTANCE_POS, Xapian::sortable_serialise(record->importance()));
	doc.add_value(XAP_ROCKSID_POS, std::to_string(record->id()));

	// write
	Get(record->lang(), ::zpds::search::DataTypeE::TEXT).replace_document(idterm, doc);
}
