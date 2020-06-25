/**
 * @project zapdos
 * @file include/query/CommandService.hpp
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
 *  CommandService.hpp : Command related services Endpoint
 *
 */
#ifndef _ZPDS_QUERY_COMMAND_SERVICE_HPP_
#define _ZPDS_QUERY_COMMAND_SERVICE_HPP_

#include "query/QueryBase.hpp"
#include "store/ExterCredService.hpp"

namespace zpds {
namespace query {

template <class HttpServerT>
class CommandService : public zpds::query::ServiceBase<HttpServerT> {
public:

	/**
	* CommandService : constructor
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

	CommandService(
	    zpds::utils::SharedTable::pointer stptr,
	    typename std::shared_ptr<HttpServerT> server,
	    ::zpds::query::HelpQuery::pointer helpquery,
	    unsigned int scope)
		: zpds::query::ServiceBase<HttpServerT>(ZPDS_SERVICE_SCOPE_HTTP | ZPDS_SERVICE_SCOPE_HTTPS)
	{
		if (!(this->myscope & scope)) return; // scope mismatch

		// Endpoint : GET _admin/api/v1/local_command/{command}

		helpquery->add({scope, "GET _admin/api/v1/local_command/commit", {
				"this forces indexing at next update"
			}
		});

		helpquery->add({scope, "GET _admin/api/v1/local_command/commitnow", {
				"this forces indexing imediately"
			}
		});

		helpquery->add({scope, "GET _admin/api/v1/local_command/noxapian", {
				"this starts ignoring search data in update"
			}
		});


		server->resource["/_admin/api/v1/local_command/(.*)$"]["GET"]
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

					if (!stptr->is_ready.Get()) throw zpds::BadDataException("System Not Ready");

					::zpds::store::ErrorT errt;
					// action
					if ( request->path_match[1]=="commit" ) {
#ifdef ZPDS_BUILD_WITH_XAPIAN
						stptr->force_commit.Set(true);
						errt.set_status("OK");
#else
						errt.set_status("NA");
#endif
					}

					else if ( request->path_match[1]=="commitnow" ) {
#ifdef ZPDS_BUILD_WITH_XAPIAN
						stptr->xapdb->CommitData();
						errt.set_status("OK");
#else
						errt.set_status("NA");
#endif
					}

					else if ( request->path_match[1]=="noxapian" ) {
#ifdef ZPDS_BUILD_WITH_XAPIAN
						stptr->no_xapian.Set(true);
						errt.set_status("OK");
#else
						errt.set_status("NA");
#endif
					}

					// aftermath
					std::string output;
					pb2json(&errt,output);

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

#endif // _ZPDS_QUERY_COMMAND_SERVICE_HPP_

