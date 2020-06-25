/**
 * @project zapdos
 * @file include/hrpc/MasterEndpointService.hpp
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
 *  MasterEndpointService.hpp : Hrpc Master Only Service Endpoints
 *
 */
#ifndef _ZPDS_HRPC_MASTER_ENDPOINT_SERVICE_HPP_
#define _ZPDS_HRPC_MASTER_ENDPOINT_SERVICE_HPP_

#include "query/QueryBase.hpp"
#include "hrpc/ProtoServiceBase.hpp"
#include "hrpc/HrpcClient.hpp"
#include "store/StoreTrans.hpp"
#include "hrpc/RemoteKeeper.hpp"

#include "store/TagDataService.hpp"
#include "store/ExterDataService.hpp"
#include "store/UserDataService.hpp"
#include "store/CategoryService.hpp"
#include "store/LocalDataService.hpp"
#include "store/WikiDataService.hpp"

namespace zpds {
namespace hrpc {

template <class HttpServerT>
class MasterEndpointService :
	public zpds::query::ServiceBase<HttpServerT>,
	public zpds::hrpc::ProtoServiceBase<HttpServerT> {
public:

	/**
	* MasterEndpointService : constructor
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

	MasterEndpointService(
	    zpds::utils::SharedTable::pointer stptr,
	    typename std::shared_ptr<HttpServerT> server,
	    ::zpds::hrpc::RemoteKeeper::pointer rkeeper,
	    unsigned int scope)
		:
		zpds::query::ServiceBase<HttpServerT>(ZPDS_SERVICE_SCOPE_HTTP | ZPDS_SERVICE_SCOPE_HTTPS)
	{
		if (!(this->myscope & scope)) return; // scope mismatch

		// Endpoint : POST _zpds/master/v1/endpoint

		server->resource["/_zpds/master/v1/endpoint$"]["POST"]
		=[this,stptr,rkeeper](typename HttpServerT::RespPtr response, typename HttpServerT::ReqPtr request) {
			ZPDS_PARALLEL_ONE([this,stptr,response,request] {
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
					if (!stptr->is_master.Get()) throw zpds::BadDataException("System Not Master");

					uint64_t currtime = ZPDS_CURRTIME_MS;
					// start here
					switch(sname) {
					default: {
						LOG(INFO) << request->path << " Unknown Service ms: " << ZPDS_CURRTIME_MS - currtime;
						throw zpds::BadDataException("Service Name unknown or unimplemented");
						break;
					}

					case ::zpds::hrpc::M_MANAGE_TAGS : {
						zpds::query::TagDataRespT data;
						if (!data.ParseFromString( ::zpds::utils::S64String::Decode(request->content.string()) ))
							throw zpds::BadDataException("Bad Protobuf Format");
						// action
						zpds::store::TagDataService service; //TagData
						service.ManageDataAction(stptr, &data);
						// aftermath
						data.SerializeToString(&output);
						LOG(INFO) << request->path << " TagData Service ms: " << ZPDS_CURRTIME_MS - currtime;
						break;
					}

					case ::zpds::hrpc::M_MANAGE_EXTER : {
						zpds::query::UserDataRespT data;
						if (!data.ParseFromString( ::zpds::utils::S64String::Decode(request->content.string()) ))
							throw zpds::BadDataException("Bad Protobuf Format");
						// action
						zpds::store::ExterDataService service; //ExterData
						service.ManageDataAction(stptr, &data);
						// aftermath
						data.SerializeToString(&output);
						LOG(INFO) << request->path << " ExterData Service ms: " << ZPDS_CURRTIME_MS - currtime;
						break;
					}

					case ::zpds::hrpc::M_MANAGE_USER : {
						zpds::query::UserDataRespT data;
						if (!data.ParseFromString( ::zpds::utils::S64String::Decode(request->content.string()) ))
							throw zpds::BadDataException("Bad Protobuf Format");
						// action
						zpds::store::UserDataService service; //UserData
						service.ManageDataAction(stptr, &data);
						// aftermath
						data.SerializeToString(&output);
						LOG(INFO) << request->path << " UserData Service ms: " << ZPDS_CURRTIME_MS - currtime;
						break;
					}

					case ::zpds::hrpc::M_RESETPASS_EXTER : {
						zpds::query::ResetRespT data;
						if (!data.ParseFromString( ::zpds::utils::S64String::Decode(request->content.string()) ))
							throw zpds::BadDataException("Bad Protobuf Format");
						// action
						zpds::store::ExterDataService service; //ExterData
						service.ResetPassAction(stptr, &data);
						// aftermath
						data.SerializeToString(&output);
						LOG(INFO) << request->path << " Exter ResetPass ms: " << ZPDS_CURRTIME_MS - currtime;
						break;
					}

					case ::zpds::hrpc::M_RESETPASS_USER : {
						zpds::query::ResetRespT data;
						if (!data.ParseFromString( ::zpds::utils::S64String::Decode(request->content.string()) ))
							throw zpds::BadDataException("Bad Protobuf Format");
						// action
						zpds::store::UserDataService service; //UserData
						service.ResetPassAction(stptr, &data);
						// aftermath
						data.SerializeToString(&output);
						LOG(INFO) << request->path << " User ResetPass ms: " << ZPDS_CURRTIME_MS - currtime;
						break;
					}

					case ::zpds::hrpc::M_MANAGE_CATEGORY : {
						zpds::query::CategoryRespT data;
						if (!data.ParseFromString( ::zpds::utils::S64String::Decode(request->content.string()) ))
							throw zpds::BadDataException("Bad Protobuf Format");
						// action
						zpds::store::CategoryService service; //Category
						service.ManageDataAction(stptr, &data);
						// aftermath
						data.SerializeToString(&output);
						LOG(INFO) << request->path << " Category Service ms: " << ZPDS_CURRTIME_MS - currtime;
						break;
					}

					case ::zpds::hrpc::M_MANAGE_LOCALDATA : {
						zpds::query::ItemDataRespT data;
						if (!data.ParseFromString( ::zpds::utils::S64String::Decode(request->content.string()) ))
							throw zpds::BadDataException("Bad Protobuf Format");
						// action
						zpds::store::LocalDataService service; //LocalData
						service.ManageDataAction(stptr, &data);
						// aftermath
						data.SerializeToString(&output);
						LOG(INFO) << request->path << " LocalData Service ms: " << ZPDS_CURRTIME_MS - currtime;
						break;
					}

					case ::zpds::hrpc::M_MANAGE_WIKIDATA : {
						zpds::query::ItemDataRespT data;
						if (!data.ParseFromString( ::zpds::utils::S64String::Decode(request->content.string()) ))
							throw zpds::BadDataException("Bad Protobuf Format");
						// action
						zpds::store::WikiDataService service; //WikiData
						service.ManageDataAction(stptr, &data);
						// aftermath
						data.SerializeToString(&output);
						LOG(INFO) << request->path << " WikiData Service ms: " << ZPDS_CURRTIME_MS - currtime;
						break;
					}

					}
					// end here
					ecode=0; // something reached here

				}
				catch (::zpds::LocalException& e)
				{
					int ecode = (e.ecode()>=ZPDS_M_ERROR_MIN && e.ecode()<=ZPDS_M_ERROR_MAX) ? e.ecode() : M_UNKNOWN;
					output=e.what();
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

#endif // _ZPDS_HRPC_MASTER_ENDPOINT_SERVICE_HPP_

