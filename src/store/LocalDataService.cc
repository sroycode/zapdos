/**
 * @project zapdos
 * @file src/store/LocalDataService.cc
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
 *  LocalDataService.cc : LocalData service impl
 *
 */
#include "store/LocalDataService.hpp"
#include "store/LocalDataTable.hpp"

/**
* Constructor : default for LocalData
*
*/
zpds::store::LocalDataService::LocalDataService() {}

/**
* Destructor : for LocalData
*
*/
zpds::store::LocalDataService::~LocalDataService() {}

/**
* GetOneRecord : reads one entry for LocalData
*
*/
bool zpds::store::LocalDataService::GetOneRecord(::zpds::utils::SharedTable::pointer stptr, ::zpds::store::ItemDataT* data) const
{
	::zpds::store::LocalDataTable vdo_table{stptr->maindb.Get()};
	bool status=false;
	if (! data->unique_id().empty())
		status = vdo_table.GetOne(data,::zpds::store::U_LOCALDATA_UNIQUEID);
	else if (data->id() >0)
		status = vdo_table.GetOne(data,::zpds::store::K_LOCALDATA);
	return status;
}

/**
* GetManyRecords : get many record
*
*/
void zpds::store::LocalDataService::GetManyRecords (
    ::zpds::utils::SharedTable::pointer stptr,
    ::google::protobuf::RepeatedPtrField< ::zpds::store::ItemDataT >* data) const
{
	::zpds::store::LocalDataTable vdo_table{stptr->maindb.Get()};
	for (auto i=0 ; i< data->size(); ++i) {
		bool status=false;
		if (! data->Mutable(i)->unique_id().empty())
			status = vdo_table.GetOne(data->Mutable(i),::zpds::store::U_LOCALDATA_UNIQUEID);
		else if (data->Mutable(i)->id() >0)
			status = vdo_table.GetOne(data->Mutable(i),::zpds::store::K_LOCALDATA);
		if (!status) data->Mutable(i)->set_notfound(true);
	}
}

/**
* AddOneRecord : writes one entry
*
*/
bool zpds::store::LocalDataService::AddOneRecord(
    ::zpds::utils::SharedTable::pointer stptr,
    ::zpds::store::ItemDataT* data,
    zpds::store::TransactionT* trans,
    bool exists)
{
	::zpds::store::LocalDataTable vdo_table{stptr->maindb.Get()};
	return vdo_table.AddRecord(data,trans,exists);
}

/**
* GetKeyType : handles keytype for LocalData
*
*/
::zpds::store::KeyTypeE zpds::store::LocalDataService::GetKeyType() const
{
	return ::zpds::store::K_LOCALDATA;
}


/**
* AllowedFieldsFilter : sets update variable
*
*/
int32_t zpds::store::LocalDataService::AllowedFieldsFilter(int32_t update)
{

	int32_t xupdate=0;
	// if (update & ZPDS_IDFLD_TEXTFIELDS) xupdate+=ZPDS_IDFLD_TEXTFIELDS;
	if (update & ZPDS_IDFLD_CANSEE) xupdate+=ZPDS_IDFLD_CANSEE;
	if (update & ZPDS_IDFLD_TAGS) xupdate+=ZPDS_IDFLD_TAGS;
	if (update & ZPDS_IDFLD_CONTACTS) xupdate+=ZPDS_IDFLD_CONTACTS;
	if (update & ZPDS_IDFLD_IMAGES) xupdate+=ZPDS_IDFLD_IMAGES;
	if (update & ZPDS_IDFLD_VERSIONS) xupdate+=ZPDS_IDFLD_VERSIONS;
	if (update & ZPDS_IDFLD_PARENT) xupdate+=ZPDS_IDFLD_PARENT;
	if (update & ZPDS_IDFLD_CATEGORY) xupdate+=ZPDS_IDFLD_CATEGORY;
	if (update & ZPDS_IDFLD_ORIGIN) xupdate+=ZPDS_IDFLD_ORIGIN;
	if (update & ZPDS_IDFLD_LOCALFIELDS) xupdate+=ZPDS_IDFLD_LOCALFIELDS;
	if (update & ZPDS_IDFLD_OSMFIELDS) xupdate+=ZPDS_IDFLD_OSMFIELDS;
	if (update & ZPDS_IDFLD_IMPORTANCE) xupdate+=ZPDS_IDFLD_IMPORTANCE;
	if (update & ZPDS_IDFLD_GEOMETRY) xupdate+=ZPDS_IDFLD_GEOMETRY;
	if (update & ZPDS_IDFLD_RATING) xupdate+=ZPDS_IDFLD_RATING;
	return xupdate;
}
