/**
 * @project zapdos
 * @file include/query/ExtraAttribService.hpp
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
 *  ExtraAttribService.hpp : Data input service header
 *
 */
#ifndef _ZPDS_QUERY_EXTRA_ATTRIB_SERVICE_HPP_
#define _ZPDS_QUERY_EXTRA_ATTRIB_SERVICE_HPP_

#include "query/QueryBase.hpp"
#include "hrpc/HrpcClient.hpp"
#include "store/ProfileService.hpp"
#include "store/ExtraAttribService.hpp"
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

namespace zpds {
namespace query {

template <class HttpServerT>
class ExtraAttribService : public zpds::query::ServiceBase<HttpServerT> {
public:

	/**
	* ExtraAttribService : constructor
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

	ExtraAttribService(
	    zpds::utils::SharedTable::pointer stptr,
	    typename std::shared_ptr<HttpServerT> server,
	    ::zpds::query::HelpQuery::pointer helpquery,
	    unsigned int scope)
		: zpds::query::ServiceBase<HttpServerT>(ZPDS_SERVICE_SCOPE_HTTP | ZPDS_SERVICE_SCOPE_HTTPS)
	{
		if (!(this->myscope & scope)) return; // scope mismatch

		// Endpoint : POST _data/api/v1/extraattr/upsert
		helpquery->add({scope, "POST _data/api/v1/extraattr/upsert", {
				"Update and overwrite",
				"Parameters: see ExtraDataT"
			}
		});

		// Endpoint : POST _data/api/v1/extraattr/merge
		helpquery->add({scope, "POST _data/api/v1/extraattr/merge", {
				"Update and merge",
				"Parameters: see ExtraDataT"
			}
		});

		server->resource["/_data/api/v1/extraattr/(upsert|merge)$"]["POST"]
		=[this,stptr](typename HttpServerT::RespPtr response, typename HttpServerT::ReqPtr request) {
			ZPDS_PARALLEL_ONE([this,stptr,response,request] {
				uint64_t currtime = ZPDS_CURRTIME_MS;
				bool ok=false;

				try
				{
					bool use_json = this->JsonRequest(request);

					zpds::query::ExtraParamsT lparams;
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
						if (!pf.VerifyProfile(stptr,&prof) )
							throw zpds::BadDataException("No such user or Invalid passkey",M_INVALID_PARAM);
					}
					ldata->clear_passkey();

					if (stptr->is_master.Get()) {
						::zpds::store::ExtraAttribService rs;
						rs.AddDataAction(stptr,&lparams);
					}
					else {
						zpds::hrpc::HrpcClient hclient;
						hclient.SendToMaster(stptr,::zpds::hrpc::M_ADDDATA_TEXT,&lparams);
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

		// Endpoint : POST _data/api/v1/extraattr/get

		helpquery->add({scope, "POST _data/api/v1/extraattr/get", {
				"gets the data as json",
				"Parameters: see ExtraDataT, just need stype lang uniqueid"
			}
		});

		// Endpoint : POST _data/api/v1/extraattr/getnd

		helpquery->add({scope, "POST _data/api/v1/extraattr/getnd", {
				"gets the data as ndjson",
				"Parameters: see ExtraDataT, just need stype lang uniqueid"
			}
		});

		server->resource["/_data/api/v1/extraattr/get(nd|)$"]["POST"]
		=[this,stptr](typename HttpServerT::RespPtr response, typename HttpServerT::ReqPtr request) {
			ZPDS_PARALLEL_ONE([this,stptr,response,request] {
				uint64_t currtime = ZPDS_CURRTIME_MS;
				bool ok=false;

				try
				{
					bool use_json = this->JsonRequest(request);
					bool use_nd = (request->path_match[1]=="nd");

					zpds::query::ExtraParamsT lparams;
					auto ldata = lparams.mutable_ldata();

					if (use_json) {
						bool stat = json2pb( request->content.string(), ldata );
						if (!stat) throw InitialException("Bad Json Format",M_BAD_JSON);
					}
					else {
						throw InitialException("Only Json Format",M_BAD_JSON);
					}

					if (!stptr->is_ready.Get()) throw zpds::BadDataException("System Not Ready");

					// action

					::zpds::store::ExtraAttribService rs;
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

	}

private:

};
} // namespace query
} // namespace zpds

#endif // _ZPDS_QUERY_EXTRA_ATTRIB_SERVICE_HPP_

