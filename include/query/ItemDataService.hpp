/**
 * @project zapdos
 * @file include/query/ItemDataService.hpp
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
 *  ItemDataService.hpp : ItemData web service Endpoint for localdata, wikidata
 *
 */
#ifndef _ZPDS_QUERY_ITEMDATA_SERVICE_HPP_
#define _ZPDS_QUERY_ITEMDATA_SERVICE_HPP_

#include "query/QueryBase.hpp"
#include "hrpc/HrpcClient.hpp"
#include "utils/PrintWith.hpp"

#include "store/LocalDataService.hpp"
#include "store/WikiDataService.hpp"

#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>

namespace zpds {
namespace query {

template <class HttpServerT>
class ItemDataService : public zpds::query::ServiceBase<HttpServerT> {
public:

	/**
	* ItemDataService : constructor
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

	ItemDataService(
	    zpds::utils::SharedTable::pointer stptr,
	    typename std::shared_ptr<HttpServerT> server,
	    ::zpds::query::HelpQuery::pointer helpquery,
	    unsigned int scope)
		: zpds::query::ServiceBase<HttpServerT>(ZPDS_SERVICE_SCOPE_HTTP | ZPDS_SERVICE_SCOPE_HTTPS)
	{
		if (!(this->myscope & scope)) return; // scope mismatch

		// Endpoint : POST _admin/api/v1/(localdata|wikidata)/(create|upsert|update|merge|delete)
		for (auto& ityp : std::vector<std::string>({ "localdata", "wikidata" }) ) {
			for (auto& gtyp : std::vector<std::string>({ "create", "upsert", "update", "merge", "delete" }) ) {
				std::string&& posturl = "POST _admin/api/v1/" + ityp + "/" + gtyp;
				std::string&& lineone = "for exter users to " + gtyp + " " + ityp + " data";

				helpquery->add({scope, posturl, {
						lineone,
						"using name(*) , only admin or manager can change",
						"Parameters:",
						"username : updater name",
						"sessionkey: updater session key",
						"payload: contains payload for data , must have unique_id ",
						"payloads: array of above"
					}
				});
			}
		}



		const std::string&& itemdata_write{"/_admin/api/v1/(localdata|wikidata)/"
		                                   "(create|upsert|update|merge|delete"
		                                   ")$"};
		server->resource[itemdata_write]["POST"]
		=[this,stptr](typename HttpServerT::RespPtr response, typename HttpServerT::ReqPtr request) {
			ZPDS_PARALLEL_ONE([this,stptr,response,request] {
				uint64_t currtime = ZPDS_CURRTIME_MS;
				bool ok=false;

				try
				{
					bool use_json = this->JsonRequest(request);
					zpds::query::ItemDataRespT getp;
					if (use_json) {
						int stat = json2pb( request->content.string(), &getp);
						if (stat<0) throw InitialException("Bad Json Format",M_BAD_JSON);
					}
					else {
						throw InitialException("Only Json Format",M_BAD_JSON);
					}

					if (getp.username().empty())
						throw InitialException("Bad Json Data",M_BAD_JSON);

					if (getp.payloads_size() > ZPDS_UPLOAD_MAX_RECORDS)
						throw zpds::BadDataException("Cannot handle that many records, max exceeded",M_INVALID_PARAM);

					bool hide_counters=false;
					DLOG(INFO) << getp.DebugString() << std::endl;

					// only exter can write
					getp.set_usertype( ::zpds::store::K_EXTERDATA );

					// set action
					// default all fields
					getp.set_fields( ZPDS_MAX_HEX_VALUE_FIFTEEN );
					if ( request->path_match[2]=="create" ) {
						getp.set_action(::zpds::query::W_CREATE);
					}
					else if ( request->path_match[2]=="upsert" ) {
						getp.set_action(::zpds::query::W_UPSERT);
					}
					else if ( request->path_match[2]=="update" ) {
						getp.set_action(::zpds::query::W_UPDATE);
					}
					else if ( request->path_match[2]=="merge" ) {
						getp.set_action(::zpds::query::W_UMERGE);
					}
					else if ( request->path_match[2]=="delete" ) {
						getp.set_action(::zpds::query::W_DELETE);
					}
					else throw zpds::BadCodeException("Unknown API used");

					if (!stptr->is_ready.Get()) throw zpds::BadDataException("System Not Ready");

					// action
					if (stptr->is_master.Get()) {
						::zpds::store::ItemDataService::pointer rs;
						if ( request->path_match[1]=="localdata" )
							rs = ::zpds::store::LocalDataService::CreateBase();
						else if ( request->path_match[1]=="wikidata" )
							rs = ::zpds::store::WikiDataService::CreateBase();
						else throw zpds::BadCodeException("Unknown API used");
						rs->ManageDataAction(stptr,&getp);
					}
					else {
						zpds::hrpc::HrpcClient hclient;
						if ( request->path_match[1]=="localdata" )
							hclient.SendToMaster(stptr,::zpds::hrpc::M_MANAGE_LOCALDATA,&getp);
						else if ( request->path_match[1]=="wikidata" )
							hclient.SendToMaster(stptr,::zpds::hrpc::M_MANAGE_WIKIDATA,&getp);
						else throw zpds::BadCodeException("Unknown API used");
					}

					// aftermath

					std::string output;
					if (hide_counters) {
						getp.mutable_status()->set_inputcount(0);
						getp.mutable_status()->set_updatecount(0);
					}
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

		// Endpoint : POST (_admin|_user)/api/v1/(localdata|wikidata)/(getone|getmany)
		for (auto& utyp : std::vector<std::string>({ "_admin", "_user" }) ) {
			for (auto& ityp : std::vector<std::string>({ "localdata", "wikidata" }) ) {
				for (auto& gtyp : std::vector<std::string>({ "getone", "getmany" }) ) {
					std::string&& posturl = "POST " + utyp + "/api/v1/" + ityp + "/" + gtyp;
					std::string&& lineone = "for users to read " + ityp + " data";
					std::string&& linetwo = (gtyp=="getone") ?
					                        "payload: contains payload for name"
					                        : "payloads: contains array of payload for name";

					helpquery->add({scope, posturl, {
							lineone,
							"using name(*)",
							"Parameters:",
							"username : reader name",
							"sessionkey: reader session key",
							linetwo
						}
					});
				}
			}
		}

		server->resource["/(_admin|_user)/api/v1/(localdata|wikidata)/(getone|getmany)$"]["POST"]
		=[this,stptr](typename HttpServerT::RespPtr response, typename HttpServerT::ReqPtr request) {
			ZPDS_PARALLEL_ONE([this,stptr,response,request] {
				uint64_t currtime = ZPDS_CURRTIME_MS;
				bool ok=false;

				try
				{
					bool use_json = this->JsonRequest(request);
					zpds::query::ItemDataRespT getp;
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

					if (request->path_match[1] == "_admin") {
						getp.set_usertype( ::zpds::store::K_EXTERDATA );
					}
					else if (request->path_match[1] == "_user") {
						getp.set_usertype( ::zpds::store::K_USERDATA );
					}

					// set action
					if (request->path_match[3]=="getone" ) {
						getp.set_fields( ZPDS_MAX_HEX_VALUE_FIFTEEN );
						getp.clear_payloads();
					}
					else if (request->path_match[3]=="getmany" ) {
						getp.set_fields( ZPDS_MAX_HEX_VALUE_FIFTEEN );
						getp.clear_payload();
					}
					else throw zpds::BadCodeException("Unknown API used");

					if (!stptr->is_ready.Get()) throw zpds::BadDataException("System Not Ready");

					// action
					{
						::zpds::store::ItemDataService::pointer rs;
						if ( request->path_match[2]=="localdata" )
							rs = ::zpds::store::LocalDataService::CreateBase();
						else if ( request->path_match[2]=="wikidata" )
							rs = ::zpds::store::WikiDataService::CreateBase();
						else throw zpds::BadCodeException("Unknown API used");
						rs->ReadDataAction(stptr,&getp);
					}

					// aftermath

					std::string output;
					// clear getp fields
					getp.clear_username();
					getp.clear_sessionkey();
					getp.clear_status();
					getp.clear_action();
					getp.clear_fields();
					getp.clear_usertype();
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

#endif // _ZPDS_QUERY_ITEMDATA_SERVICE_HPP_

