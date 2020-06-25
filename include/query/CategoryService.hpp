/**
 * @project zapdos
 * @file include/query/CategoryService.hpp
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
 *  CategoryService.hpp : Category Manipulation Endpoint
 *
 */
#ifndef _ZPDS_QUERY_CATEGORY_SERVICE_HPP_
#define _ZPDS_QUERY_CATEGORY_SERVICE_HPP_

#include "query/QueryBase.hpp"
#include "hrpc/HrpcClient.hpp"

#include "store/CategoryService.hpp"

#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>

namespace zpds {
namespace query {

template <class HttpServerT>
class CategoryService : public zpds::query::ServiceBase<HttpServerT> {
public:

	/**
	* CategoryService : constructor
	*
	* @param stptr
	*   zpds::utils::SharedTable::pointer stptr
	*
	* @param server
	*   std::shared_ptr<HttpServerT> server
	*
	* @param helpquery
	*   ::zpds::query::HelpQuery::pointer helpquery
	*
	* @param scope
	*   const unsigned int scope check
	*
	* @return
	*   none
	*/

	CategoryService(
	    zpds::utils::SharedTable::pointer stptr,
	    typename std::shared_ptr<HttpServerT> server,
	    ::zpds::query::HelpQuery::pointer helpquery,
	    unsigned int scope)
		: zpds::query::ServiceBase<HttpServerT>(ZPDS_SERVICE_SCOPE_HTTP | ZPDS_SERVICE_SCOPE_HTTPS)
	{
		if (!(this->myscope & scope)) return; // scope mismatch

		// Endpoint : POST _admin/api/v1/category/create

		helpquery->add({scope, "POST _admin/api/v1/category/create", {
				"create hosts and related data",
				"Parameters:",
				"username : updater name",
				"sessionkey: updater session key",
				"payload: contains payload for category, name(*), lang(def: EN), description ",
				"payloads: array of above"
			}
		});

		// Endpoint : POST _admin/api/v1/category/upsert

		helpquery->add({scope, "POST _admin/api/v1/category/upsert", {
				"upsert hosts and related data",
				"Parameters:",
				"username : updater name",
				"sessionkey: updater session key",
				"payload: contains payload for category, name(*), lang(def: EN), description",
				"payloads: array of above"
			}
		});

		// Endpoint : POST _admin/api/v1/category/update

		helpquery->add({scope, "POST _admin/api/v1/category/update", {
				"update hosts and related data",
				"Parameters:",
				"username : updater name",
				"sessionkey: updater session key",
				"payload: contains payload for category, name(*), lang(def: EN), description",
				"payloads: array of above"
			}
		});

		// Endpoint : POST _admin/api/v1/category/delete

		helpquery->add({scope, "POST _admin/api/v1/category/delete", {
				"delete hosts and related data",
				"Parameters:",
				"username : updater name",
				"sessionkey: updater session key",
				"payload: contains payload for category, name(*)",
				"payloads: array of above"
			}
		});

		server->resource["/_admin/api/v1/category/(create|upsert|update|delete)$"]["POST"]
		=[this,stptr](typename HttpServerT::RespPtr response, typename HttpServerT::ReqPtr request) {
			ZPDS_PARALLEL_ONE([this,stptr,response,request] {
				uint64_t currtime = ZPDS_CURRTIME_MS;
				bool ok=false;

				try
				{
					bool use_json = this->JsonRequest(request);
					zpds::query::CategoryRespT getp;
					if (use_json) {
						int stat = json2pb( request->content.string(), &getp);
						if (stat<0) throw InitialException("Bad Json Format",M_BAD_JSON);
					}
					else {
						throw InitialException("Only Json Format",M_BAD_JSON);
					}

					if (getp.username().empty())
						throw InitialException("Bad Json Data",M_BAD_JSON);

					if (getp.payloads_size() > ZPDS_NUPLOAD_MAX_RECORDS)
						throw zpds::BadDataException("Cannot handle that many records, max exceeded",M_INVALID_PARAM);

					// set action
					if (request->path_match[1]=="create" ) getp.set_action(::zpds::query::W_CREATE);
					else if (request->path_match[1]=="upsert" ) getp.set_action(::zpds::query::W_UPSERT);
					else if (request->path_match[1]=="update" ) getp.set_action(::zpds::query::W_UPDATE);
					else if (request->path_match[1]=="delete" ) getp.set_action(::zpds::query::W_DELETE);
					else throw zpds::BadCodeException("Unknown API used");

					if (!stptr->is_ready.Get()) throw zpds::BadDataException("System Not Ready");

					// action
					if (stptr->is_master.Get()) {
						::zpds::store::CategoryService rs;
						rs.ManageDataAction(stptr,&getp);
					}
					else {
						zpds::hrpc::HrpcClient hclient;
						hclient.SendToMaster(stptr,::zpds::hrpc::M_MANAGE_CATEGORY,&getp);
					}

					// aftermath

					std::string output;
					pb2json(getp.mutable_status(),output);

					ok=true;
					this->HttpOKAction(response,request,200,"OK","application/json",output);
				}
				catch (zpds::BaseException& e)
				{
					std::string output = err2json(e.ecode(),e.what());
					this->HttpOKAction(response,request,200,"OK","application/json",output);
				}
				catch (std::exception& e)
				{
					std::string output = err2json(M_UNKNOWN,e.what());
					this->HttpOKAction(response,request,200,"OK","application/json",output);
				}
				catch (...)
				{
					this->HttpErrorAction(response,request,500,"INTERNAL SERVER ERROR");
				}
				LOG(INFO) << request->path << " ms: " << ZPDS_CURRTIME_MS - currtime;
			});
		};

		// Endpoint : POST _admin/api/v1/category/getone

		helpquery->add({scope, "POST _admin/api/v1/category/getone", {
				"for exter users to read category and related data",
				"using name(*)",
				"Parameters:",
				"username : reader name",
				"sessionkey: reader session key",
				"payload: contains payload for name"
			}
		});

		// Endpoint : POST _admin/api/v1/category/getmany

		helpquery->add({scope, "POST _admin/api/v1/category/getmany", {
				"for exter users to read category and related data",
				"using name(*)",
				"Parameters:",
				"username : reader name",
				"sessionkey: reader session key",
				"payloads: contains array of payload for name"
			}
		});

		server->resource["/_admin/api/v1/category/(getone|getmany)$"]["POST"]
		=[this,stptr](typename HttpServerT::RespPtr response, typename HttpServerT::ReqPtr request) {
			ZPDS_PARALLEL_ONE([this,stptr,response,request] {
				uint64_t currtime = ZPDS_CURRTIME_MS;
				bool ok=false;

				try
				{
					bool use_json = this->JsonRequest(request);
					zpds::query::CategoryRespT getp;
					if (use_json) {
						int stat = json2pb( request->content.string(), &getp);
						if (stat<0) throw InitialException("Bad Json Format",M_BAD_JSON);
					}
					else {
						throw InitialException("Only Json Format",M_BAD_JSON);
					}

					if (getp.username().empty())
						throw InitialException("Bad Json Data",M_BAD_JSON);

					if (getp.payloads_size() > ZPDS_NUPLOAD_MAX_RECORDS)
						throw zpds::BadDataException("Cannot handle that many records, max exceeded",M_INVALID_PARAM);

					DLOG(INFO) << getp.DebugString() << std::endl;

					// set action
					if (request->path_match[1]=="getone" ) {
						getp.clear_payloads();
					}
					else if (request->path_match[1]=="getmany" ) {
						getp.clear_payload();
					}
					else throw zpds::BadCodeException("Unknown API used");

					if (!stptr->is_ready.Get()) throw zpds::BadDataException("System Not Ready");

					// action
					::zpds::store::CategoryService rs;
					rs.ReadDataAction(stptr,&getp);

					// aftermath

					std::string output;
					// clear getp fields
					getp.clear_username();
					getp.clear_sessionkey();
					getp.clear_status();
					getp.clear_action();
					pb2json(&getp,output);

					ok=true;
					this->HttpOKAction(response,request,200,"OK","application/json",output);
				}
				catch (zpds::BaseException& e)
				{
					std::string output = err2json(e.ecode(),e.what());
					this->HttpOKAction(response,request,200,"OK","application/json",output);
				}
				catch (std::exception& e)
				{
					std::string output = err2json(M_UNKNOWN,e.what());
					this->HttpOKAction(response,request,200,"OK","application/json",output);
				}
				catch (...)
				{
					this->HttpErrorAction(response,request,500,"INTERNAL SERVER ERROR");
				}
				LOG(INFO) << request->path << " ms: " << ZPDS_CURRTIME_MS - currtime;
			});
		};

		// end
	}


private:

};
} // namespace query
} // namespace zpds

#endif // _ZPDS_QUERY_CATEGORY_SERVICE_HPP_


