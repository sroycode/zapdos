/**
 * @project zapdos
 * @file src/http/WebServer.cc
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
 *  WebServer.cc :   Web Server Implementation
 *
 */
#include <syslog.h>
#include <unistd.h>
#include <memory>

#include "WebServer.hpp"
#include <query/WebServiceList.hpp>

zpds::http::WebServer::WebServer(std::shared_ptr<boost::asio::io_service> io_service, zpds::utils::SharedTable::pointer stptr)
	: zpds::utils::ServerBase(stptr), io_service(io_service), is_init(false)
{
	DLOG(INFO) << "WebServer Created" << std::endl;
}

zpds::http::WebServer::~WebServer ()
{
	DLOG(INFO) << "WebServer Deleted" << std::endl;
}

const std::string zpds::http::WebServer::GetSection()
{
	return "http";
}

const zpds::utils::ServerBase::ParamsListT zpds::http::WebServer::GetRequire()
{
	return {"host","port"};
}

void zpds::http::WebServer::init(zpds::utils::ServerBase::ParamsListT params)
{
	if (params.size() != this->GetRequire().size() )
		throw zpds::ConfigException("WebServer: params and required size mismatch");
	server = std::make_shared<HttpServerT>( std::stoul(params[1]) );
	server->config.address=params[0];
	server->io_service = io_service;
	is_init=true;
	DLOG(INFO) << "WebServer init 1 here" << std::endl;
	{
		ZPDS_WEBSERVICELIST_SCOPE_HTTP
	}
	DLOG(INFO) << "WebServer init 2 here" << std::endl;
	server->start();
}

void zpds::http::WebServer::stop()
{
	if (is_init) server->stop();
	DLOG(INFO) << "WebServer Stopped" << std::endl;
}
