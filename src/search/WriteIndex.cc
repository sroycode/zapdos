/**
 * @project zapdos
 * @file src/search/WriteIndex.cc
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
 *  WriteIndex.cc : Xapian Search Writable Index impl
 *
 */
#include <cmath>

#include "search/WriteIndex.hpp"
#include <boost/filesystem.hpp>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>


/**
 * Constructor : default
 *
 */
zpds::search::WriteIndex::WriteIndex(std::string dbpath_)
	: dbpath(dbpath_)
{
	if (dbpath.empty()) throw zpds::InitialException("dbpath cannot be blank");
	const google::protobuf::EnumDescriptor *l = zpds::search::LangTypeE_descriptor();
	const google::protobuf::EnumDescriptor *d = zpds::search::IndexTypeE_descriptor();
	for (auto i=0 ; i < l->value_count() ; ++i ) {
		for (auto j=0 ; j < d->value_count() ; ++j ) {
			Get(::zpds::search::LangTypeE(i), ::zpds::search::IndexTypeE(j) );
			LOG(INFO) << "Created xap index : " << l->value(i)->name() << "_" << d->value(j)->name();
		}
	}
}

/**
 * Destructor : default
 *
 */
zpds::search::WriteIndex::~WriteIndex()
{
	for (auto it = triemap.begin() ; it != triemap.end() ; ++it) {
		it->second.close();
	}
}

/**
* CommitData : commits data
*
*/
void zpds::search::WriteIndex::CommitData()
{
	for (auto it = triemap.begin() ; it != triemap.end() ; ++it) {
		it->second.commit();
	}
}

/**
* Get: get the storage instance
*
*/
zpds::search::WriteIndex::DatabaseT& zpds::search::WriteIndex::Get(::zpds::search::LangTypeE ltyp, ::zpds::search::IndexTypeE dtyp)
{
	const google::protobuf::EnumDescriptor *l = zpds::search::LangTypeE_descriptor();
	const google::protobuf::EnumDescriptor *d = zpds::search::IndexTypeE_descriptor();
	int f = (ltyp * 1000) + dtyp;
	if ( triemap.find(f) == triemap.end() ) {
		const std::string xapath{dbpath + "/" + l->FindValueByNumber(ltyp)->name() + "_" + d->FindValueByNumber(dtyp)->name()};
		if (!boost::filesystem::exists(xapath)) boost::filesystem::create_directories(xapath);
		triemap[f] = DatabaseT(xapath, Xapian::DB_CREATE_OR_OPEN );
	}
	return triemap.at(f);
}

/**
* Update : updates a document
*
*/
void zpds::search::WriteIndex::Update(
    ::zpds::search::LangTypeE ltyp, ::zpds::search::IndexTypeE dtyp,
    const std::string& idterm, ::Xapian::Document&& doc)
{
	std::lock_guard<std::mutex> lock(update_lock);
	DLOG(INFO) << idterm ;
	DLOG(INFO) << doc.serialise() ;
	Get(ltyp, dtyp).replace_document(idterm, doc);
}

/**
* Delete : deletes a document
*
*/
void zpds::search::WriteIndex::Delete(::zpds::search::LangTypeE ltyp, ::zpds::search::IndexTypeE dtyp, const std::string& idterm)
{
	std::lock_guard<std::mutex> lock(update_lock);
	DLOG(INFO) << idterm ;
	Get(ltyp, dtyp).delete_document(idterm);
}
