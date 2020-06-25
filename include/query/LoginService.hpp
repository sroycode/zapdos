/**
 * @project zapdos
 * @file include/query/LoginService.hpp
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
 *  LoginService.hpp : Login as exter or admin Endpoint
 *
 */
#ifndef _ZPDS_QUERY_LOGIN_SERVICE_HPP_
#define _ZPDS_QUERY_LOGIN_SERVICE_HPP_

#include "query/QueryBase.hpp"
#include "hrpc/HrpcClient.hpp"

#include "store/ExterCredService.hpp"
#include "store/UserCredService.hpp"

#include "store/ExterDataService.hpp"
#include "store/UserDataService.hpp"

#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>

namespace zpds {
namespace query {

template <class HttpServerT>
class LoginService : public zpds::query::ServiceBase<HttpServerT> {
public:

	/**
	* LoginService : constructor
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

	LoginService(
	    zpds::utils::SharedTable::pointer stptr,
	    typename std::shared_ptr<HttpServerT> server,
	    ::zpds::query::HelpQuery::pointer helpquery,
	    unsigned int scope)
		: zpds::query::ServiceBase<HttpServerT>(ZPDS_SERVICE_SCOPE_HTTP | ZPDS_SERVICE_SCOPE_HTTPS)
	{
		if (!(this->myscope & scope)) return; // scope mismatch

		// Endpoint : POST _admin/api/v1/login

		helpquery->add({scope, "POST _admin/api/v1/login", {
				"this gets the admin session on giving valid password",
				"Parameters:",
				"username : user name",
				"password: password"
			}
		});

		// Endpoint : POST _user/api/v1/login

		helpquery->add({scope, "POST _user/api/v1/login", {
				"this gets the user session on giving valid password",
				"Parameters:",
				"username : user name",
				"password: password"
			}
		});

		server->resource["/(_admin|_user)/api/v1/login$"]["POST"]
		=[this,stptr](typename HttpServerT::RespPtr response, typename HttpServerT::ReqPtr request) {
			ZPDS_PARALLEL_ONE([this,stptr,response,request] {
				uint64_t currtime = ZPDS_CURRTIME_MS;
				bool ok=false;

				try
				{
					bool use_json = this->JsonRequest(request);
					zpds::query::LoginRespT getp;
					if (use_json) {
						int stat = json2pb( request->content.string(), &getp);
						if (stat<0) throw InitialException("Bad Json Format",M_BAD_JSON);
					}
					else {
						form2pb( request->content.string(), &getp );  // throws on error
					}

					if (!stptr->is_ready.Get()) throw zpds::BadDataException("System Not Ready");

					// action
					if (request->path_match[1]=="_admin" ) {
						zpds::store::ExterCredService rs;
						rs.GetSession(stptr,&getp);
					}
					else if (request->path_match[1]=="_user" ) {
						zpds::store::UserCredService rs;
						rs.GetSession(stptr,&getp);
					}
					else {
						throw zpds::InitialException("Unknown API used",M_INVALID_PARAM);
					}

					// aftermath
					std::string output;
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

		// Endpoint : POST _admin/api/v1/resetpass

		helpquery->add({scope, "POST _admin/api/v1/resetpass", {
				"this resets passkey on giving valid password or otp",
				"Parameters:",
				"username : user name",
				"newpass: new password"
				"and one of these two:"
				"otp: one time password sent separately"
				"oldpass: old password"
			}
		});

		// Endpoint : POST _user/api/v1/resetpass

		helpquery->add({scope, "POST _user/api/v1/resetpass", {
				"this gets the user session on giving valid password or otp",
				"Parameters:",
				"username : user name",
				"newpass: new password"
				"and one of these two:"
				"otp: one time password sent separately"
				"oldpass: old password"
			}
		});

		server->resource["/(_admin|_user)/api/v1/resetpass$"]["POST"]
		=[this,stptr](typename HttpServerT::RespPtr response, typename HttpServerT::ReqPtr request) {
			ZPDS_PARALLEL_ONE([this,stptr,response,request] {
				uint64_t currtime = ZPDS_CURRTIME_MS;
				bool ok=false;

				try
				{
					bool use_json = this->JsonRequest(request);
					zpds::query::ResetRespT getp;
					if (use_json) {
						int stat = json2pb( request->content.string(), &getp);
						if (stat<0) throw InitialException("Bad Json Format",M_BAD_JSON);
					}
					else {
						form2pb( request->content.string(), &getp );  // throws on error
					}

					if (!stptr->is_ready.Get()) throw zpds::BadDataException("System Not Ready");

					// action
					if (request->path_match[1]=="_admin" ) {
						if (stptr->is_master.Get()) {
							::zpds::store::ExterDataService rs;
							rs.ResetPassAction(stptr,&getp);
						}
						else {
							zpds::hrpc::HrpcClient hclient;
							hclient.SendToMaster(stptr,::zpds::hrpc::M_RESETPASS_EXTER,&getp);
						}
					}
					else if (request->path_match[1]=="_user" ) {
						if (stptr->is_master.Get()) {
							::zpds::store::UserDataService rs;
							rs.ResetPassAction(stptr,&getp);
						}
						else {
							zpds::hrpc::HrpcClient hclient;
							hclient.SendToMaster(stptr,::zpds::hrpc::M_RESETPASS_USER,&getp);
						}
					}
					else {
						throw zpds::InitialException("Unknown API used",M_INVALID_PARAM);
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


		// end
	}


private:

};
} // namespace query
} // namespace zpds

#endif // _ZPDS_QUERY_LOGIN_SERVICE_HPP_

