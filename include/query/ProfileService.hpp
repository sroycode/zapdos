/**
 * @project zapdos
 * @file include/query/ProfileService.hpp
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
 *  ProfileService.hpp : Profile related services
 *
 */
#ifndef _ZPDS_QUERY_PROFILE_SERVICE_HPP_
#define _ZPDS_QUERY_PROFILE_SERVICE_HPP_

#include "query/QueryBase.hpp"
#include "hrpc/HrpcClient.hpp"
#include "store/ProfileService.hpp"

#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>

namespace zpds {
namespace query {

template <class HttpServerT>
class ProfileService : public zpds::query::ServiceBase<HttpServerT> {
public:

	/**
	* ProfileService : constructor
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

	ProfileService(
	    zpds::utils::SharedTable::pointer stptr,
	    typename std::shared_ptr<HttpServerT> server,
	    ::zpds::query::HelpQuery::pointer helpquery,
	    unsigned int scope)
		: zpds::query::ServiceBase<HttpServerT>(ZPDS_SERVICE_SCOPE_HTTP | ZPDS_SERVICE_SCOPE_HTTPS)
	{
		if (!(this->myscope & scope)) return; // scope mismatch

		// Endpoint : GET _admin/api/v1/get_profile

		helpquery->add({scope, "GET _admin/api/v1/get_profile?{params}", {
				"this gets the profile info ",
				"Parameters:",
				"name : name of profile"
			}
		});

		server->resource["/_admin/api/v1/get_profile$"]["GET"]
		=[this,stptr](typename HttpServerT::RespPtr response, typename HttpServerT::ReqPtr request) {
			ZPDS_PARALLEL_ONE([this,stptr,response,request] {
				uint64_t currtime = ZPDS_CURRTIME_MS;
				bool ok=false;

				try
				{
					auto params = urldecode( request->query_string );
					zpds::query::ProfileRespT getp;

					if ( params.find("name") == params.end() )
						throw zpds::InitialException("name variable needed",M_INVALID_PARAM);
					getp.set_name( params["name"]);

					if (!stptr->is_ready.Get()) throw zpds::BadDataException("System Not Ready");

					// action
					zpds::store::ProfileService rs;
					rs.GetProfileAction(stptr,&getp);
					// aftermath
					std::string output;
					getp.clear_oldpass();
					getp.clear_newpass();
					pb2json(&getp,output);

					ok=true;
					this->HttpOKAction(response,request,200,"OK","application/json",output,true);
				}
				catch (zpds::BaseException& e)
				{
					std::string output = err2json(e.ecode(),e.what());
					this->HttpOKAction(response,request,200,"OK","application/json",output,true);
				}
				catch (std::exception& e)
				{
					std::string output = err2json(M_UNKNOWN,e.what());
					this->HttpOKAction(response,request,200,"OK","application/json",output,true);
				}
				catch (...)
				{
					this->HttpErrorAction(response,request,500,"INTERNAL SERVER ERROR");
				}
				LOG(INFO) << request->path << " ms: " << ZPDS_CURRTIME_MS - currtime;
			});
		};

		// Endpoint : GET _admin/api/v1/set_profile

		helpquery->add({scope, "GET _admin/api/v1/set_profile?{params}", {
				"this modifies a profile ",
				"Parameters:",
				"name : name of profile",
				"oldpass: old password",
				"newpass: new password"
			}
		});

		server->resource["/_admin/api/v1/set_profile$"]["GET"]
		=[this,stptr](typename HttpServerT::RespPtr response, typename HttpServerT::ReqPtr request) {
			ZPDS_PARALLEL_ONE([this,stptr,response,request] {
				uint64_t currtime = ZPDS_CURRTIME_MS;
				bool ok=false;

				try
				{
					auto params = urldecode( request->query_string );
					zpds::query::ProfileRespT getp;

					if ( params.find("name") == params.end() )
						throw zpds::InitialException("name variable needed",M_INVALID_PARAM);
					getp.set_name( params["name"]);

					if ( params.find("oldpass") == params.end() )
						throw zpds::InitialException("oldpass variable needed",M_INVALID_PARAM);
					getp.set_oldpass( params["oldpass"]);

					if ( params.find("newpass") == params.end() )
						throw zpds::InitialException("newpass variable needed",M_INVALID_PARAM);
					getp.set_newpass( params["newpass"]);

					getp.set_updated_at( currtime );

					if (!stptr->is_ready.Get()) throw zpds::BadDataException("System Not Ready");

					// action

					if (stptr->is_master.Get()) {
						::zpds::store::ProfileService rs;
						rs.SetProfileAction(stptr,&getp);
					}
					else {
						zpds::hrpc::HrpcClient hclient;
						hclient.SendToMaster(stptr,::zpds::hrpc::M_SETPROFILE,&getp);
					}
					// aftermath
					std::string output;
					getp.clear_oldpass();
					getp.clear_newpass();
					pb2json(&getp,output);

					ok=true;
					this->HttpOKAction(response,request,200,"OK","application/json",output,true);
				}
				catch (zpds::BaseException& e)
				{
					std::string output = err2json(e.ecode(),e.what());
					this->HttpOKAction(response,request,200,"OK","application/json",output,true);
				}
				catch (std::exception& e)
				{
					std::string output = err2json(M_UNKNOWN,e.what());
					this->HttpOKAction(response,request,200,"OK","application/json",output,true);
				}
				catch (...)
				{
					this->HttpErrorAction(response,request,500,"INTERNAL SERVER ERROR");
				}
				LOG(INFO) << request->path << " ms: " << ZPDS_CURRTIME_MS - currtime;
			});
		};

		// Endpoint : GET _admin/api/v1/new_profile

		helpquery->add({scope, "GET _admin/api/v1/new_profile?{params}", {
				"this creates a new profile, needs shared-secret and resets if exists",
				"Parameters:",
				"name : name of profile",
				"newpass: new password",
				"update: can update "
			}
		});

		server->resource["/_admin/api/v1/new_profile$"]["GET"]
		=[this,stptr](typename HttpServerT::RespPtr response, typename HttpServerT::ReqPtr request) {
			ZPDS_PARALLEL_ONE([this,stptr,response,request] {
				uint64_t currtime = ZPDS_CURRTIME_MS;
				bool ok=false;

				try
				{
					auto params = urldecode( request->query_string );
					zpds::query::ProfileRespT getp;

					// check shared secret
					auto it=request->header.find("Shared-Secret");
					if (it==request->header.end())
						throw zpds::BadDataException("No Header Shared Secret");
					if (it->second != stptr->shared_secret.Get())
						throw zpds::BadDataException("Bad Shared Secret");

					if ( params.find("name") == params.end() )
						throw zpds::InitialException("name variable needed",M_INVALID_PARAM);
					getp.set_name( params["name"]);

					if ( params.find("newpass") == params.end() )
						throw zpds::InitialException("newpass variable needed",M_INVALID_PARAM);
					getp.set_newpass( params["newpass"]);

					if ( params.find("update") != params.end() )
						getp.set_update( std::strtoul ( params["update"].c_str(), nullptr, 0) );

					getp.set_updated_at( currtime );

					if (!stptr->is_ready.Get()) throw zpds::BadDataException("System Not Ready");

					// action

					if (stptr->is_master.Get()) {
						::zpds::store::ProfileService rs;
						rs.NewProfileAction(stptr,&getp);
					}
					else {
						zpds::hrpc::HrpcClient hclient;
						hclient.SendToMaster(stptr,::zpds::hrpc::M_NEWPROFILE,&getp);
					}
					// aftermath
					std::string output;
					getp.clear_oldpass();
					getp.clear_newpass();
					pb2json(&getp,output);

					ok=true;
					this->HttpOKAction(response,request,200,"OK","application/json",output,true);
				}
				catch (zpds::BaseException& e)
				{
					std::string output = err2json(e.ecode(),e.what());
					this->HttpOKAction(response,request,200,"OK","application/json",output,true);
				}
				catch (std::exception& e)
				{
					std::string output = err2json(M_UNKNOWN,e.what());
					this->HttpOKAction(response,request,200,"OK","application/json",output,true);
				}
				catch (...)
				{
					this->HttpErrorAction(response,request,500,"INTERNAL SERVER ERROR");
				}
				LOG(INFO) << request->path << " ms: " << ZPDS_CURRTIME_MS - currtime;
			});
		};

		// Endpoint : POST _admin/api/v1/set_profile

		helpquery->add({scope, "POST _admin/api/v1/set_profile", {
				"this modifies a profile ",
				"Parameters:",
				"name : name of profile",
				"oldpass: old password",
				"newpass: new password",
				"update : update flag"
			}
		});

		server->resource["/_admin/api/v1/set_profile"]["POST"]
		=[this,stptr](typename HttpServerT::RespPtr response, typename HttpServerT::ReqPtr request) {
			ZPDS_PARALLEL_ONE([this,stptr,response,request] {
				uint64_t currtime = ZPDS_CURRTIME_MS;
				bool ok=false;

				try
				{
					bool use_json = this->JsonRequest(request);
					zpds::query::ProfileRespT getp;
					if (use_json) {
						int stat = json2pb( request->content.string(), &getp);
						if (stat<0) throw InitialException("Bad Json Format",M_BAD_JSON);
					}
					else {
						form2pb( request->content.string(), &getp );  // throws on error
					}

					getp.set_not_found( false ); // check
					getp.set_updated_at( currtime );

					if (!stptr->is_ready.Get()) throw zpds::BadDataException("System Not Ready");

					// action

					if (stptr->is_master.Get()) {
						::zpds::store::ProfileService rs;
						rs.SetProfileAction(stptr,&getp);
					}
					else {
						zpds::hrpc::HrpcClient hclient;
						hclient.SendToMaster(stptr,::zpds::hrpc::M_SETPROFILE,&getp);
					}
					// aftermath
					std::string output;
					getp.clear_oldpass();
					getp.clear_newpass();
					pb2json(&getp,output);

					ok=true;
					this->HttpOKAction(response,request,200,"OK","application/json",output,true);
				}
				catch (zpds::BaseException& e)
				{
					std::string output = err2json(e.ecode(),e.what());
					this->HttpOKAction(response,request,200,"OK","application/json",output,true);
				}
				catch (std::exception& e)
				{
					std::string output = err2json(M_UNKNOWN,e.what());
					this->HttpOKAction(response,request,200,"OK","application/json",output,true);
				}
				catch (...)
				{
					this->HttpErrorAction(response,request,500,"INTERNAL SERVER ERROR");
				}
				LOG(INFO) << request->path << " ms: " << ZPDS_CURRTIME_MS - currtime;
			});
		};

		// Endpoint : POST _admin/api/v1/new_profile

		helpquery->add({scope, "POST _admin/api/v1/new_profile", {
				"this creates a new profile, needs shared-secret and resets if exists",
				"Parameters:",
				"name : name of profile",
				"newpass: new password",
				"update : update flag"
			}
		});

		server->resource["/_admin/api/v1/new_profile$"]["POST"]
		=[this,stptr](typename HttpServerT::RespPtr response, typename HttpServerT::ReqPtr request) {
			ZPDS_PARALLEL_ONE([this,stptr,response,request] {
				uint64_t currtime = ZPDS_CURRTIME_MS;
				bool ok=false;

				try
				{
					// check shared secret
					auto it=request->header.find("Shared-Secret");
					if (it==request->header.end())
						throw zpds::BadDataException("No Header Shared Secret");
					if (it->second != stptr->shared_secret.Get())
						throw zpds::BadDataException("Bad Shared Secret");

					bool use_json = this->JsonRequest(request);
					zpds::query::ProfileRespT getp;
					if (use_json) {
						int stat = json2pb( request->content.string(), &getp);
						if (stat<0) throw InitialException("Bad Json Format",M_BAD_JSON);
					}
					else {
						form2pb( request->content.string(), &getp );  // throws on error
					}

					getp.set_not_found( false ); // check
					getp.set_updated_at( currtime );

					if (!stptr->is_ready.Get()) throw zpds::BadDataException("System Not Ready");

					// action

					if (stptr->is_master.Get()) {
						::zpds::store::ProfileService rs;
						rs.NewProfileAction(stptr,&getp);
					}
					else {
						zpds::hrpc::HrpcClient hclient;
						hclient.SendToMaster(stptr,::zpds::hrpc::M_NEWPROFILE,&getp);
					}
					// aftermath
					std::string output;
					getp.clear_oldpass();
					getp.clear_newpass();
					pb2json(&getp,output);

					ok=true;
					this->HttpOKAction(response,request,200,"OK","application/json",output,true);
				}
				catch (zpds::BaseException& e)
				{
					std::string output = err2json(e.ecode(),e.what());
					this->HttpOKAction(response,request,200,"OK","application/json",output,true);
				}
				catch (std::exception& e)
				{
					std::string output = err2json(M_UNKNOWN,e.what());
					this->HttpOKAction(response,request,200,"OK","application/json",output,true);
				}
				catch (...)
				{
					this->HttpErrorAction(response,request,500,"INTERNAL SERVER ERROR");
				}
				LOG(INFO) << request->path << " ms: " << ZPDS_CURRTIME_MS - currtime;
			});
		};

		// Endpoint : GET _admin/api/v1/get_simpletemplate

		helpquery->add({scope, "GET _admin/api/v1/get_simpletemplate?{params}", {
				"this gets the profile info ",
				"Parameters:",
				"name : name of profile",
				"qtyp : query type"
			}
		});

		server->resource["/_admin/api/v1/get_simpletemplate$"]["GET"]
		=[this,stptr](typename HttpServerT::RespPtr response, typename HttpServerT::ReqPtr request) {
			ZPDS_PARALLEL_ONE([this,stptr,response,request] {
				uint64_t currtime = ZPDS_CURRTIME_MS;
				bool ok=false;

				try
				{
					auto params = urldecode( request->query_string );
					zpds::query::SimpleProRespT getp;

					if ( params.find("name") == params.end() )
						throw zpds::InitialException("name variable needed",M_INVALID_PARAM);
					getp.set_name( params["name"]);

					if ( params.find("qtyp") == params.end() )
						throw zpds::InitialException("qtyp variable needed",M_INVALID_PARAM);

					size_t qtyp = std::strtoul ( params["qtyp"].c_str(), nullptr, 10);
					if (qtyp==0) {
						const google::protobuf::EnumDescriptor *descriptor = ::zpds::search::QueryTypeE_descriptor();
						if ( descriptor->FindValueByName( params["qtyp"] ) )
							qtyp = descriptor->FindValueByName( params["qtyp"] )->number();
					}
					if (qtyp==0)
						throw zpds::InitialException("qtyp non zero variable needed",M_INVALID_PARAM);
					getp.set_qtyp( ::zpds::search::QueryTypeE (qtyp) );

					if (!stptr->is_ready.Get()) throw zpds::BadDataException("System Not Ready");

					// action
					zpds::store::ProfileService rs;
					rs.GetSimpleTemplateAction(stptr,&getp);
					// aftermath
					std::string output;
					getp.clear_passkey();
					pb2json(&getp,output);

					ok=true;
					this->HttpOKAction(response,request,200,"OK","application/json",output,true);
				}
				catch (zpds::BaseException& e)
				{
					std::string output = err2json(e.ecode(),e.what());
					this->HttpOKAction(response,request,200,"OK","application/json",output,true);
				}
				catch (std::exception& e)
				{
					std::string output = err2json(M_UNKNOWN,e.what());
					this->HttpOKAction(response,request,200,"OK","application/json",output,true);
				}
				catch (...)
				{
					this->HttpErrorAction(response,request,500,"INTERNAL SERVER ERROR");
				}
				LOG(INFO) << request->path << " ms: " << ZPDS_CURRTIME_MS - currtime;
			});
		};

		// Endpoint : GET _admin/api/v1/get_simpletemplates

		helpquery->add({scope, "GET _admin/api/v1/get_simpletemplates?{params}", {
				"this gets all the profile templates for one user ",
				"Parameters:",
				"name : name of profile"
			}
		});

		// Endpoint : GET _admin/api/v1/check_simpletemplates

		helpquery->add({scope, "GET _admin/api/v1/check_simpletemplates?{params}", {
				"this gets all the profile templates for one user after validation",
				"Parameters:",
				"name : name of profile",
				"passkey : passkey"
			}
		});

		server->resource["/_admin/api/v1/(get|check)_simpletemplates$"]["GET"]
		=[this,stptr](typename HttpServerT::RespPtr response, typename HttpServerT::ReqPtr request) {
			ZPDS_PARALLEL_ONE([this,stptr,response,request] {
				uint64_t currtime = ZPDS_CURRTIME_MS;
				bool ok=false;

				try
				{
					bool is_chk = (request->path_match[1] == "check");
					auto params = urldecode( request->query_string );
					zpds::query::MultiSimpleProRespT getp;

					if ( params.find("name") == params.end() )
						throw zpds::InitialException("name variable needed",M_INVALID_PARAM);
					getp.set_name( params["name"]);

					if ( is_chk ) {
						if ( params.find("passkey") == params.end() )
							throw zpds::InitialException("passkey variable needed",M_INVALID_PARAM);
						getp.set_passkey( params["passkey"]);
					}

					if (!stptr->is_ready.Get()) throw zpds::BadDataException("System Not Ready");

					if ( is_chk ) {
						::zpds::store::ProfileService pf;
						::zpds::store::ProfileT prof;
						prof.set_name( getp.name() );
						prof.set_passkey( getp.passkey() );
						if (!pf.VerifyProfile(stptr,&prof) )
							throw zpds::BadDataException("No such user or Invalid passkey",M_INVALID_PARAM);
					}

					// action
					zpds::store::ProfileService rs;
					rs.GetAllSimpleTemplateAction(stptr,&getp);
					// aftermath
					std::string output;
					getp.clear_passkey();
					pb2json(&getp,output, is_chk);

					ok=true;
					this->HttpOKAction(response,request,200,"OK","application/json",output,true);
				}
				catch (zpds::BaseException& e)
				{
					std::string output = err2json(e.ecode(),e.what());
					this->HttpOKAction(response,request,200,"OK","application/json",output,true);
				}
				catch (std::exception& e)
				{
					std::string output = err2json(M_UNKNOWN,e.what());
					this->HttpOKAction(response,request,200,"OK","application/json",output,true);
				}
				catch (...)
				{
					this->HttpErrorAction(response,request,500,"INTERNAL SERVER ERROR");
				}
				LOG(INFO) << request->path << " ms: " << ZPDS_CURRTIME_MS - currtime;
			});
		};

		// Endpoint : POST _admin/api/v1/set_simpletemplate

		helpquery->add({scope, "POST _admin/api/v1/set_simpletemplate", {
				"this modifies a profile template",
				"Parameters:",
				"name : name of profile",
				"passkey: password",
				"qtyp : query type template id",
				"qprof : query template",
				"ignore : flag"
			}
		});

		server->resource["/_admin/api/v1/set_simpletemplate"]["POST"]
		=[this,stptr](typename HttpServerT::RespPtr response, typename HttpServerT::ReqPtr request) {
			ZPDS_PARALLEL_ONE([this,stptr,response,request] {
				uint64_t currtime = ZPDS_CURRTIME_MS;
				bool ok=false;

				try
				{
					bool use_json = this->JsonRequest(request);
					zpds::query::SimpleProRespT getp;
					if (use_json) {
						int stat = json2pb( request->content.string(), &getp);
						if (stat<0) throw InitialException("Bad Json Format",M_BAD_JSON);
					}
					else {
						throw InitialException("Only Json Accepted",M_BAD_JSON);
					}

					getp.set_not_found( false ); // check
					getp.set_updated_at( currtime );


					if (!stptr->is_ready.Get()) throw zpds::BadDataException("System Not Ready");

					// action

					if (stptr->is_master.Get()) {
						::zpds::store::ProfileService rs;
						rs.SetSimpleTemplateAction(stptr,&getp);
					}
					else {
						zpds::hrpc::HrpcClient hclient;
						hclient.SendToMaster(stptr,::zpds::hrpc::M_SETSIMPLETEMPLATE,&getp);
					}
					// aftermath
					std::string output;
					getp.clear_passkey();
					pb2json(&getp,output);

					ok=true;
					this->HttpOKAction(response,request,200,"OK","application/json",output,true);
				}
				catch (zpds::BaseException& e)
				{
					std::string output = err2json(e.ecode(),e.what());
					this->HttpOKAction(response,request,200,"OK","application/json",output,true);
				}
				catch (std::exception& e)
				{
					std::string output = err2json(M_UNKNOWN,e.what());
					this->HttpOKAction(response,request,200,"OK","application/json",output,true);
				}
				catch (...)
				{
					this->HttpErrorAction(response,request,500,"INTERNAL SERVER ERROR");
				}
				LOG(INFO) << request->path << " ms: " << ZPDS_CURRTIME_MS - currtime;
			});
		};

		// Endpoint : GET _admin/api/v1/get_templateformat

		helpquery->add({scope, "GET _admin/api/v1/get_templateformat", {
				"this gets the profile template info "
			}
		});

		server->resource["/_admin/api/v1/get_templateformat$"]["GET"]
		=[this,stptr](typename HttpServerT::RespPtr response, typename HttpServerT::ReqPtr request) {
			ZPDS_PARALLEL_ONE([this,stptr,response,request] {
				uint64_t currtime = ZPDS_CURRTIME_MS;
				bool ok=false;

				try
				{

					if (!stptr->is_ready.Get()) throw zpds::BadDataException("System Not Ready");

					// action
					zpds::search::QueryOrderT po;
					zpds::query::SimpleFieldListT getp;

					const google::protobuf::Descriptor *d = po.GetDescriptor();
					const google::protobuf::Reflection *ref = po.GetReflection();

					if (!d || !ref) {
						throw ::zpds::BadQueryException("invalid pb object");
					}

					for (auto i=0; i< d->field_count() ; ++i) {
						const google::protobuf::FieldDescriptor *field = d->field(i);
						auto f = getp.add_fields();
						f->set_name( field->name() );
						f->set_type( field->type_name() );
						f->set_is_repeated( field->is_repeated() );
						if ( field->cpp_type() == ::google::protobuf::FieldDescriptor::CPPTYPE_ENUM ) {
							const google::protobuf::EnumDescriptor *e = field->enum_type();
							for (auto j=0; j< e->value_count(); ++j) {
								f->add_values( e->value(j)->name() );
							}
						}
					}

					// aftermath
					std::string output;
					pb2json(&getp,output);

					ok=true;
					this->HttpOKAction(response,request,200,"OK","application/json",output,true);
				}
				catch (zpds::BaseException& e)
				{
					std::string output = err2json(e.ecode(),e.what());
					this->HttpOKAction(response,request,200,"OK","application/json",output,true);
				}
				catch (std::exception& e)
				{
					std::string output = err2json(M_UNKNOWN,e.what());
					this->HttpOKAction(response,request,200,"OK","application/json",output,true);
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

#endif // _ZPDS_QUERY_PROFILE_SERVICE_HPP_

