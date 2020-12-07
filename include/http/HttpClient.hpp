/**
 * @project zapdos
 * @file include/http/HttpClient.hpp
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
 *  HttpClient.hpp : HTTP Client ( Modified from eidheim/Simple-Web-Server )
 *
 */
#ifndef _ZPDS_HTTP_CLIENT_HPP_
#define _ZPDS_HTTP_CLIENT_HPP_

#include "HttpClientBase.hpp"

namespace zpds {
namespace http {

template <class socket_type>
class HttpClient : public HttpClientBase<socket_type> {};

using HTTP = boost::asio::ip::tcp::socket;

template <>
class HttpClient<HTTP> : public HttpClientBase<HTTP> {
public:

	/**
	 * Constructs a client object.
	 *
	 * @param server_port_path Server resource given by host[:port][/path]
	 */
	HttpClient(const std::string &server_port_path, unsigned short port) noexcept
		: HttpClientBase<HTTP>::HttpClientBase(server_port_path, port) {}

protected:
	std::shared_ptr<Connection> create_connection() noexcept override
	{
		return std::make_shared<Connection>(handler_runner, *io_whatever);
	}

	void connect(const std::shared_ptr<Session> &session) override
	{
		if(!session->connection->socket->lowest_layer().is_open()) {
			auto resolver = std::make_shared<boost::asio::ip::tcp::resolver>(*io_whatever);
			session->connection->set_timeout(config.timeout_connect);
			async_resolve(*resolver, *host_port, [this, session, resolver](const error_code &ec, resolver_results results) {
				session->connection->cancel_timeout();
				auto lock = session->connection->handler_runner->continue_lock();
				if(!lock)
					return;
				if(!ec) {
					session->connection->set_timeout(config.timeout_connect);
					asio::async_connect(*session->connection->socket, results, [this, session, resolver](const error_code &ec, async_connect_endpoint /*endpoint*/) {
						session->connection->cancel_timeout();
						auto lock = session->connection->handler_runner->continue_lock();
						if(!lock)
							return;
						if(!ec) {
							asio::ip::tcp::no_delay option(true);
							error_code ec;
							session->connection->socket->set_option(option, ec);
							this->write(session);
						}
						else
							session->callback(ec);
					});
				}
				else
					session->callback(ec);
			});
		}
		else
			write(session);
	}

};

} // namespace http
} // namespace zpds
#endif	/* _ZPDS_HTTP_CLIENT_HPP_ */
