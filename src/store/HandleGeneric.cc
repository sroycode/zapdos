/**
 * @project zapdos
 * @file src/store/HandleGeneric.cc
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
 *  HandleGeneric.cc : Generic Functions other classes impl
 *
 */
#include "store/HandleGeneric.hpp"
#include "store/ServiceDefine.hh"

/**
* WriteActionFilter : sets action variable
*
*/
std::pair<int32_t,bool> zpds::store::HandleGeneric::WriteActionFilter(::zpds::query::WriteTypeE writetype)
{
	int32_t xaction=ZPDS_UACTION_NONE;
	bool xmerge=false;
	switch(writetype) {
	default:
		break;
	case ::zpds::query::W_CREATE:
		xaction=ZPDS_UACTION_CREATE;
		break;
	case ::zpds::query::W_UPSERT:
		xaction=ZPDS_UACTION_UPSERT;
		break;
	case ::zpds::query::W_UPDATE:
		xaction=ZPDS_UACTION_UPDATE;
		break;
	case ::zpds::query::W_UMERGE:
		xaction=ZPDS_UACTION_UPDATE;
		xmerge=true;
		break;
	case ::zpds::query::W_DELETE:
		xaction=ZPDS_UACTION_DELETE;
		break;
	}
	if (xaction==ZPDS_UACTION_NONE)
		throw zpds::BadDataException("invalid action",M_INVALID_PARAM);
	return std::make_pair(xaction, xmerge);
}

/**
* ReaderTypeFilter : sets action variable
*
*/
int32_t zpds::store::HandleGeneric::ReaderTypeFilter(::zpds::query::ReaderTypeE readtype)
{
	int32_t xaction=0;
	switch(readtype) {
	default:
		break;
	case ::zpds::query::R_SELF:
		xaction=ZPDS_READR_IS_SELF;
		break;
	case ::zpds::query::R_ADMIN:
		xaction=ZPDS_READR_IS_ADMIN;
		break;
	case ::zpds::query::R_CREATR:
		xaction=ZPDS_READR_IS_CREATR;
		break;
	case ::zpds::query::R_OTHER:
		xaction=ZPDS_READR_IS_EXTER;
		break;
	case ::zpds::query::R_USER:
		xaction=ZPDS_READR_IS_USER;
		break;
	case ::zpds::query::R_PUBLIC:
		xaction=ZPDS_READR_IS_PUBLIC;
		break;
	}
	return xaction;
}
