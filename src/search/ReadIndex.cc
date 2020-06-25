/**
 * @project zapdos
 * @file src/search/ReadIndex.cc
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
 *  ReadIndex.cc : Xapian Read Only Index impl
 *
 */
#include <cmath>

#include "search/ReadIndex.hpp"
#include <boost/filesystem.hpp>

#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>


/**
 * Constructor : default
 *
 */
zpds::search::ReadIndex::ReadIndex(std::string dbpath_)
	: dbpath(dbpath_)
{
	if (dbpath.empty()) throw zpds::InitialException("dbpath cannot be blank");
}

/**
 * Destructor : default
 *
 */
zpds::search::ReadIndex::~ReadIndex() {}

/**
* Get: get the storage instance
*
*/
zpds::search::ReadIndex::DatabaseT& zpds::search::ReadIndex::Get(::zpds::search::LangTypeE ltyp, ::zpds::search::IndexTypeE dtyp)
{
	const google::protobuf::EnumDescriptor *l = zpds::search::LangTypeE_descriptor();
	const google::protobuf::EnumDescriptor *d = zpds::search::IndexTypeE_descriptor();
	int f = ltyp * 1000 + dtyp;
	if ( triemap.find(f) == triemap.end() ) {
		const std::string xapath{dbpath + "/" + l->FindValueByNumber(ltyp)->name() + "_" + d->FindValueByNumber(dtyp)->name()};
		if (!boost::filesystem::exists(xapath))
			throw ::zpds::BadCodeException("search dir does no exist for this language");
		triemap[f] = DatabaseT(xapath, Xapian::DB_OPEN );
	}
	return triemap.at(f);
}

