/**
 * @project zapdos
 * @file include/query/NotFoundService.hpp
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
 *  NotFoundService.hpp : NotFound Server Endpoint
 *
 */
#ifndef _ZPDS_QUERY_NOTFOUNDSERVICE_HPP_
#define _ZPDS_QUERY_NOTFOUNDSERVICE_HPP_

#include "query/ServiceBase.hpp"

namespace zpds {
namespace query {

template <class HttpServerT>
class NotFoundService : public zpds::query::ServiceBase<HttpServerT> {
public:

	/**
	* NotFoundService : constructor
	*
	* @param context
	*   zpds::utils::SharedTable::pointer stptr
	*
	* @param server
	*   HttpServerT server
	*
	* @param scope
	*   unsigned int scope check
	*
	* @return
	*   none
	*/

	NotFoundService(zpds::utils::SharedTable::pointer stptr,
	                typename std::shared_ptr<HttpServerT> server,
	                unsigned int scope)
		: zpds::query::ServiceBase<HttpServerT>(ZPDS_SERVICE_SCOPE_HTTP | ZPDS_SERVICE_SCOPE_HTTPS)
	{
		if (!(this->myscope & scope)) return; // scope mismatch

		server->default_resource["GET"] =
		[this](typename HttpServerT::RespPtr response, typename HttpServerT::ReqPtr request) {
			this->HttpErrorAction(response,request,404,"NOT FOUND");
		};

		server->default_resource["HEAD"] =
		[this](typename HttpServerT::RespPtr response, typename HttpServerT::ReqPtr request) {
			this->HttpErrorAction(response,request,404,"NOT FOUND");
		};

		server->default_resource["POST"] =
		[this](typename HttpServerT::RespPtr response, typename HttpServerT::ReqPtr request) {
			this->HttpErrorAction(response,request,404,"NOT FOUND");
		};

		server->default_resource["PUT"] =
		[this](typename HttpServerT::RespPtr response, typename HttpServerT::ReqPtr request) {
			this->HttpErrorAction(response,request,404,"NOT FOUND");
		};

		server->default_resource["DELETE"] =
		[this](typename HttpServerT::RespPtr response, typename HttpServerT::ReqPtr request) {
			this->HttpErrorAction(response,request,404,"NOT FOUND");
		};

		server->default_resource["CONNECT"] =
		[this](typename HttpServerT::RespPtr response, typename HttpServerT::ReqPtr request) {
			this->HttpErrorAction(response,request,404,"NOT FOUND");
		};

		server->default_resource["OPTIONS"] =
		[this](typename HttpServerT::RespPtr response, typename HttpServerT::ReqPtr request) {
			this->HttpErrorAction(response,request,404,"NOT FOUND");
		};

		server->default_resource["TRACE"] =
		[this](typename HttpServerT::RespPtr response, typename HttpServerT::ReqPtr request) {
			this->HttpErrorAction(response,request,404,"NOT FOUND");
		};

	}

private:
};

} // namespace query
} // namespace zpds

#endif // _ZPDS_QUERY_NOTFOUNDSERVICE_HPP_
