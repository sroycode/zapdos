/**
 * @project zapdos
 * @file src/tools/DataField.hpp
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
 *  DataField.hpp : data field
 *
 */

#ifndef _ZPDS_TOOLS_DATA_FIELD_HPP_
#define _ZPDS_TOOLS_DATA_FIELD_HPP_

#include "utils/BaseUtils.hpp"
#include <unordered_set>
#include <unordered_map>
#include <pqxx/pqxx>

#define TRY_CATCH_LOOP(X) try { X } catch(...){}

namespace zpds {
namespace tools {

using StringSetT = std::unordered_set<std::string>;
// hamlet, village was creating major chaos in metro cities like mumbai - sroycode
static const StringSetT city_place_values { "city", "county", "town" }; // in alphabetic order
static const StringSetT locality_place_values {"hamlet", "neighbourhood", "suburb", "village"}; // alphabetic order
static const StringSetT useful_context_keys {"boundary", "landuse", "place"}; // alphabetic order
using IntSetT = std::unordered_set<int>;
// relation ids that are known to be cities, but cannot be deduced by mapping as they are mapped in a special way
// see https://github.com/komoot/photon/issues/138
static const IntSetT curated_city_relation_ids {
	27021, 27027, 62340, 62347, 62349, 62352, 62369, 62370, 62374, 62381
	, 62385, 62391, 62396, 62400, 62403, 62405, 62407, 62409, 62410, 62411
	, 62414, 62418, 62422, 62428, 62430, 62444, 62449, 62450, 62453, 62455
	, 62456, 62464, 62470, 62471, 62478, 62481, 62484, 62493, 62495, 62496
	, 62499, 62508, 62512, 62518, 62522, 62523, 62525, 62526, 62528, 62531
	, 62532, 62534, 62539, 62554, 62559, 62562, 62573, 62578, 62581, 62589
	, 62590, 62591, 62594, 62598, 62629, 62630, 62631, 62634, 62636, 62638
	, 62640, 62642, 62644, 62646, 62649, 62652, 62654, 62658, 62659, 62671
	, 62675, 62685, 62691, 62693, 62695, 62699, 62701, 62713, 62717, 62719
	, 62720, 62724, 62734, 62745, 62748, 62751, 62768, 62772, 62780, 62782
	, 172679, 191645, 285864, 1800297, 1829065, 2168233, 2793104
};

struct DataFieldT {
	std::string   styp = "S_OSM"; // INPUT record source type
	// std::string   uniqueid; // INPUT unique id uses placeid
	double        importance=0; // INPUT importance or equivalent
	std::string   ccode; // INPUT country code Alpha 3
	std::string   scode; // INPUT state code
	std::string   city; // INPUT city name
	std::string   country; // INPUT country
	std::string   state; // INPUT state
	std::string   fld_name; // INPUT name field
	std::string   fld_area; // INPUT area field
	std::string   address; // INPUT address
	std::string   pincode; // INPUT pin code
	double        accuracy=0; // INPUT accuracy of lat lon in m
	double        lat=0.0; // INPUT latitude
	double        lon=0.0; // INPUT longitude
	double        rating; // INPUT average rating of place
	std::string   landmark; // INPUT landmark
	std::string   is_in_place; // INPUT uniqueid of parent place comma sep
	std::string   tags; // INPUT keywords comma sep
	std::string   lang; // INPUT language
	uint64_t      osm_id; // INPUT osm_id
	std::string   osm_key; // INPUT osm_key
	std::string   osm_value; // INPUT osm_value
	std::string   osm_type; // INPUT osm_type
	std::string   my_geom; // INPUT geometry

	// additional
	double area ; // Geometry area in sqm
	uint64_t placeid; // placeid
	uint64_t parent_placeid; // parent placeid
	uint64_t linked_placeid; // linked placeid
	int rank_address;
	int admin_level;
	std::string place;
	std::string housenumber;
	std::string county;

	/**
	* PrintDebug : debug print
	*
	* @return
	*   none
	*/
	void PrintDebug()
	{
		std::cout
		        << "styp= "<< styp << "\n"
		        << "uniqueid= "<< placeid << "\n"
		        << "importance= "<< importance << "\n"
		        << "ccode= "<< ccode << "\n"
		        << "scode= "<< scode << "\n"
		        << "city= "<< city << "\n"
		        << "country= "<< country << "\n"
		        << "state= "<< state << "\n"
		        << "fld_name= "<< fld_name << "\n"
		        << "fld_area= "<< fld_area << "\n"
		        << "address= "<< address << "\n"
		        << "pincode= "<< pincode << "\n"
		        << "accuracy= "<< accuracy << "\n"
		        << "lat= "<< lat << "\n"
		        << "lon= "<< lon << "\n"
		        << "rating= "<< rating << "\n"
		        << "landmark= "<< landmark << "\n"
		        << "is_in_place= "<< is_in_place << "\n"
		        << "tags= "<< tags << "\n"
		        << "parent_placeid= "<< parent_placeid << "\n"
		        << "linked_placeid= "<< linked_placeid << "\n"
		        << "placeid= "<< placeid << "\n"
		        << "osm_id= "<< osm_id << "\n"
		        << "osm_type= "<< osm_type << "\n"
		        << "osm_key= "<< osm_key << "\n"
		        << "osm_value= "<< osm_value << "\n"
		        << "my_geom length= "<<  my_geom.length()<< "\n"
		        << "area= "<< area << "\n"
		        << "lang= "<< lang << std::endl;
	}

	/**
	* PrintName : name fields for spellchecker
	*
	* @return
	*   none
	*/
	void PrintName()
	{
		// if (! is_useful_for_context() ) return;
		std::cout << fld_name << " , " << fld_area << std::endl;
	}

	/**
	* Print : fields for search
	*
	* @return
	*   none
	*/
	void Print()
	{
		// if (! is_useful_for_context() ) return;
		std::cout
		        << styp << "\t"
		        << placeid << "\t"
		        << importance << "\t"
		        << ccode << "\t"
		        << scode << "\t"
		        << city << "\t"
		        << country << "\t"
		        << state << "\t"
		        << fld_name << "\t"
		        << fld_area << "\t"
		        << address << "\t"
		        << pincode << "\t"
		        << accuracy << "\t"
		        << lat << "\t"
		        << lon << "\t"
		        << rating << "\t"
		        << landmark << "\t"
		        << is_in_place << "\t"
		        << tags << "\t"
		        << lang << "\t"
		        << osm_id << "\t"
		        << osm_type << "\t"
		        << osm_key << "\t"
		        << osm_value << "\t"
		        << "" << "\t"
		        << "" << "\t"
		        << my_geom << std::endl;
	}

	/**
	* is_street : has address set
	*
	* @return
	*   bool
	*/
	bool is_street()
	{
		return (26 <= rank_address && rank_address < 28);
	}

	/**
	* is_city : is it a city
	*
	* @return
	*   bool
	*/
	bool is_city()
	{
		if ( (osm_key=="place" || (!place.empty()) ) && (city_place_values.find(osm_value)!=city_place_values.end()) ) return true;
		if ( (admin_level==8) && (osm_key == "boundary") && (osm_value == "administrative") ) return true;
		return false;
	}

	/**
	* is_locality: locality is mostly visible in level 15 - sroycode
	*
	* @return
	*   bool
	*/
	bool is_locality()
	{
		if ( (osm_key=="place" || (!place.empty()) ) && (locality_place_values.find(osm_value)!=locality_place_values.end()) ) return true;
		if ( (admin_level==15) && (osm_key == "boundary") && (osm_value == "administrative") ) return true;
		return false;
	}

	/**
	* is_curated_city: whether address row was manually marked as city
	*
	* @return
	*   bool
	*/
	bool is_curated_city()
	{
		if ( (osm_type=="R")  && (curated_city_relation_ids.find(osm_id)!=curated_city_relation_ids.end()) ) return true;
		return false;
	}

	/**
	* is_postcode: is postcode
	*
	* @return
	*   bool
	*/
	bool is_postcode()
	{
		if ( (osm_key=="place" ) && (osm_value=="postcode") ) return true;
		if ( (osm_key=="boundary" ) && (osm_value=="postal_code") ) return true;
		return false;
	}

	/**
	* has_postcode: is postcode null
	*
	* @return
	*   bool
	*/
	bool has_postcode()
	{
		return ( ! pincode.empty() );
	}

	/**
	* has_place: is place null
	*
	* @return
	*   bool
	*/
	bool has_place()
	{
		return ( ! place.empty() );
	}

	/**
	* is_useful_for_context : is it useful for context
	*
	* @return
	*   bool
	*/
	bool is_useful_for_context()
	{
		if (fld_name.empty()) return false;
		if (is_postcode()) return false;
		if (is_curated_city()) return false; // was already added to city
		if (rank_address < 4) return false; // continent, sea , etc
		if ( useful_context_keys.find(osm_key) != useful_context_keys.end() ) return true;
		return false;
	}

	/**
	* is_country: is country
	*
	* @return
	*   bool
	*/
	bool is_country()
	{
		if ( (admin_level==2) && (osm_key == "boundary") && (osm_value == "administrative") ) return true;
		if ( (osm_key=="place") && (osm_value=="country") ) return true;
		return false;
	}

	/**
	* is_state: is state
	*
	* @return
	*   bool
	*/
	bool is_state()
	{
		if ( (admin_level==4) && (osm_key == "boundary") && (osm_value == "administrative") ) return true;
		if ( (osm_key=="place") && (osm_value=="state") ) return true;
		return false;
	}

	/**
	* Constructor
	*
	* @param pgconn
	*   pqxx::connection& pgconn
	*
	* @param id
	*   uint64_t place_id
	*
	*/
	DataFieldT(pqxx::connection& pgconn, uint64_t id) : placeid(id)
	{
		pqxx::work txn(pgconn);
		std::ostringstream xtmp ;
		xtmp << "SELECT place_id, class, type, name->'name' as name , admin_level"
		     << ",housenumber,osm_type,osm_id, ST_Area(ST_Transform(geometry,3857)) as area"
		     << ",postcode, country_code, wikipedia"
		     << ",address->'street' as a_street , address->'country_code' as a_ccode"
		     << ",address->'housenumber' as a_house , address->'postcode' as a_pincode"
		     << ",rank_address, rank_search, parent_place_id, linked_place_id"
		     << ",ST_y(centroid) AS lat, ST_x(centroid) AS lon , Box2D(geometry) as my_geom"
		     << ",CASE WHEN importance = 0 OR importance IS NULL THEN 0.75-(rank_search::float/40) ELSE importance END AS importance"
		     << " FROM placex WHERE place_id = " << id << " LIMIT 1;" ;
		pqxx::result res = txn.exec(xtmp.str());
		for (auto row : res ) {
			TRY_CATCH_LOOP( placeid=row.at("place_id").as<uint64_t>(); )
			TRY_CATCH_LOOP( osm_id=row.at("osm_id").as<uint64_t>(); )
			osm_type=row.at("osm_type").c_str();
			TRY_CATCH_LOOP( area=row.at("area").as<double>(); )
			TRY_CATCH_LOOP( rank_address=row.at("rank_address").as<double>(); )
			TRY_CATCH_LOOP( admin_level=row.at("admin_level").as<double>(); )
			TRY_CATCH_LOOP( parent_placeid=row.at("parent_place_id").as<uint64_t>(); )
			TRY_CATCH_LOOP( linked_placeid=row.at("linked_place_id").as<uint64_t>(); )

			osm_key=row.at("class").c_str();
			osm_value=row.at("type").c_str();
			fld_name=row.at("name").c_str();
			pincode = row.at("a_pincode").is_null() ?  row.at("postcode").c_str() : row.at("a_pincode").c_str();
			ccode = row.at("a_ccode").is_null() ?  row.at("country_code").c_str() : row.at("a_ccode").c_str();
			if ( !row.at("a_street").is_null() ) address = row.at("a_street").c_str();
			TRY_CATCH_LOOP( lat=row.at("lat").as<double>(); )
			TRY_CATCH_LOOP( lon=row.at("lon").as<double>(); )
			TRY_CATCH_LOOP( importance=row.at("importance").as<double>(); )
			// extra
			housenumber = row.at("a_house").is_null() ?  row.at("housenumber").c_str() : row.at("a_house").c_str();
			my_geom = row.at("my_geom").c_str();

		}

		/**
				// from placex address tags
				xtmp.str(std::string());
				res.clear();
				// xtmp << "SELECT (each(name)).key,(each(name)).value FROM placex WHERE place_id=" << id << " ORDER BY (each(name)).key ;";
				xtmp << "SELECT * FROM placex WHERE place_id=" << id << ";";
				res = txn.exec(xtmp.str());
				for (auto row : res ) {
					// LOG(INFO) << row.at("key").c_str() << "\t=\t" << row.at("value").c_str() << std::endl;
					for (auto col : row ) {
						if (std::string(col.name())=="geometry") continue;
						LOG(INFO) << col.name() << "\t :=\t" << col.c_str() << std::endl;
					}
				}
		*/

		// from get_addressdata
		xtmp.str(std::string());
		res.clear();
		xtmp << "SELECT name->'name' as name"
		     << ",admin_level, rank_address, class, type, isaddress"
		     << " FROM get_addressdata(" << id << " , -1 ) ORDER BY rank_address DESC, isaddress DESC;";
		res = txn.exec(xtmp.str());
		for (auto row : res ) {
			if ( row.at("name").is_null() ) continue;
			bool isaddress = row.at("isaddress").is_null() ? 0 : row.at("isaddress").as<bool>() ;

			std::string name = row.at("name").c_str() ;
			int admin_level = row.at("admin_level").is_null() ? 0 : row.at("admin_level").as<int>() ;
			int rank_address = row.at("rank_address").is_null() ? 0 : row.at("rank_address").as<int>() ;

			std::string class_type = row.at("class").c_str() + std::string(":") + row.at("type").c_str() ;

			if (isaddress && class_type=="place:city") city = name;
			else if (isaddress && class_type=="place:country") country = name;
			else if (isaddress && class_type=="place:state") state = name;
			else if (isaddress && class_type=="place:county") county = name;
			else if (isaddress && class_type=="place:neighbourhood" ) fld_area += std::string(", ") + name;
			else if (isaddress && class_type=="place:suburb" ) fld_area += std::string(", ") + name;
			else if (isaddress && class_type=="place:village" ) fld_area += std::string(", ") + name;

			DLOG(INFO) << name << "\t" << admin_level << "\t" << rank_address << "\t" << isaddress << "\t" << class_type << std::endl;
		}

		if ( fld_area.substr(0,2) == ", ") fld_area = fld_area.substr(2);
		if ( city.empty() && ccode=="in") city = county;

		if ( address.empty() ) {
			// make subs address - TODO
		}

	}

};

} // namespace tools
} // namespace zpds

#endif // _ZPDS_TOOLS_DATA_FIELD_HPP_
