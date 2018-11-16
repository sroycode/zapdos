/**
 * @project zapdos
 * @file include/query/WebServiceList.hpp
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
 *  WebServiceList.hpp :   Set up Service List
 *
 */
#ifndef _ZPDS_QUERY_WEBSERVICELIST_HPP_
#define _ZPDS_QUERY_WEBSERVICELIST_HPP_

#include "query/NotFoundService.hpp"
#include "query/HelpQuery.hpp"
#include "query/QueryBase.hpp"
#include "query/InfoService.hpp"
#include "query/StaticFileService.hpp"

#include "query/ProfileService.hpp"
#include "query/LookupRecordService.hpp"
#include "query/TextRecordService.hpp"
#include "query/ExtraAttribService.hpp"
#include "query/PhotonService.hpp"

#define ZPDS_WEBSERVICELIST_SCOPE_HTTP \
		zpds::query::NotFoundService<HttpServerT> {sharedtable,server,ZPDS_SERVICE_SCOPE_HTTP}; \
		zpds::query::HelpQuery::pointer helpquery = std::make_shared<zpds::query::HelpQuery>(); \
		zpds::query::InfoService<HttpServerT>(sharedtable,server,helpquery,ZPDS_SERVICE_SCOPE_HTTP); \
		zpds::query::StaticFileService<HttpServerT>(sharedtable,server,helpquery,ZPDS_SERVICE_SCOPE_HTTP); \
		zpds::query::ProfileService<HttpServerT>(sharedtable,server,helpquery,ZPDS_SERVICE_SCOPE_HTTP); \
		zpds::query::LookupRecordService<HttpServerT>(sharedtable,server,helpquery,ZPDS_SERVICE_SCOPE_HTTP); \
		zpds::query::TextRecordService<HttpServerT>(sharedtable,server,helpquery,ZPDS_SERVICE_SCOPE_HTTP); \
		zpds::query::ExtraAttribService<HttpServerT>(sharedtable,server,helpquery,ZPDS_SERVICE_SCOPE_HTTP); \
		zpds::query::PhotonService<HttpServerT>(sharedtable,server,helpquery,ZPDS_SERVICE_SCOPE_HTTP); 

#endif // _ZPDS_QUERY_WEBSERVICELIST_HPP_
