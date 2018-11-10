/**
 * @project zapdos
 * @file src/hrpc/HrpcClient.cc
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
 *  HrpcClient.cc : Hrpc implementation
 *                   - right now HTTP based with some headers
 *                   - can change to another protocol in future
 *
 */

#include "hrpc/HrpcClient.hpp"
#include "google/protobuf/descriptor.h"
#include "cpr/cpr.h"

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
		if (service_id == M_NOACTION)
			throw zpds::BadDataException("Bad Service Id");
		std::string endpoint = stptr->master.Get() + "/_zpds/master/v1/endpoint";
		std::string tmpstr;
		msg->SerializeToString(&tmpstr);

		auto r = cpr::Post(
		             cpr::Url{endpoint},
		             cpr::Body{b64_encode(tmpstr)},
		cpr::Header{
			{"Service-Name", std::to_string(service_id) },
			{"Content-Type", "application/proto"},
			{"Proto-Transfer-Encoding", "base64"},
			{"Shared-Secret", stptr->shared_secret.Get()}
		});
		if (r.status_code != 200)
			throw zpds::BadDataException("Cannot connect to master, bad HTTP Status");

		if (r.header["Service-Error"].length()>0) {
			int ecode = std::stoi(r.header["Service-Error"]);
			if (ecode>=ZPDS_M_ERROR_MIN && ecode<=ZPDS_M_ERROR_MAX)
				throw ::zpds::LocalException(r.text.c_str(), ecode);
			else
				throw ::zpds::BadDataException(r.text.c_str(), M_UNKNOWN);
			// rethrow the error from other side
		}
		bool data_ok = (r.header["Proto-Transfer-Encoding"] == "base64" ) ?
		               msg->ParseFromString(b64_decode(r.text)) : msg->ParseFromString(r.text);

		if (!data_ok)
			throw zpds::BadDataException("Bad Received Protobuf Data Format from master endpoint: " + endpoint);
	}
	catch (...) {
		if (nothrow) return false;
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
		if (service_id == R_NOACTION)
			throw zpds::BadDataException("Bad Service Id for remote ");
		std::string endpoint = address + "/_zpds/remote/v1/endpoint";
		std::string tmpstr;
		msg->SerializeToString(&tmpstr);
		DLOG(INFO) << service_id;
		DLOG(INFO) << msg->DebugString();

		auto r = cpr::Post(
		             cpr::Url{endpoint},
		             cpr::Body{b64_encode(tmpstr)},
		cpr::Header{
			{"Service-Name", std::to_string(service_id) },
			{"Content-Type", "application/proto"},
			{"Proto-Transfer-Encoding", "base64"},
			{"Shared-Secret", stptr->shared_secret.Get()}
		});
		if (r.status_code != 200)
			throw zpds::BadDataException("Bad HTTP Status for remote");

		// rethrow the error from other side
		if (r.header["Service-Error"].length()>0)
			throw ::zpds::BadDataException(r.text.c_str());

		bool data_ok = (r.header["Proto-Transfer-Encoding"] == "base64" ) ?
		               msg->ParseFromString(b64_decode(r.text)) : msg->ParseFromString(r.text);

		if (!data_ok)
			throw zpds::BadDataException("Bad Received Protobuf Data Format from remote from endpoint: " + endpoint);
	}
	catch (...) {
		if (nothrow) return false;
		std::rethrow_exception(std::current_exception());
	}
	return true;
}

