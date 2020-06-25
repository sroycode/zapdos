/**
 * @project zapdos
 * @file src/hrpc/HrpcClient.cc
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
 *  HrpcClient.cc : Hrpc Client impl
 *
 */
#include <thread>
#include <chrono>
#include <google/protobuf/descriptor.h>

#include "hrpc/HrpcClient.hpp"
#include "http/HttpClient.hpp"
#include "utils/SplitWith.hpp"


/**
* constructor
*
*/
::zpds::hrpc::HrpcClient::HrpcClient() {}

/**
* destructor
*/
::zpds::hrpc::HrpcClient::~HrpcClient () {}

/**
* SendToMaster : send to master and get output
*
*/
bool ::zpds::hrpc::HrpcClient::SendToMaster(
    ::zpds::utils::SharedTable::pointer stptr, ::zpds::hrpc::MasterCmdTypeE service_id,
    google::protobuf::Message* msg, bool nothrow)
{
	try {
		if (service_id == ::zpds::hrpc::M_NOACTION)
			throw zpds::BadDataException("Bad Service Id for master service");
		SendFunction(stptr, stptr->master.Get(),"/_zpds/master/v1/endpoint", std::to_string(service_id), msg, true);
	}
	catch (...) {
		if (nothrow) return false;
		// rethrow the error from other side
		std::rethrow_exception(std::current_exception());
	}
	return true;
}

/**
* SendToRemote : send to Remote and get output
*
*/
bool ::zpds::hrpc::HrpcClient::SendToRemote(::zpds::utils::SharedTable::pointer stptr, std::string address,
        ::zpds::hrpc::RemoteCmdTypeE service_id, google::protobuf::Message* msg, bool nothrow)
{
	try {
		if (service_id == ::zpds::hrpc::R_NOACTION)
			throw zpds::BadDataException("Bad Service Id for remote service");
		SendFunction(stptr, address,"/_zpds/remote/v1/endpoint", std::to_string(service_id), msg, false);
	}
	catch (...) {
		if (nothrow) return false;
		// rethrow the error from other side
		std::rethrow_exception(std::current_exception());
	}
	return true;
}

/**
* SendFunction : actual send
*
*/
void zpds::hrpc::HrpcClient::SendFunction(
    ::zpds::utils::SharedTable::pointer stptr,
    const std::string address, const std::string endpoint,
    const std::string service_name, google::protobuf::Message* msg, bool for_master)
{
	auto hostport = ::zpds::utils::SplitWith::Regex(address,boost::regex(":"),true);
	if (hostport.size()==3) {
		// pop front
		hostport.erase(hostport.begin());
		if (hostport.at(0).length()>2)
			hostport[0]=hostport.at(0).substr(2);
	}
	else if (hostport.size()==2) {
		// OK
	}
	else
		throw zpds::BadDataException("Bad Host:Port " + address);
	std::string tmpstr;
	msg->SerializeToString(&tmpstr);

	using HttpClient = ::zpds::http::HttpClient<::zpds::http::HTTP>;
	using RespPtr=std::shared_ptr< HttpClient::Response >;

	HttpClient client(hostport.at(0), std::stoi( hostport.at(1)));
	RespPtr response;

	::zpds::http::CaseInsensitiveMultimap header {
		{"Service-Name", service_name },
		{"Content-Type", "application/proto"},
		{"Proto-Transfer-Encoding", "base64"},
		{"Shared-Secret", stptr->shared_secret.Get()}
	};

	if (no_asio) {
		// use sync request
		response = client.request("POST", endpoint, ::zpds::utils::S64String::Encode(tmpstr), header);
	}
	else {
		// use async request
		client.io_whatever = stptr->io_whatever;
		std::promise<bool> response_promise;
		client.request( "POST", endpoint, ::zpds::utils::S64String::Encode(tmpstr), header,
		[&response, &response_promise](RespPtr response_, const ::zpds::http::error_code &ec) {
			response = response_;
			response_promise.set_value( (!ec) );
		});

		if (! response_promise.get_future().get() ) {
			throw zpds::BadDataException("Cannot connect , Unknown Error");
		}
	}

	// process response

	if (std::stoi(response->status_code) != 200)
		throw zpds::BadDataException("Cannot connect , bad HTTP Status");

	// rethrow the error from other side
	auto se = response->header.find("Service-Error");
	if ( se != response->header.end()) {
		int ecode = std::stoi(se->second);
		if (for_master && ecode>=ZPDS_M_ERROR_MIN && ecode<=ZPDS_M_ERROR_MAX)
			throw ::zpds::LocalException(response->content.string(), ecode);
		else
			throw ::zpds::BadDataException(response->content.string(), M_UNKNOWN);
	}

	auto te = response->header.find("Proto-Transfer-Encoding");
	if ( te == response->header.end()) {
		throw ::zpds::BadCodeException("Bad Hrpc data");
	}

	bool data_ok = (te->second == "base64" ) ?
	               msg->ParseFromString(::zpds::utils::S64String::Decode(response->content.string()))
	               : msg->ParseFromString(response->content.string());

	if (!data_ok)
		throw zpds::BadDataException("Bad Received Protobuf Data Format from master endpoint: " + endpoint);
}
