/**
 * @project zapdos
 * @file src/hrpc/SyncServiceList.hpp
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
 *  SyncServiceList.hpp :   Set up Service List
 *
 */
#ifndef _ZPDS_HRPC_SYNC_SERVICE_LIST_HPP_
#define _ZPDS_HRPC_SYNC_SERVICE_LIST_HPP_

#include <query/NotFoundService.hpp>
#include <query/QueryBase.hpp>
#include <hrpc/HrpcMasterEndpointService.hpp>
#include <hrpc/HrpcRemoteEndpointService.hpp>

#define ZPDS_SYNCSERVICELIST_SCOPE_HTTP \
		zpds::query::NotFoundService<HttpServerT> {sharedtable,server,ZPDS_SERVICE_SCOPE_HTTP}; \
		zpds::hrpc::RemoteKeeper::pointer rkeeper = std::make_shared<zpds::hrpc::RemoteKeeper>(sharedtable); \
		zpds::hrpc::HrpcMasterEndpointService<HttpServerT>(sharedtable,server,rkeeper,ZPDS_SERVICE_SCOPE_HTTP); \
		zpds::hrpc::HrpcRemoteEndpointService<HttpServerT>(sharedtable,server,rkeeper,ZPDS_SERVICE_SCOPE_HTTP);

#define ZPDS_SYNCSERVICELIST_SCOPE_HTTPS \
		zpds::query::NotFoundService<HttpServerT> {sharedtable,server,ZPDS_SERVICE_SCOPE_HTTP};

#endif // _ZPDS_HRPC_SYNC_SERVICE_LIST_HPP_
