/**
 * @project zapdos
 * @file include/hrpc/RemoteEndpointService.hpp
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
 *  RemoteEndpointService.hpp : Hrpc Remote Service Endpoints
 *
 */
#ifndef _ZPDS_HRPC_REMOTE_ENDPOINT_SERVICE_HPP_
#define _ZPDS_HRPC_REMOTE_ENDPOINT_SERVICE_HPP_

#include "query/QueryBase.hpp"
#include "hrpc/ProtoServiceBase.hpp"
#include "hrpc/HrpcClient.hpp"
#include "store/StoreTrans.hpp"

#include "hrpc/RemoteKeeper.hpp"

namespace zpds {
namespace hrpc {

template <class HttpServerT>
class RemoteEndpointService :
	public zpds::query::ServiceBase<HttpServerT>,
	public zpds::hrpc::ProtoServiceBase<HttpServerT> {
public:

	/**
	* RemoteEndpointService : constructor
	*
	* @param stptr
	*   zpds::utils::SharedTable::pointer stptr
	*
	* @param server
	*   std::shared_ptr<HttpServerT> server
	*
	* @param rkeeper
	*   ::zpds::hrpc::RemoteKeeper::pointer rkeeper
	*
	* @param scope
	*   const unsigned int scope check
	*
	* @return
	*   none
	*/

	RemoteEndpointService(
	    zpds::utils::SharedTable::pointer stptr,
	    typename std::shared_ptr<HttpServerT> server,
	    ::zpds::hrpc::RemoteKeeper::pointer rkeeper,
	    unsigned int scope)
		:
		zpds::query::ServiceBase<HttpServerT>(ZPDS_SERVICE_SCOPE_HTTP | ZPDS_SERVICE_SCOPE_HTTPS)
	{
		if (!(this->myscope & scope)) return; // scope mismatch

		// Endpoint : POST _zpds/remote/v1/endpoint

		server->resource["/_zpds/remote/v1/endpoint$"]["POST"]
		=[this,stptr,rkeeper](typename HttpServerT::RespPtr response, typename HttpServerT::ReqPtr request) {
			ZPDS_PARALLEL_ONE([this,stptr,rkeeper,response,request] {
				std::string output;
				int ecode=M_UNKNOWN;
				try
				{
					if (!this->ProtoRequest(request))
						throw zpds::BadDataException("Only Protobuf Supported");
					if (!this->SharedSecretOK(request,stptr->shared_secret.Get()))
						throw zpds::BadDataException("Bad Shared Secret");

					auto sname = this->ServiceName(request);

					if (!stptr->is_ready.Get()) throw zpds::BadDataException("System Not Ready");

					// start here
					switch(sname) {
					case ::zpds::hrpc::R_REGISTER : {
						zpds::hrpc::StateT data;
						if (!data.ParseFromString( ::zpds::utils::S64String::Decode(request->content.string()) ))
							throw zpds::BadDataException("Bad Protobuf Format");
						// action
						// update remote in list
						if (stptr->is_master.Get()) {
							rkeeper->AddHost( data.thisurl(), data.logcounter(), data.ts() );
							rkeeper->HostUpdate();
						}
						// aftermath
						data.Clear();
						rkeeper->GetHosts(&data);
						data.SerializeToString(&output);
						DLOG(INFO) << request->path << " " << sname;
						break;
					}
					case ::zpds::hrpc::R_READONE : {
						zpds::store::TransactionT data;
						if (!data.ParseFromString( ::zpds::utils::S64String::Decode(request->content.string()) ))
							throw zpds::BadDataException("Bad Protobuf Format");
						// action
						zpds::store::StoreTrans service; //ReadOne
						service.ReadOne(stptr, &data);
						// aftermath
						data.SerializeToString(&output);
						DLOG(INFO) << request->path << " " << sname;
						break;
					}
					case ::zpds::hrpc::R_TRANSLOG : {
						zpds::store::TransListT data;
						if (!data.ParseFromString( ::zpds::utils::S64String::Decode(request->content.string()) ))
							throw zpds::BadDataException("Bad Protobuf Format");
						// action
						// check if counter is current
						auto logc = stptr->logcounter.Get();
						if (data.lastid()>logc)
							throw ::zpds::BadDataException("Remote Log Counter ahead");
						// if no change return
						if (data.lastid()==logc) {
							data.set_ts( ZPDS_CURRTIME_MS );
							data.set_currid( logc );
							data.set_limit( 0 );
							// set the last slave as this if unset, this slave is in sync
							// so future pushes can go to this
							if (stptr->lastslave.Get().empty())
								stptr->lastslave.Set(data.endpoint());
						}
						else {
							// if counter is not current
							zpds::store::StoreTrans service; //ReadLog
							service.ReadLog(stptr, &data);
						}
						// aftermath
						data.SerializeToString(&output);
						DLOG(INFO) << request->path << " " << sname;
						break;
					}
					case ::zpds::hrpc::R_SETINFO : {
						zpds::hrpc::StateT data;
						if (!data.ParseFromString( ::zpds::utils::S64String::Decode(request->content.string()) ))
							throw zpds::BadDataException("Bad Protobuf Format");
						// action
						// update remotes only if from master
						rkeeper->SetHosts(&data);
						// aftermath
						data.Clear();
						rkeeper->GetHosts(&data);
						data.SerializeToString(&output);
						DLOG(INFO) << request->path << " " << sname;
						break;
					}
					case ::zpds::hrpc::R_ADDHOST : {
						zpds::hrpc::RemoteT data;
						if (!data.ParseFromString( ::zpds::utils::S64String::Decode(request->content.string()) ))
							throw zpds::BadDataException("Bad Protobuf Format");
						// action
						// update remotes only if from master
						if (stptr->is_master.Get())
							throw zpds::BadDataException("Host addition is slave only");
						rkeeper->AddHost(&data);
						// aftermath
						data.Clear();
						data.SerializeToString(&output);
						DLOG(INFO) << request->path << " " << sname;
						break;
					}
					case ::zpds::hrpc::R_BUFFTRANS : {
						zpds::store::TransactionT data;
						if (!data.ParseFromString( ::zpds::utils::S64String::Decode(request->content.string()) ))
							throw zpds::BadDataException("Bad Protobuf Format");
						// update only if from master
						if (stptr->is_master.Get())
							throw zpds::BadDataException("Data addition is slave only");
						// action
						data.SerializeToString(&output);
						stptr->transactions.AddOne(data.id(),output);
						// aftermath
						DLOG(INFO) << request->path << " " << sname;
						break;
					}
					default:
						DLOG(INFO) << request->path << " " << sname;
						throw zpds::BadDataException("Service Name unknown or unimplemented");
						break;
					}
					ecode=0; // something reached here
					// end here

				}
				catch (...)
				{
					ecode=M_UNKNOWN; // default
					output="Unknown Error";
				}

				if (ecode==0)
				{
					this->ServiceOKAction(response,request,::zpds::utils::S64String::Encode(output));
				}
				else
				{
					this->ServiceErrAction(response,request,ecode,output);
				}
			});
		};
	}
};
} // namespace query
} // namespace zpds

#endif // _ZPDS_HRPC_REMOTE_ENDPOINT_SERVICE_HPP_

