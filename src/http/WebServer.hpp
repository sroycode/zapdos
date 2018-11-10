/**
 * @project zapdos
 * @file src/http/WebServer.hpp
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
 *  WebServer.hpp :   Web Server Headers
 *
 */
#ifndef _ZPDS_HTTP_WEBSERVER_HPP_
#define _ZPDS_HTTP_WEBSERVER_HPP_

#include "utils/ServerBase.hpp"
#include "http/HttpServer.hpp"


namespace zpds {
namespace http {

class WebServer :
	public zpds::utils::ServerBase,
	public std::enable_shared_from_this<WebServer> {
public:
	using pointer=std::shared_ptr<WebServer>;
	using HttpServerT=HttpServer<HTTP>;

	/**
	* make noncopyable
	*/

	WebServer() = delete;
	WebServer(const WebServer&) = delete;
	WebServer& operator=(const WebServer&) = delete;

	/**
	* create : static construction creates new first time
	*
	* @param io_service_
	*   std::shared_ptr<boost::asio::io_service> io service pointer
	*
	* @param stptr
	*   zpds::utils::SharedTable::pointer sharedtable pointer
	*
	* @return
	*   none
	*/
	static pointer create(std::shared_ptr<boost::asio::io_service> io_service_, zpds::utils::SharedTable::pointer stptr)
	{
		pointer p(new WebServer(io_service_,stptr));
		return p;
	}

	/**
	* share : return instance
	*
	* @return
	*   none
	*/
	pointer share()
	{
		return shared_from_this();
	}

	/**
	* destructor
	*/
	virtual ~WebServer ();

	/**
	* init : initialize
	*
	* @param params
	*   ParamsListT parameters to init
	*
	* @return
	*   none
	*/
	void init(zpds::utils::ServerBase::ParamsListT params);

	/**
	 * GetSection : section required
	 *
	 * @return
	 *   std::string section
	 */
	const static std::string GetSection();

	/**
	 * GetRequire : parameters required
	 *
	 * @return
	 *   ParamsListT
	 */
	const static zpds::utils::ServerBase::ParamsListT GetRequire();

	/**
	* stop : shutdown
	*
	* @return
	*   none
	*/
	void stop();

protected:
	std::shared_ptr<HttpServerT> server;
	std::shared_ptr<boost::asio::io_service> io_service;

private:
	bool is_init;

	/**
	* Constructor : private used Constructor
	*
	* @param io_service_
	*   std::shared_ptr<boost::asio::io_service> io service pointer
	*
	* @param stptr
	*   zpds::utils::SharedTable::pointer sharedtable pointer
	*
	*/
	WebServer(std::shared_ptr<boost::asio::io_service> io_service_, zpds::utils::SharedTable::pointer stptr);

};
} // namespace http
} // namespace zpds
#endif /* _ZPDS_HTTP_WEBSERVER_HPP_ */
