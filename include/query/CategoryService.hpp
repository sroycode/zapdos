/**
 * @project zapdos
 * @file include/query/CategoryService.hpp
 * @author  S Roychowdhury <sroycode AT gmail DOT com>
 * @version 1.0.0
 *
 * @section LICENSE
 *
 * Copyright (c) 2018-2019 S Roychowdhury.
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
 *  CategoryService.hpp : Data input service header
 *
 */
#ifndef _ZPDS_QUERY_CATEGORY_SERVICE_HPP_
#define _ZPDS_QUERY_CATEGORY_SERVICE_HPP_

#include "query/QueryBase.hpp"
#include "hrpc/HrpcClient.hpp"
#include "store/ProfileService.hpp"
#include "store/CategoryService.hpp"
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

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

		// Endpoint : POST _data/api/v1/category/upsert
		helpquery->add({scope, "POST _data/api/v1/category/upsert", {
				"Update and overwrite",
				"Parameters: see CatDataT"
			}
		});

		// Endpoint : POST _data/api/v1/category/merge
		helpquery->add({scope, "POST _data/api/v1/category/merge", {
				"Update and merge",
				"Parameters: see CatDataT"
			}
		});

		server->resource["/_data/api/v1/category/(upsert|merge)$"]["POST"]
		=[this,stptr](typename HttpServerT::RespPtr response, typename HttpServerT::ReqPtr request) {
			ZPDS_PARALLEL_ONE([this,stptr,response,request] {
				uint64_t currtime = ZPDS_CURRTIME_MS;
				bool ok=false;

				try
				{
					bool use_json = this->JsonRequest(request);

					zpds::query::CatParamsT lparams;
					auto ldata = lparams.mutable_ldata();

					if (use_json) {
						bool stat = json2pb( request->content.string(), ldata );
						if (!stat) throw InitialException("Bad Json Format",M_BAD_JSON);
					}
					else {
						throw InitialException("Only Json Format",M_BAD_JSON);
					}

					ldata->set_merge(  request->path_match[1]=="merge" );

					if (ldata->records_size() > ZPDS_UPLOAD_MAX_RECORDS) 
							throw zpds::BadDataException("Cannot handle that many records, max exceeded",M_INVALID_PARAM);

					if (!stptr->is_ready.Get()) throw zpds::BadDataException("System Not Ready");

					// action
					if (!stptr->allow_free_update.Get()) {
						::zpds::store::ProfileService pf;
						::zpds::store::ProfileT prof;
						prof.set_name( ldata->name() );
						prof.set_passkey( ldata->passkey() );
						if (!pf.VerifyProfile(stptr,&prof,true) )
							throw zpds::BadDataException("No such user or Invalid passkey or Used cannot update",M_INVALID_PARAM);
					}
					ldata->clear_passkey();

					if (stptr->is_master.Get()) {
						::zpds::store::CategoryService rs;
						rs.AddDataAction(stptr,&lparams);
					}
					else {
						zpds::hrpc::HrpcClient hclient;
						hclient.SendToMaster(stptr,::zpds::hrpc::M_ADDDATA_CAT,&lparams);
					}
					// aftermath

					std::string output;
					pb2json(lparams.mutable_lresp(),output);

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

		// Endpoint : POST _data/api/v1/category/get

		helpquery->add({scope, "POST _data/api/v1/category/get", {
				"gets the data as json",
				"Parameters: see CatDataT, just need stype lang uniqueid"
			}
		});

		// Endpoint : POST _data/api/v1/category/getnd

		helpquery->add({scope, "POST _data/api/v1/category/getnd", {
				"gets the data as ndjson",
				"Parameters: see CatDataT, just need stype lang uniqueid"
			}
		});

		server->resource["/_data/api/v1/category/get(nd|)$"]["POST"]
		=[this,stptr](typename HttpServerT::RespPtr response, typename HttpServerT::ReqPtr request) {
			ZPDS_PARALLEL_ONE([this,stptr,response,request] {
				uint64_t currtime = ZPDS_CURRTIME_MS;
				bool ok=false;

				try
				{
					bool use_json = this->JsonRequest(request);
					bool use_nd = (request->path_match[1]=="nd");

					zpds::query::CatParamsT lparams;
					auto ldata = lparams.mutable_ldata();

					if (use_json) {
						bool stat = json2pb( request->content.string(), ldata );
						if (!stat) throw InitialException("Bad Json Format",M_BAD_JSON);
					}
					else {
						throw InitialException("Only Json Format",M_BAD_JSON);
					}

					// extra
					lparams.set_extra( ldata->extra() > 0 );

					if (!stptr->is_ready.Get()) throw zpds::BadDataException("System Not Ready");

					// action

					::zpds::store::CategoryService rs;
					rs.GetDataAction(stptr,&lparams);
					// aftermath
					std::string output;
					if (use_nd) {
						std::stringstream ss;
						for (auto i=0; i<ldata->records_size(); ++i) {
							if (!ldata->records(i).notfound()) // remove notfound
								ss << pb2json(ldata->mutable_records(i)) << "\n";
						}
						output = ss.str();
					}
					else {
						pb2json(ldata,output);
					}

					ok=true;
					this->HttpOKAction(response,request,200,"OK", (use_nd) ? "application/x-ndjson" : "application/json", output,true);
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

		// Endpoint : GET _data/api/v1/category/esbulk

		helpquery->add({scope, "GET _data/api/v1/category/esbulk?{params}", {
				"this gets the elastic data",
				"Parameters:",
				"index: _index to populate",
				"type: _type to populate",
				"styp: record type to get",
				"lang: language type to get",
				"last_up: updated after",
				"last_id: id of last updated optional",
				"limit: limit optional default 1000 ,max 10k"
			}
		});

		server->resource["/_data/api/v1/category/esbulk$"]["GET"]
		=[this,stptr](typename HttpServerT::RespPtr response, typename HttpServerT::ReqPtr request) {
			ZPDS_PARALLEL_ONE([this,stptr,response,request] {
				uint64_t currtime = ZPDS_CURRTIME_MS;
				bool ok=false;

				try
				{
					auto params = urldecode( request->query_string );
					zpds::query::CatParamsT lparams;

					// index for es
					if ( params.find("index") == params.end() )
						throw zpds::InitialException("index variable needed",M_INVALID_PARAM);
					std::string e_index{params["index"]};

					// type for es
					if ( params.find("type") == params.end() )
						throw zpds::InitialException("type variable needed",M_INVALID_PARAM);
					std::string e_type{params["type"]};

					// styp default S_NONE
					if ( params.find("styp") == params.end() ) {
						const google::protobuf::EnumDescriptor *descriptor = zpds::search::SourceTypeE_descriptor();
						if ( descriptor->FindValueByName( params["styp"] ) )
							lparams.set_styp( zpds::search::SourceTypeE ( descriptor->FindValueByName( params["styp"])->number() ) );
					}

					// lang default EN
					if ( params.find("lang") == params.end() ) {

						std::string lang = params["lang"];
						boost::algorithm::to_upper(lang);
						const google::protobuf::EnumDescriptor *descriptor = zpds::search::LangTypeE_descriptor();
						if ( descriptor->FindValueByName( lang ) )
							lparams.set_lang( zpds::search::LangTypeE ( descriptor->FindValueByName(lang)->number() ) );
					}

					// updated_at
					if ( params.find("last_up") != params.end() ) {
						if (params["last_up"].length()>0)
							lparams.set_last_up( boost::lexical_cast<uint64_t>(params["last_up"]) );
					}

					// last_id
					if ( params.find("last_id") != params.end() ) {
						if (params["last_id"].length()>0)
							lparams.set_last_id( boost::lexical_cast<uint64_t>(params["last_id"]) );
					}

					// limit
					lparams.set_limit( 1000 ); // default limit
					if ( params.find("limit") != params.end() ) {
						if (params["limit"].length()>0)
							lparams.set_limit( boost::lexical_cast<uint64_t>(params["limit"]) );
					}

					// extra
					if ( params.find("extra") != params.end() ) {
						if (params["extra"].length()>0)
							lparams.set_extra( boost::lexical_cast<uint64_t>(params["last_id"]) > 0 );
					}

					if (!stptr->is_ready.Get()) throw zpds::BadDataException("System Not Ready");

					::zpds::store::CategoryService rs;
					rs.GetIndexDataAction(stptr,&lparams);
					// aftermath
					auto ldata = lparams.mutable_ldata();
					std::stringstream ss;
					for (auto i=0; i<ldata->records_size(); ++i) {
						if (ldata->records(i).notfound()) continue;
						if (ldata->records(i).is_deleted()) {
							ss << "{ \"delete\" : { \"_index\" : \"" << e_index
							   << "\", \"_type\" : \"" << e_type
							   << "\", \"_id\" : \"" << ldata->records(i).id() << "\" } }\n";
						}
						else {
							ss << "{ \"index\" : { \"_index\" : \"" << e_index
							   << "\", \"_type\" : \"" << e_type
							   << "\", \"_id\" : \"" << ldata->records(i).id() << "\" } }\n"
							   << pb2json(ldata->mutable_records(i)) << "\n";
						}
					}
					std::string output = ss.str();
					ok=true;
					this->HttpOKAction(response,request,200,"OK","application/x-ndjson",output,true);
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

	}

private:

};
} // namespace query
} // namespace zpds

#endif // _ZPDS_QUERY_CATEGORY_SERVICE_HPP_

