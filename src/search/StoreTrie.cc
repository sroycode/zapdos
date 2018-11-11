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
	const google::protobuf::EnumDescriptor *d = zpds::search::LangTypeE_descriptor();
	for (auto i=0 ; i < d->value_count() ; ++i ) {
		Get(::zpds::search::LangTypeE(i));
		LOG(INFO) << "Created xap index : " << d->value(i)->name();
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
zpds::search::StoreTrie::DatabaseT& zpds::search::StoreTrie::Get(::zpds::search::LangTypeE dtyp)
{
	const google::protobuf::EnumDescriptor *d = zpds::search::LangTypeE_descriptor();
	if ( triemap.find(dtyp) == triemap.end() ) {
		const std::string xapath{dbpath + "/" + d->FindValueByNumber(dtyp)->name()};
		if (!boost::filesystem::exists(xapath)) boost::filesystem::create_directory(xapath);
		triemap[dtyp] = zpds::search::StoreTrie::DatabaseT(xapath, Xapian::DB_CREATE_OR_OPEN );
	}
	return triemap.at(dtyp);
}


/**
 * Del : delete from trie , not supported
 *
 */
void zpds::search::StoreTrie::DelData(::zpds::store::LookupRecordT* record) {}

/**
* AddData : add Lookup data
*
*/
void zpds::search::StoreTrie::AddData(::zpds::store::LookupRecordT* record)
{

	Xapian::Document doc;
	const std::string blank;
	const std::string prefix = XAP_PARTWORD_PREFIX;

	// set basic variables
	std::string suggest = PrintWithComma( record->fld_name(), record->fld_area(), record->pincode(), record->city(), record->country() ) ;

	// process suggest field
	auto wvec = Split(suggest);
	for (auto i=0; i< wvec.size(); ++i) {
		int pos = i+1;
		// default
		doc.add_posting(wvec.at(i), pos);

		// handle stem for full word
		if ((record->lang()==::zpds::search::LangTypeE::EN) && ( wvec.at(i).length() >= XAP_MIN_STEM_LEN )) {
			char word[26];
			strcpy(word, wvec.at(i).substr(0,25).c_str() );
			const char* nword = stemmer.kstem_stemmer(word);
			std::string stem_word = std::string(nword);
			if ( wvec.at(i) != stem_word) {
				doc.add_posting(stem_word, pos);
			}
		}

		// handle shingles with repeat words
		if (i>0) {
			if ( ( wvec.at(i) == wvec.at(i-1)) || ( wvec.at(i-1).length() <= XAP_MAX_SHIN_LEN ) ) {
				doc.add_posting( wvec.at(i-1)+wvec.at(i), pos);
			}
		}

		// handle first word
		if (i==0) doc.add_posting(XAP_BEGINFULL_PREFIX + wvec.at(i), pos);

		// handle partial words
		for (auto j=0; j< wvec.at(i).length() ; ++j) {
			doc.add_posting(XAP_PARTWORD_PREFIX + wvec.at(i).substr(0,j+1), pos);
			// special case of first word
			if (i==0) doc.add_posting(XAP_BEGINPART_PREFIX + wvec.at(i).substr(0,j+1), pos);
		}
	}

	// tags
	if (!record->tags().empty() ) {
		auto tvec = Split(record->tags());
		for (auto item : tvec ) {
			doc.add_posting(XAP_TAG_PREFIX + item, 0);
		}
	}

	// is_in_place
	if (!record->is_in_place().empty() ) {
		auto tvec = Split(record->is_in_place());
		for (auto item : tvec ) {
			doc.add_posting(XAP_ISINP_PREFIX + item, 0);
		}
	}

	// add pincode, city and country prefix
	if (!record->pincode().empty() && !record->ccode().empty())
		doc.add_term( FormatPrefix( record->pincode() +record->ccode(), XAP_PINCODE_PREFIX ) );
	if (!record->city().empty() && !record->ccode().empty())
		doc.add_term( FormatPrefix( record->city() +record->ccode(), XAP_CITY_PREFIX ) );
	if (!record->ccode().empty() )
		doc.add_term( FormatPrefix( record->ccode(), XAP_CCODE_PREFIX ) );


	// add geohash ignore if throw
	if ( ! (record->lat() ==0.0 && record->lon() ==0.0) ) {
		try {
			std::string hash = gh.Encode( record->lat(), record->lon(), 9);

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
	coords.append(Xapian::LatLongCoord(record->lat(),record->lon()));
	doc.add_value(XAP_LATLON_POS, coords.serialise());

	std::string idterm = "Q" + std::to_string(record->id());
	doc.add_boolean_term(idterm);

	doc.add_value(XAP_IMPORTANCE_POS, Xapian::sortable_serialise(record->importance()));
	doc.add_value(XAP_ROCKSID_POS, std::to_string(record->id()));

	// for (auto it = doc.termlist_begin() ; it != doc.termlist_end() ; ++it ) LOG(INFO) << *it;

	// write
	Get(record->lang()).replace_document(idterm, doc);
}

