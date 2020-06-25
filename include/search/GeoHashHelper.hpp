/**
 * @project zapdos
 * @file include/search/GeoHashHelper.hpp
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
 *  GeoHashHelper.hpp : Geohash c++ helper Headers
 *
 */
#ifndef _ZPDS_SEARCH_GEOHASH_HELPER_HPP_
#define _ZPDS_SEARCH_GEOHASH_HELPER_HPP_

#include <vector>
#include "utils/BaseUtils.hpp"

#define ZPDS_DEFAULT_GEOHASH_PRECISION 5

#define ZPDS_NORTH               0
#define ZPDS_EAST                1
#define ZPDS_SOUTH               2
#define ZPDS_WEST                3

#define ZPDS_EARTH_RADIUS_M 6371000.8
#define ZPDS_EARTH_RADIUS_KM 6371.0
#define ZPDS_PI 3.14159265359
#define ZPDS_LENGTH_OF_DEGREE  111100        // meters

#define ZPDS_DEFAULT_MIN_LAT -90.0
#define ZPDS_DEFAULT_MAX_LAT  90.0
#define ZPDS_DEFAULT_MIN_LON -180.0
#define ZPDS_DEFAULT_MAX_LON  180.0

#define ZPDS_ALLOWED_MIN_LAT -85.05113
#define ZPDS_ALLOWED_MAX_LAT  85.05113
#define ZPDS_ALLOWED_MIN_LON -180.0
#define ZPDS_ALLOWED_MAX_LON  180.0

namespace zpds {
class BadGeoHashException : public BaseException {
	using BaseException::BaseException;
};

namespace search {

const static std::vector<double> GeoHashLatPrecs {23.0, 2.8, 0.7, 0.087, 0.022, 0.0027, 0.00068, 0.000085};
const static std::vector<double> GeoHashLonPrecs {23.0, 5.6, 0.7, 0.18, 0.022, 0.0055, 0.00068, 0.00017};
const static std::vector<double> GeoHashKmPrecs {2500.0, 630.0, 78.0, 20.0, 2.4, 0.61, 0.076, 0.019};

class GeoHashHelper {
public:
	struct IntervalT {
		double high;
		double low;
		IntervalT(double h,double l) : high(h), low(l) {}
	};

	struct NewsT {
		double north = 0.0; // maxlat
		double east = 0.0; // maxlon
		double south = 0.0; // minlat
		double west = 0.0; // minlon
	};

	struct GeoBoxDimensionT {
		double height = 0.0;
		double width = 0.0;
	};

	struct GeoCoordT {
		double lat = 0.0;
		double lon = 0.0;
		NewsT news;
		GeoBoxDimensionT dimension;
	};


	using NeighbourT = std::vector<std::string>;

	/**
	* Encode: get geohash of this precision
	*
	* @param lat
	*   double latitude
	*
	* @param lon
	*   double longitude
	*
	* @param prec
	*   size_t precesion
	*
	* @return
	*   std::string
	*/
	std::string Encode(double lat, double lon, size_t prec=ZPDS_DEFAULT_GEOHASH_PRECISION) const;

	/**
	* Decode: decode geohash
	*
	* @param hash
	*   std::string hash to decode
	*
	* @return
	*   GeoCoordT
	*/
	GeoCoordT Decode(std::string hash) const;

	/**
	* GetNeighbours : get neighbours
	*
	* @param hash
	*   std::string hash
	*
	* @param level
	*   size_t level of neighbour default 1
	*
	* @return
	*   NeighbourT
	*/
	NeighbourT GetNeighbours(std::string hash,size_t level=1) const;

	/**
	* Deg2Rad : degree to raidians
	*
	* @param degrees
	*   double degrees
	*
	* @return
	*   double radians
	*/
	double Deg2Rad(double degrees) const;

	/**
	* DistanceKm: Haversine distance between two points lon-lat points in km
	*
	* @param lat1
	*   double degrees
	*
	* @param lon1
	*   double degrees
	*
	* @param lat2
	*   double degrees
	*
	* @param lon2
	*   double degrees
	*
	* @return
	*   double radians
	*/
	double DistanceKm(double lat1, double lon1, double lat2, double lon2) const;

	/**
	* GetOneNeighbour : get neighbour in one dir
	*
	* @param hash
	*   std::string hash
	*
	* @param direction
	*   size_t direction
	*
	* @return
	*   NeighbourT
	*/
	std::string GetOneNeighbour(std::string hash, size_t direction) const;

	/**
	* GetBbox : get the hashes for a bbox
	*
	* @param left
	*   double left
	*
	* @param bottom
	*   double bottom
	*
	* @param right
	*   double right
	*
	* @param top
	*   double top
	*
	* @param prec
	*   double precesion
	*
	* @return
	*   NeighbourT
	*/
	NeighbourT GetBbox(double left, double bottom, double right, double top, size_t prec=ZPDS_DEFAULT_GEOHASH_PRECISION) const;

private:

};
} // namespace search
} // namespace zpds

#endif /* _ZPDS_SEARCH_GEOHASH_HELPER_HPP_ */
