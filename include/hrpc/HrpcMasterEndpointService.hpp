/**
 * @project zapdos
 * @file include/hrpc/HrpcMasterEndpointService.hpp
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
 *  HrpcMasterEndpointService.hpp : Header master only services
 *
 */
#ifndef _ZPDS_HRPC_MASTER_ENDPOINT_SERVICE_HPP_
#define _ZPDS_HRPC_MASTER_ENDPOINT_SERVICE_HPP_

#include "query/QueryBase.hpp"
#include "hrpc/ProtoServiceBase.hpp"
#include "store/StoreTrans.hpp"
#include "hrpc/RemoteKeeper.hpp"

#include "store/ProfileService.hpp"
#include "store/LookupRecordService.hpp"
#include "store/TextRecordService.hpp"
#include "store/ExtraAttribService.hpp"
#include "store/CategoryService.hpp"

namespace zpds {
namespace hrpc {

template <class HttpServerT>
class HrpcMasterEndpointService :
	public zpds::query::ServiceBase<HttpServerT>,
	protected zpds::hrpc::ProtoServiceBase<HttpServerT> {
public:

	/**
	* HrpcMasterEndpointService : constructor
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

	HrpcMasterEndpointService(
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

					// start here
					switch(sname) {
					default: {
						DLOG(INFO) << request->path;
						throw zpds::BadDataException("Service Name unknown or unimplemented");
						break;
					}
					case ::zpds::hrpc::M_NEWPROFILE : {
						zpds::query::ProfileRespT data;
						if (!data.ParseFromString( b64_decode(request->content.string()) ))
							throw zpds::BadDataException("Bad Protobuf Format");
						// action
						zpds::store::ProfileService service; //NewProfile
						service.NewProfileAction(stptr, &data);
						// aftermath
						data.SerializeToString(&output);
						DLOG(INFO) << request->path;
						break;
					}
					case ::zpds::hrpc::M_SETPROFILE : {
						zpds::query::ProfileRespT data;
						if (!data.ParseFromString( b64_decode(request->content.string()) ))
							throw zpds::BadDataException("Bad Protobuf Format");
						// action
						zpds::store::ProfileService service; //SetProfile
						service.SetProfileAction(stptr, &data);
						// aftermath
						data.SerializeToString(&output);
						DLOG(INFO) << request->path;
						break;
					}
					case ::zpds::hrpc::M_SETSIMPLETEMPLATE : {
						zpds::query::SimpleProRespT data;
						if (!data.ParseFromString( b64_decode(request->content.string()) ))
							throw zpds::BadDataException("Bad Protobuf Format");
						// action
						zpds::store::ProfileService service; //SetSimpleTemplate
						service.SetSimpleTemplateAction(stptr, &data);
						// aftermath
						data.SerializeToString(&output);
						DLOG(INFO) << request->path;
						break;
					}
					case ::zpds::hrpc::M_ADDDATA_LOCAL : {
						zpds::query::LookupParamsT data;
						if (!data.ParseFromString( b64_decode(request->content.string()) ))
							throw zpds::BadDataException("Bad Protobuf Format");
						// action
						zpds::store::LookupRecordService service; //AddData
						service.AddDataAction(stptr, &data);
						// aftermath
						data.SerializeToString(&output);
						DLOG(INFO) << request->path;
						break;
					}
					case ::zpds::hrpc::M_ADDDATA_TEXT : {
						zpds::query::TextParamsT data;
						if (!data.ParseFromString( b64_decode(request->content.string()) ))
							throw zpds::BadDataException("Bad Protobuf Format");
						// action
						zpds::store::TextRecordService service; //AddData
						service.AddDataAction(stptr, &data);
						// aftermath
						data.SerializeToString(&output);
						DLOG(INFO) << request->path;
						break;
					}
					case ::zpds::hrpc::M_ADDDATA_EXTRA : {
						zpds::query::ExtraParamsT data;
						if (!data.ParseFromString( b64_decode(request->content.string()) ))
							throw zpds::BadDataException("Bad Protobuf Format");
						// action
						zpds::store::ExtraAttribService service; //AddData
						service.AddDataAction(stptr, &data);
						// aftermath
						data.SerializeToString(&output);
						DLOG(INFO) << request->path;
						break;
					}
					case ::zpds::hrpc::M_ADDDATA_CAT : {
						zpds::query::CatParamsT data;
						if (!data.ParseFromString( b64_decode(request->content.string()) ))
							throw zpds::BadDataException("Bad Protobuf Format");
						// action
						zpds::store::CategoryService service; //AddData
						service.AddDataAction(stptr, &data);
						// aftermath
						data.SerializeToString(&output);
						DLOG(INFO) << request->path;
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
					this->ServiceOKAction(response,request,b64_encode(output));
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

