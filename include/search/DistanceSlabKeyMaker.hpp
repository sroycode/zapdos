/**
 * @project zapdos
 * @file include/search/DistanceSlabKeyMaker.hpp
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
 *  DistanceSlabKeyMaker.hpp :   For Xapian Keys distance and values of 32-bit integer
 *
 */
#ifndef _ZPDS_SEARCH_DISTANCE_SLAB_KEYMAKER_HPP_
#define _ZPDS_SEARCH_DISTANCE_SLAB_KEYMAKER_HPP_

#include <cmath>
#include <boost/format.hpp>
#include <xapian.h>

#include "utils/BaseUtils.hpp"
#include "utils/StringHelpers.hpp"
#include <vector>
#include <limits>

#define XAP_DSLAB_MAX_IMPORTANCE 100
#define XAP_DSLAB_MAX_DISTANCE  400000

namespace zpds {
namespace search {

class DistanceSlabKeyMaker : public Xapian::KeyMaker {

public:
	/**
	* Constructor: default
	*
	*  @param slot_
	*     Xapian::valueno  Value slot to use for loc.
	*
	*  @param slot_o_
	*     Xapian::valueno  Value slot to use for other.
	*
	*  @param centre_
	*     Xapian::LatLongCoords& List of points to calculate distance from (closest distance is used).
	*
	*  @param slab_
	*     int rounding value
	*
	*  @param dd
	*     int default distance
	*
	*  @param md
	*     int max distance
	*
	*/
	DistanceSlabKeyMaker(
	    Xapian::valueno slot_,
	    Xapian::valueno slot_o_,
	    const Xapian::LatLongCoords& centre_,
	    int slab_,
	    int dd= XAP_DSLAB_MAX_DISTANCE,
	    int md= XAP_DSLAB_MAX_DISTANCE)
		: slot(slot_),
		  slot_o(slot_o_),
		  centre(centre_),
		  metric(new Xapian::GreatCircleMetric()),
		  slab( KeepInBound<int>(slab_, 1, XAP_DSLAB_MAX_DISTANCE ) ),
		  defdist( KeepInBound<int>(dd, 0, XAP_DSLAB_MAX_DISTANCE ) ),
		  maxdist( KeepInBound<int>(md, 0, XAP_DSLAB_MAX_DISTANCE ) )
	{}

	/**
	* Destructor: default
	*
	*/
	~DistanceSlabKeyMaker()
	{
		delete metric;
	}

	/**
	* operator(): default
	*
	*  @param doc
	*   Xapian::Document&  doc
	*
	*  @return
	*   std::string
	*/
	std::string operator()(const Xapian::Document& doc) const
	{

		// handle dist as int32 faster
		int dist = defdist;
		std::string val_l(doc.get_value(slot));
		if (!val_l.empty()) {
			Xapian::LatLongCoords doccoords;
			doccoords.unserialise(val_l);
			dist = KeepInBound<int>( std::lround((*metric)(centre, doccoords)) , 0, maxdist);
		}
		int dslab = 1+ (dist/slab);

		// handle cc
		double val_o = KeepInBound<double>( Xapian::sortable_unserialise(doc.get_value(slot_o)) , 0, XAP_DSLAB_MAX_IMPORTANCE );

		// if (dist!=defdist)
		// LOG(INFO) << "slab=" << slab << ", dist=" << dist << " , dslab=" << dslab
		// << ", val_o=" << val_o << " , final=" << (double)(dslab * XAP_DSLAB_MAX_IMPORTANCE) + (XAP_DSLAB_MAX_IMPORTANCE-val_o);
		return Xapian::sortable_serialise( (double)(dslab * XAP_DSLAB_MAX_IMPORTANCE) + (XAP_DSLAB_MAX_IMPORTANCE-val_o) );
	}

protected:
	Xapian::valueno slot; // distance
	Xapian::valueno slot_o; // other slot
	Xapian::LatLongCoords centre;
	const Xapian::LatLongMetric* metric;
	int slab;
	int defdist;
	int maxdist;

};
} // namespace search
} // namespace zpds
#endif // _ZPDS_SEARCH_DISTANCE_SLAB_KEYMAKER_HPP_
