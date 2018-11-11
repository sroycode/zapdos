/**
 * @project zapdos
 * @file src/tools/LineReader.cc
 * @author  S Roychowdhury
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
 *  LineReader.cc : read file data implementation
 *
 */

#include "LineReader.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include "../proto/Query.pb.h"
#include <google/protobuf/reflection.h>
#include <google/protobuf/util/json_util.h>
#include <boost/range/algorithm/remove_if.hpp>
#include <boost/algorithm/string/classification.hpp>
#include "utils/StringHelpers.hpp"

/**
 * Constructor : default
 *
 */
zpds::tools::LineReader::LineReader() {}

/**
 * destructor
 */
zpds::tools::LineReader::~LineReader () {}

/**
 * ReadLookupRecord: read line vec to ER
 *
 */
void zpds::tools::LineReader::ReadLookupRecord(
    ::zpds::tools::LineReader::StrVecT& strvec, ::zpds::store::LookupRecordT* record)
{

	// SourceTypeE     styp           =  0 ( INPUT record source type )
	if (! strvec[0].empty()) {
		const google::protobuf::EnumDescriptor *descriptor = zpds::search::SourceTypeE_descriptor();
		if ( descriptor->FindValueByName( strvec[0] ) )
			record->set_styp( zpds::search::SourceTypeE ( descriptor->FindValueByName( strvec[0] )->number() ) );
	}
	// string          uniqueid       =  1 ( INPUT unique id )
	record->set_uniqueid( strvec[1] );
	// double          importance     =  2 ( INPUT importance or equivalent )
	if (!strvec[2].empty())
		record->set_importance( boost::lexical_cast<double> (strvec[2]) );
	// string          ccode          =  3 ( INPUT country code Alpha 3 )
	record->set_ccode( strvec[3] );
	// string          scode          =  4 ( INPUT state code )
	record->set_scode(strvec[4]);
	// string          city           =  5 ( INPUT city name )
	record->set_city(strvec[5]);
	// string          country        =  6 ( INPUT country )
	record->set_country(strvec[6]);
	// string          fld_name       =  7 ( INPUT name field )
	record->set_fld_name(strvec[7]);
	// string          fld_area       =  8 ( INPUT area field )
	record->set_fld_area(strvec[8]);
	// string          address        =  9 ( INPUT address )
	record->set_address(strvec[9]);
	// string          pincode        = 10 ( INPUT pin code )
	record->set_pincode(strvec[10]);
	// double          accuracy       = 11 ( INPUT accuracy of lat lon in m )
	if (!strvec[11].empty()) record->set_accuracy( boost::lexical_cast<uint64_t> (strvec[11]) );
	// double          lat            = 12 ( INPUT latitude )
	if (!strvec[12].empty()) record->set_lat( boost::lexical_cast<double> (strvec[12]) );
	// double          lon            = 13 ( INPUT longitude )
	if (!strvec[13].empty()) record->set_lon( boost::lexical_cast<double> (strvec[13]) );
	// double          rating         = 14 ( INPUT average rating of place )
	if (!strvec[14].empty()) record->set_rating( boost::lexical_cast<double> (strvec[14]) );
	// string          landmark       = 15 ( INPUT landmark )
	record->set_landmark(strvec[15]);
	// string          is_in_place    = 16 ( INPUT uniqueid of parent place comma sep )
	record->set_is_in_place(strvec[16]);
	// string          tags           = 17 ( INPUT keywords comma sep )
	record->set_tags(strvec[17]);
	// string          lang           = 18 ( INPUT language )
	if (!strvec[18].empty()) {
		std::string lang = strvec.at(18);
		boost::algorithm::to_upper(lang);
		const google::protobuf::EnumDescriptor *descriptor = zpds::search::LangTypeE_descriptor();
		if ( descriptor->FindValueByName( lang ) )
			record->set_lang( zpds::search::LangTypeE ( descriptor->FindValueByName(lang)->number() ) );
	}
	// string          osm_id          = 19 ( INPUT  osm_id )
	if (!strvec[19].empty()) record->set_osm_id( boost::lexical_cast<int64_t> (strvec[19]) );
	// string          osm_key         = 20 ( INPUT  osm_key )
	record->set_osm_key(strvec[20]);
	// string          osm_value       = 21 ( INPUT  osm_value )
	record->set_osm_value(strvec[21]);
	// string          osm_type        = 22 ( INPUT  osm_type )
	record->set_osm_type(strvec[22]);
	// SourceTypeE     alias_styp      =  23 ( INPUT record alias source type )
	if (! strvec[23].empty()) {
		const google::protobuf::EnumDescriptor *descriptor = zpds::search::SourceTypeE_descriptor();
		if ( descriptor->FindValueByName( strvec[23] ) )
			record->set_alias_styp( zpds::search::SourceTypeE ( descriptor->FindValueByName( strvec[0] )->number() ) );
	}
	// string          alias_uniqueid   = 24 ( INPUT alias unique id )
	record->set_alias_uniqueid( strvec[24] );
	// string          geometry         = 25 ( INPUT geometry )
	record->set_geometry( strvec[25] );

}


/**
* ReadToLookup: load to LookupDataT
*
*/
void zpds::tools::LineReader::ReadToLookup(
    ::zpds::tools::LineReader::StrVecT strvec, ::zpds::query::LookupDataT* data, int qtyp, size_t counter)
{
	{
		switch (qtyp) {
		default:
			break;
		case 1: // UPSERT
		case 2: // UPDATE
			if (strvec.size()<26) {
				LOG(INFO) << "Rejected Line no due to size " << counter ;
				break;
			}
			if ( strvec[0].empty() ||strvec[1].empty() ) {
				LOG(INFO) << "Rejected Line no due to empty styp uniqueid " << counter ;
				break;
			}
			try {
				::zpds::store::LookupRecordT record;
				ReadLookupRecord(strvec,&record);
				// swap record
				record.Swap( data->add_records() );
			}
			catch (std::exception& e) {
				DLOG(INFO) << "Rejected Line no due to data error " << counter << " : " << e.what();
			}
			catch (...) {
				LOG(INFO) << "Rejected Line no due to data error " << counter ;
			}
			break;

		}
	}
}

