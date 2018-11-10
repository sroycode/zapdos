/**
 * @project zapdos
 * @file include/query/InfoService.hpp
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
 *  InfoService.hpp :   Info Service Implementation
 *
 */
#ifndef _ZPDS_QUERY_INFO_SERVICE_HPP_
#define _ZPDS_QUERY_INFO_SERVICE_HPP_

#include "query/QueryBase.hpp"

namespace zpds {
namespace query {

template <class HttpServerT>
class InfoService : protected zpds::query::ServiceBase<HttpServerT> {
public:

	/**
	* InfoService : constructor
	*
	* @param context
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

	InfoService(
	    zpds::utils::SharedTable::pointer stptr,
	    typename std::shared_ptr<HttpServerT> server,
	    ::zpds::query::HelpQuery::pointer helpquery,
	    const unsigned int scope)
		: zpds::query::ServiceBase<HttpServerT>(ZPDS_SERVICE_SCOPE_HTTP | ZPDS_SERVICE_SCOPE_HTTPS)
	{
		if (!(this->myscope & scope)) return; // scope mismatch

		// Endpoint : GET help
		helpquery->add({scope,"GET help", { "This query, Gets info about endpoints this server" } });

		server->resource["/help$"]["GET"]
		=[this,stptr,scope,helpquery](typename HttpServerT::RespPtr response, typename HttpServerT::ReqPtr request) {
			ZPDS_PARALLEL_ONE([this,stptr,scope,helpquery,response,request] {
				try {
					DLOG(INFO) << request->path;
					std::string output;
					std::stringstream content_stream;
					for (auto& var: helpquery->get(scope))
					{
						content_stream << "\r\n\r\n" << var.route ;
						size_t counter=0;
						for (auto& desc: var.desc) content_stream << "\r\n -- " << desc;
					}
					output = content_stream.str();
					*response << "HTTP/1.1 200 OK\r\n";
					*response << "Content-type: text/plain\r\n";
					*response << "Content-Length: " << output.length() << "\r\n\r\n" << output.c_str();
					this->HttpOKAction(response,request,200,"OK","text/plain",output,true);
				} catch (...)
				{
					this->HttpErrorAction(response,request,500,"INTERNAL SERVER ERROR");
				}
			});
		};


#ifdef ZPDS_BUILD_WITH_CTEMPLATE
		// Endpoint : GET help.html
		helpquery->add({scope,"GET help.html", { "This query, Gets info about endpoints this server in html" } });

		server->resource["/help.html$"]["GET"]
		=[this,stptr,scope,helpquery](typename HttpServerT::RespPtr response, typename HttpServerT::ReqPtr request) {
			ZPDS_PARALLEL_ONE([this,stptr,scope,helpquery,response,request] {
				try {
					DLOG(INFO) << request->path;
					ctemplate::TemplateDictionary d("help");
					for (auto& var: helpquery->get(scope))
					{
						ctemplate::TemplateDictionary* dict = d.AddSectionDictionary("ONE_RESULT");
						dict->SetValue("ROUTE",var.route);
						for (auto& desc: var.desc) dict->SetValueAndShowSection("DESC",desc,"DESC_SECTION");
					}
					this->HttpTemplateAction(response,request,200,"OK","text/html","help.tpl",&d,true);
				} catch (...)
				{
					this->HttpErrorAction(response,request,500,"INTERNAL SERVER ERROR");
				}
			});
		};
#endif

		// Endpoint : GET info
		helpquery->add({scope,"GET info", { "Gets info about this server" } });

		server->resource["/info$"]["GET"]
		=[this,stptr](typename HttpServerT::RespPtr response, typename HttpServerT::ReqPtr request) {
			ZPDS_PARALLEL_ONE([this,stptr,response,request] {
				try {
					LOG(INFO) << request->path;
					::zpds::query::StateT sstate;
					sstate.set_ts( ZPDS_CURRTIME_MS );
					sstate.set_is_ready( stptr->is_ready.Get() );
					// get other params if ready
					if (sstate.is_ready())
					{
						sstate.set_hostname( stptr->hostname.Get() );
						sstate.set_is_master( stptr->is_master.Get() );
						if(!sstate.is_master())
							sstate.set_master( stptr->master.Get() );
					}

					// aftermath
					std::string output;
					pb2json(&sstate , output);
					this->HttpOKAction(response,request,200,"OK","application/json",output,true);
				} catch (...)
				{
					this->HttpErrorAction(response,request,500,"INTERNAL SERVER ERROR");
				}
			});
		};

	}

private:

};
} // namespace query
} // namespace zpds

#endif // _ZPDS_QUERY_INFO_SERVICE_HPP_
