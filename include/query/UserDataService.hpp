/**
 * @project zapdos
 * @file include/query/UserDataService.hpp
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
 *  UserDataService.hpp : User and Exter Data Management Endpoint
 *
 */
#ifndef _ZPDS_QUERY_USERDATA_SERVICE_HPP_
#define _ZPDS_QUERY_USERDATA_SERVICE_HPP_

#include "query/QueryBase.hpp"
#include "hrpc/HrpcClient.hpp"

#include "store/ExterDataService.hpp"

#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>

namespace zpds {
namespace query {

template <class HttpServerT>
class UserDataService : public zpds::query::ServiceBase<HttpServerT> {
public:

	/**
	* UserDataService : constructor
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

	UserDataService(
	    zpds::utils::SharedTable::pointer stptr,
	    typename std::shared_ptr<HttpServerT> server,
	    ::zpds::query::HelpQuery::pointer helpquery,
	    unsigned int scope)
		: zpds::query::ServiceBase<HttpServerT>(ZPDS_SERVICE_SCOPE_HTTP | ZPDS_SERVICE_SCOPE_HTTPS)
	{
		if (!(this->myscope & scope)) return; // scope mismatch

		// Endpoint : POST _admin/api/v1/userdata/create

		helpquery->add({scope, "POST _admin/api/v1/userdata/create", {
				"for exter users to create regular users and related data",
				"using name(*)",
				"Parameters:",
				"username : updater name",
				"sessionkey: updater session key",
				"payload: contains payload for name, lang(def: EN), passkey(*) description mobile email given_name",
				"payloads: array of above"
			}
		});

		// Endpoint : POST _admin/api/v1/userdata/upsert

		helpquery->add({scope, "POST _admin/api/v1/userdata/upsert", {
				"for exter users to upsert regular users and related data",
				"using name(*)",
				"Parameters:",
				"username : updater name",
				"sessionkey: updater session key",
				"payload: contains payload for name, lang(def: EN), passkey(*) description mobile email given_name",
				"payloads: array of above"
			}
		});

		// Endpoint : POST _admin/api/v1/userdata/update

		helpquery->add({scope, "POST _admin/api/v1/userdata/update", {
				"for exter users to update regular users and related data",
				"using name(*) , only admin or manager can change",
				"Parameters:",
				"username : updater name",
				"sessionkey: updater session key",
				"payload: contains payload for name, lang(def: EN), passkey description mobile email given_name",
				"payloads: array of above"
			}
		});

		// Endpoint : POST _admin/api/v1/userdata/delete

		helpquery->add({scope, "POST _admin/api/v1/userdata/delete", {
				"for exter users to delete regular users and related data",
				"using name(*) , only admin or manager can change",
				"Parameters:",
				"username : updater name",
				"sessionkey: updater session key",
				"payload: contains payload for name",
				"payloads: array of above"
			}
		});

		// Endpoint : POST _admin/api/v1/userdata/update_passkey

		helpquery->add({scope, "POST _admin/api/v1/userdata/update_passkey", {
				"for exter users to update passkey",
				"using name(*) , only admin or manager can change",
				"Parameters:",
				"username : updater name",
				"sessionkey: updater session key",
				"payload: contains payload for name, lang(def: EN), passkey"
			}
		});

		// Endpoint : POST _admin/api/v1/userdata/update_description

		helpquery->add({scope, "POST _admin/api/v1/userdata/update_description", {
				"for exter users to update description",
				"using name(*) , only admin or manager can change",
				"Parameters:",
				"username : updater name",
				"sessionkey: updater session key",
				"payload: contains payload for name, lang(def: EN), description"
			}
		});

		// Endpoint : POST _admin/api/v1/userdata/update_mobile

		helpquery->add({scope, "POST _admin/api/v1/userdata/update_mobile", {
				"for exter users to update mobile",
				"using name(*) , only admin or manager can change",
				"Parameters:",
				"username : updater name",
				"sessionkey: updater session key",
				"payload: contains payload for name, lang(def: EN), mobile"
			}
		});

		// Endpoint : POST _admin/api/v1/userdata/update_email

		helpquery->add({scope, "POST _admin/api/v1/userdata/update_email", {
				"for exter users to update email",
				"using name(*) , only admin or manager can change",
				"Parameters:",
				"username : updater name",
				"sessionkey: updater session key",
				"payload: contains payload for name, lang(def: EN), email"
			}
		});

		// Endpoint : POST _admin/api/v1/userdata/update_given_name

		helpquery->add({scope, "POST _admin/api/v1/userdata/update_given_name", {
				"for exter users to update given_name",
				"using name(*) , only admin or manager can change",
				"Parameters:",
				"username : updater name",
				"sessionkey: updater session key",
				"payload: contains payload for name, lang(def: EN), given_name "
			}
		});

		// Endpoint : POST _admin/api/v1/userdata/add_tags

		helpquery->add({scope, "POST _admin/api/v1/userdata/add_tags", {
				"for exter users to update tags",
				"using name(*) , only admin or manager can change",
				"Parameters:",
				"username : updater name",
				"sessionkey: updater session key",
				"payload: contains payload for name, lang(def: EN), tags "
			}
		});

		// Endpoint : POST _admin/api/v1/userdata/add_categories

		helpquery->add({scope, "POST _admin/api/v1/userdata/add_categories", {
				"for exter users to update categories",
				"using name(*) , only admin or manager can change",
				"Parameters:",
				"username : updater name",
				"sessionkey: updater session key",
				"payload: contains payload for name, lang(def: EN), categories "
			}
		});

		// Endpoint : POST _user/api/v1/userdata/update_passkey

		helpquery->add({scope, "POST _user/api/v1/userdata/update_passkey", {
				"for users to update own passkey",
				"Parameters:",
				"username : updater name",
				"sessionkey: updater session key",
				"payload: contains payload for passkey"
			}
		});

		// Endpoint : POST _user/api/v1/userdata/update_description

		helpquery->add({scope, "POST _user/api/v1/userdata/update_description", {
				"for users to update own description",
				"Parameters:",
				"username : updater name",
				"sessionkey: updater session key",
				"payload: contains payload for description"
			}
		});

		// Endpoint : POST _user/api/v1/userdata/update_given_name

		helpquery->add({scope, "POST _user/api/v1/userdata/update_given_name", {
				"for users to update own given_name",
				"Parameters:",
				"username : updater name",
				"sessionkey: updater session key",
				"payload: contains payload for given_name"
			}
		});

		const std::string&& userdata_write{"/(_admin|_user)/api/v1/userdata/"
		                                   "(create|upsert|update|delete"
		                                   "|update_passkey|update_description|update_mobile|update_email|update_given_name"
		                                   "|add_tags|add_categories"
		                                   ")$"};
		server->resource[userdata_write]["POST"]
		=[this,stptr](typename HttpServerT::RespPtr response, typename HttpServerT::ReqPtr request) {
			ZPDS_PARALLEL_ONE([this,stptr,response,request] {
				uint64_t currtime = ZPDS_CURRTIME_MS;
				bool ok=false;

				try
				{
					bool use_json = this->JsonRequest(request);
					zpds::query::UserDataRespT getp;
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

					bool hide_counters=false;
					DLOG(INFO) << getp.DebugString() << std::endl;
					if (request->path_match[1] == "_admin") {
						getp.set_keytype( ::zpds::store::K_EXTERDATA );
					}
					else if (request->path_match[1] == "_user") {
						getp.set_keytype( ::zpds::store::K_USERDATA ) ;
					}
					// set action
					// default all fields
					getp.set_fields( ZPDS_MAX_HEX_VALUE_FIFTEEN );
					if (request->path_match[1]=="_admin" && request->path_match[2]=="create" ) {
						getp.set_action(::zpds::query::W_CREATE);
					}
					else if (request->path_match[1]=="_admin" && request->path_match[2]=="upsert" ) {
						getp.set_action(::zpds::query::W_UPSERT);
					}
					else if (request->path_match[1]=="_admin" && request->path_match[2]=="update" ) {
						getp.set_action(::zpds::query::W_UPDATE);
					}
					else if (request->path_match[1]=="_admin" && request->path_match[2]=="delete" ) {
						getp.set_action(::zpds::query::W_DELETE);
					}
					else if (request->path_match[2]=="update_passkey" ) {
						if (request->path_match[1]=="_user")
							getp.mutable_payload()->set_name( getp.username() );
						getp.set_action(::zpds::query::W_UPDATE);
						getp.set_fields( ZPDS_UDFLD_PASSKEY );
						getp.clear_payloads();
						hide_counters=true;
					}
					else if (request->path_match[2]=="update_description" ) {
						if (request->path_match[1]=="_user")
							getp.mutable_payload()->set_name( getp.username() );
						getp.set_action(::zpds::query::W_UPDATE);
						getp.set_fields( ZPDS_UDFLD_INFO );
						getp.clear_payloads();
						hide_counters=true;
					}
					else if (request->path_match[1]=="_admin" && request->path_match[2]=="update_mobile" ) {
						getp.set_action(::zpds::query::W_UPDATE);
						getp.set_fields( ZPDS_UDFLD_MOBILE );
						getp.clear_payloads();
						hide_counters=true;
					}
					else if (request->path_match[1]=="_admin" && request->path_match[2]=="update_email" ) {
						getp.set_action(::zpds::query::W_UPDATE);
						getp.set_fields( ZPDS_UDFLD_EMAIL );
						getp.clear_payloads();
						hide_counters=true;
					}
					else if (request->path_match[2]=="update_given_name" ) {
						if (request->path_match[1]=="_user")
							getp.mutable_payload()->set_name( getp.username() );
						getp.set_action(::zpds::query::W_UPDATE);
						getp.set_fields( ZPDS_UDFLD_GNAME );
						getp.clear_payloads();
						hide_counters=true;
					}
					else if (request->path_match[1]=="_admin" && request->path_match[2]=="add_tags" ) {
						getp.set_action(::zpds::query::W_UMERGE);
						getp.set_fields( ZPDS_UDFLD_TAGS );
						getp.clear_payloads();
						hide_counters=true;
					}
					else if (request->path_match[1]=="_admin" && request->path_match[2]=="add_categories" ) {
						getp.set_action(::zpds::query::W_UMERGE);
						getp.set_fields( ZPDS_UDFLD_CATEGORIES );
						getp.clear_payloads();
						hide_counters=true;
					}
					else throw zpds::BadCodeException("Unknown API used");

					if (!stptr->is_ready.Get()) throw zpds::BadDataException("System Not Ready");

					// action
					if (stptr->is_master.Get()) {
						::zpds::store::UserDataService rs;
						rs.ManageDataAction(stptr,&getp);
					}
					else {
						zpds::hrpc::HrpcClient hclient;
						hclient.SendToMaster(stptr,::zpds::hrpc::M_MANAGE_USER,&getp);
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

		// Endpoint : POST _admin/api/v1/userdata/getone

		helpquery->add({scope, "POST _admin/api/v1/userdata/getone", {
				"for exter users to read exter users and related data",
				"using name(*)",
				"Parameters:",
				"username : reader name",
				"sessionkey: reader session key",
				"payload: contains payload for name"
			}
		});

		// Endpoint : POST _admin/api/v1/userdata/getmany

		helpquery->add({scope, "POST _admin/api/v1/userdata/getmany", {
				"for exter users to read exter users and related data",
				"using name(*)",
				"Parameters:",
				"username : reader name",
				"sessionkey: reader session key",
				"payloads: contains array of payload for name"
			}
		});

		// Endpoint : POST _user/api/v1/userdata/getone

		helpquery->add({scope, "POST _user/api/v1/userdata/getone", {
				"for exter users to read exter users and related data",
				"using name(*)",
				"Parameters:",
				"username : reader name",
				"sessionkey: reader session key",
				"payload: contains payload for name"
			}
		});

		// Endpoint : POST _user/api/v1/userdata/getmany

		helpquery->add({scope, "POST _user/api/v1/userdata/getmany", {
				"for exter users to read exter users and related data",
				"using name(*)",
				"Parameters:",
				"username : reader name",
				"sessionkey: reader session key",
				"payloads: contains array of payload for name"
			}
		});

		server->resource["/(_admin|_user)/api/v1/userdata/(getone|getmany)$"]["POST"]
		=[this,stptr](typename HttpServerT::RespPtr response, typename HttpServerT::ReqPtr request) {
			ZPDS_PARALLEL_ONE([this,stptr,response,request] {
				uint64_t currtime = ZPDS_CURRTIME_MS;
				bool ok=false;

				try
				{
					bool use_json = this->JsonRequest(request);
					zpds::query::UserDataRespT getp;
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
						getp.set_keytype( ::zpds::store::K_EXTERDATA );
					}
					else if (request->path_match[1] == "_user") {
						getp.set_keytype( ::zpds::store::K_USERDATA ) ;
					}

					// set action
					if (request->path_match[2]=="getone" ) {
						getp.set_fields( ZPDS_MAX_HEX_VALUE_FIFTEEN );
						getp.clear_payloads();
					}
					else if (request->path_match[1] == "_admin" && request->path_match[2]=="getmany" ) {
						getp.set_fields( ZPDS_MAX_HEX_VALUE_FIFTEEN );
						getp.clear_payload();
					}
					else throw zpds::BadCodeException("Unknown API used");

					if (!stptr->is_ready.Get()) throw zpds::BadDataException("System Not Ready");

					// action
					::zpds::store::UserDataService rs;
					rs.ReadDataAction(stptr,&getp);

					// aftermath

					std::string output;
					// clear getp fields
					getp.clear_username();
					getp.clear_sessionkey();
					getp.clear_status();
					getp.clear_action();
					getp.clear_fields();
					getp.clear_is_self();
					getp.clear_keytype();
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

		// Endpoint : POST _admin/api/v1/exterdata/create

		helpquery->add({scope, "POST _admin/api/v1/exterdata/create", {
				"for admin users to create other exter users and related data",
				"Parameters:",
				"username : updater name",
				"sessionkey: updater session key",
				"payload: contains payload for name(*), lang(def: EN), passkey(*) description mobile email given_name",
				"payloads: array of above"
			}
		});

		// Endpoint : POST _admin/api/v1/exterdata/upsert

		helpquery->add({scope, "POST _admin/api/v1/exterdata/upsert", {
				"for admin users to upsert other exter users and related data",
				"Parameters:",
				"username : updater name",
				"sessionkey: updater session key",
				"payload: contains payload for name(*), lang(def: EN), passkey(*) description mobile email given_name",
				"payloads: array of above"
			}
		});

		// Endpoint : POST _admin/api/v1/exterdata/update

		helpquery->add({scope, "POST _admin/api/v1/exterdata/update", {
				"for admin users to update other exter users and related data",
				"Parameters:",
				"username : updater name",
				"sessionkey: updater session key",
				"payload: contains payload for name(*), lang(def: EN), passkey description mobile email given_name",
				"payloads: array of above"
			}
		});

		// Endpoint : POST _admin/api/v1/exterdata/delete

		helpquery->add({scope, "POST _admin/api/v1/exterdata/delete", {
				"for admin users to delete other exter users and related data",
				"Parameters:",
				"username : updater name",
				"sessionkey: updater session key",
				"payload: contains payload for name(*)",
				"payloads: array of above"
			}
		});

		// Endpoint : POST _admin/api/v1/exterdata/update_passkey

		helpquery->add({scope, "POST _admin/api/v1/exterdata/update_passkey", {
				"for exter users to update passkey",
				"Parameters:",
				"username : updater name",
				"sessionkey: updater session key",
				"payload: contains payload for name(*), lang(def: EN), passkey"
			}
		});

		// Endpoint : POST _admin/api/v1/exterdata/update_description

		helpquery->add({scope, "POST _admin/api/v1/exterdata/update_description", {
				"for exter users to update description",
				"Parameters:",
				"username : updater name",
				"sessionkey: updater session key",
				"payload: contains payload for name(*), lang(def: EN), description"
			}
		});

		// Endpoint : POST _admin/api/v1/exterdata/update_mobile

		helpquery->add({scope, "POST _admin/api/v1/exterdata/update_mobile", {
				"for exter users to update mobile",
				"Parameters:",
				"username : updater name",
				"sessionkey: updater session key",
				"payload: contains payload for name(*), lang(def: EN), mobile"
			}
		});

		// Endpoint : POST _admin/api/v1/exterdata/update_email

		helpquery->add({scope, "POST _admin/api/v1/exterdata/update_email", {
				"for exter users to update email",
				"Parameters:",
				"username : updater name",
				"sessionkey: updater session key",
				"payload: contains payload for name(*), lang(def: EN), email"
			}
		});

		// Endpoint : POST _admin/api/v1/exterdata/update_given_name

		helpquery->add({scope, "POST _admin/api/v1/exterdata/update_given_name", {
				"for exter users to update given_name",
				"Parameters:",
				"username : updater name",
				"sessionkey: updater session key",
				"payload: contains payload for name(*), lang(def: EN), given_name "
			}
		});

		// Endpoint : POST _admin/api/v1/exterdata/add_tags

		helpquery->add({scope, "POST _admin/api/v1/exterdata/add_tags", {
				"for exter users to update given_name",
				"Parameters:",
				"username : updater name",
				"sessionkey: updater session key",
				"payload: contains payload for name(*), lang(def: EN), given_name "
			}
		});

		// Endpoint : POST _admin/api/v1/exterdata/add_profiles

		helpquery->add({scope, "POST _admin/api/v1/exterdata/add_profiles", {
				"for users to update own profiles",
				"Parameters:",
				"username : updater name",
				"sessionkey: updater session key",
				"payload: contains payload for profiles, aka label, description, qtype, rules"
			}
		});


		const std::string&& exterdata_write{"/_admin/api/v1/exterdata/"
		                                    "(create|upsert|update|delete"
		                                    "|update_passkey|update_description|update_mobile|update_email|update_given_name|add_tags"
		                                    "|add_profiles"
		                                    ")$"};
		server->resource[exterdata_write]["POST"]
		=[this,stptr](typename HttpServerT::RespPtr response, typename HttpServerT::ReqPtr request) {
			ZPDS_PARALLEL_ONE([this,stptr,response,request] {
				uint64_t currtime = ZPDS_CURRTIME_MS;
				bool ok=false;

				try
				{
					bool use_json = this->JsonRequest(request);
					zpds::query::UserDataRespT getp;
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

					bool hide_counters=false;
					DLOG(INFO) << getp.DebugString() << std::endl;
					DLOG(INFO) << request->path_match[1]<< std::endl;

#define SET_NAME_IF_NOT_SET if (getp.payload().name().empty()) getp.mutable_payload()->set_name( getp.username() );
					// set action
					if (request->path_match[1]=="create" ) {
						getp.set_action(::zpds::query::W_CREATE);
						getp.set_fields( ZPDS_MAX_HEX_VALUE_FIFTEEN );
					}
					else if (request->path_match[1]=="upsert" ) {
						getp.set_action(::zpds::query::W_UPSERT);
						getp.set_fields( ZPDS_MAX_HEX_VALUE_FIFTEEN );
					}
					else if (request->path_match[1]=="update" ) {
						getp.set_action(::zpds::query::W_UPDATE);
						getp.set_fields( ZPDS_MAX_HEX_VALUE_FIFTEEN );
					}
					else if (request->path_match[1]=="delete" ) {
						getp.set_action(::zpds::query::W_DELETE);
					}
					else if (request->path_match[1]=="update_passkey" ) {
						getp.set_action(::zpds::query::W_UPDATE);
						getp.set_fields( ZPDS_UDFLD_PASSKEY );
						getp.clear_payloads();
						hide_counters=true;
						SET_NAME_IF_NOT_SET
					}
					else if (request->path_match[1]=="update_description" ) {
						getp.set_action(::zpds::query::W_UPDATE);
						getp.set_fields( ZPDS_UDFLD_INFO );
						getp.clear_payloads();
						hide_counters=true;
						SET_NAME_IF_NOT_SET
					}
					else if (request->path_match[1]=="update_mobile" ) {
						getp.set_action(::zpds::query::W_UPDATE);
						getp.set_fields( ZPDS_UDFLD_MOBILE );
						getp.clear_payloads();
						hide_counters=true;
						SET_NAME_IF_NOT_SET
					}
					else if (request->path_match[1]=="update_email" ) {
						getp.set_action(::zpds::query::W_UPDATE);
						getp.set_fields( ZPDS_UDFLD_EMAIL );
						getp.clear_payloads();
						hide_counters=true;
						SET_NAME_IF_NOT_SET
					}
					else if (request->path_match[1]=="update_given_name" ) {
						getp.set_action(::zpds::query::W_UPDATE);
						getp.set_fields( ZPDS_UDFLD_GNAME );
						getp.clear_payloads();
						hide_counters=true;
						SET_NAME_IF_NOT_SET
					}
					else if (request->path_match[1]=="add_tags" ) {
						getp.set_action(::zpds::query::W_UMERGE);
						getp.set_fields( ZPDS_UDFLD_TAGS );
						getp.clear_payloads();
						hide_counters=true;
						SET_NAME_IF_NOT_SET
					}
					else if (request->path_match[1]=="add_categories" ) {
						getp.set_action(::zpds::query::W_UMERGE);
						getp.set_fields( ZPDS_UDFLD_CATEGORIES );
						getp.clear_payloads();
						hide_counters=true;
						SET_NAME_IF_NOT_SET
					}
					else if (request->path_match[1]=="add_profiles" ) {
						getp.set_action(::zpds::query::W_UMERGE);
						getp.set_fields( ZPDS_UDFLD_PROFILES );
						getp.clear_payloads();
						hide_counters=true;
						SET_NAME_IF_NOT_SET
					}
					else throw zpds::BadCodeException("Unknown API used");
#undef SET_NAME_IF_NOT_SET

					if (!stptr->is_ready.Get()) throw zpds::BadDataException("System Not Ready");

					// action
					if (stptr->is_master.Get()) {
						::zpds::store::ExterDataService rs;
						rs.ManageDataAction(stptr,&getp);
					}
					else {
						zpds::hrpc::HrpcClient hclient;
						hclient.SendToMaster(stptr,::zpds::hrpc::M_MANAGE_EXTER,&getp);
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

		// Endpoint : POST _admin/api/v1/exterdata/getone

		helpquery->add({scope, "POST _admin/api/v1/exterdata/getone", {
				"for exter users to read exter users and related data",
				"Parameters:",
				"username : reader name",
				"sessionkey: reader session key",
				"payload: contains payload for name(*)"
			}
		});

		// Endpoint : POST _admin/api/v1/exterdata/getmany

		helpquery->add({scope, "POST _admin/api/v1/exterdata/getone", {
				"for exter users to read exter users and related data",
				"Parameters:",
				"username : reader name",
				"sessionkey: reader session key",
				"payloads: contains array of payload for name(*)"
			}
		});

		server->resource["/_admin/api/v1/exterdata/(getone|getmany)$"]["POST"]
		=[this,stptr](typename HttpServerT::RespPtr response, typename HttpServerT::ReqPtr request) {
			ZPDS_PARALLEL_ONE([this,stptr,response,request] {
				uint64_t currtime = ZPDS_CURRTIME_MS;
				bool ok=false;

				try
				{
					bool use_json = this->JsonRequest(request);
					zpds::query::UserDataRespT getp;
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
						getp.set_fields( ZPDS_MAX_HEX_VALUE_FIFTEEN );
						getp.clear_payloads();
					}
					else if (request->path_match[1]=="getmany" ) {
						getp.set_fields( ZPDS_MAX_HEX_VALUE_FIFTEEN );
						getp.clear_payload();
					}
					else throw zpds::BadCodeException("Unknown API used");

					if (!stptr->is_ready.Get()) throw zpds::BadDataException("System Not Ready");

					// action
					::zpds::store::ExterDataService rs;
					rs.ReadDataAction(stptr,&getp);

					// aftermath

					std::string output;
					// clear getp fields
					getp.clear_username();
					getp.clear_sessionkey();
					getp.clear_status();
					getp.clear_action();
					getp.clear_fields();
					getp.clear_is_self();
					getp.clear_keytype();
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

#endif // _ZPDS_QUERY_USERDATA_SERVICE_HPP_

