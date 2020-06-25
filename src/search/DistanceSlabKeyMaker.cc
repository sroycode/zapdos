/**
 * @project zapdos
 * @file src/search/DistanceSlabKeyMaker.cc
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
 *  DistanceSlabKeyMaker.cc : For Xapian Keys distance and values of 32-bit integer impl
 *
 */
#include <cmath>
#include <vector>
#include <limits>

#include "search/DistanceSlabKeyMaker.hpp"

/**
* KeepInBound : keep the number in bound
*
*  @param val
*     int64_t value
*
*  @param min
*     int64_t value min
*
*  @param max
*     int64_t value max
*
*  @return
*   int64_t
*/
template <typename T>
const inline T KeepInBound(T val, T min, T max)
{
	return (val > min) ? ( (val<max) ? val : max ) : min;
}

/**
* Constructor: default
*
*/
zpds::search::DistanceSlabKeyMaker::DistanceSlabKeyMaker(
    Xapian::valueno slot_,
    Xapian::valueno slot_o_,
    const Xapian::LatLongCoords& centre_,
    int slab_,
    int dd, int md)
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
zpds::search::DistanceSlabKeyMaker::~DistanceSlabKeyMaker()
{
	delete metric;
}

/**
* operator(): default
*
*/
std::string zpds::search::DistanceSlabKeyMaker::operator()(const Xapian::Document& doc) const
{

	// handle dist as int32 faster
	int dist = defdist;
	std::string val_l(doc.get_value(slot));
	if (!val_l.empty()) {
		Xapian::LatLongCoords doccoords;
		doccoords.unserialise(val_l);
		dist = KeepInBound<int>( std::lround((*metric)(centre, doccoords)), 0, maxdist);
	}
	int dslab = 1+ (dist/slab);

	// handle cc
	double val_o = KeepInBound<double>( Xapian::sortable_unserialise(doc.get_value(slot_o)), 0, XAP_DSLAB_MAX_IMPORTANCE );

	// if (dist!=defdist)
	// LOG(INFO) << "slab=" << slab << ", dist=" << dist << " , dslab=" << dslab
	// << ", val_o=" << val_o << " , final=" << (double)(dslab * XAP_DSLAB_MAX_IMPORTANCE) + (XAP_DSLAB_MAX_IMPORTANCE-val_o);
	return Xapian::sortable_serialise( (double)(dslab * XAP_DSLAB_MAX_IMPORTANCE) + (XAP_DSLAB_MAX_IMPORTANCE-val_o) );
}

