/**
 * @project zapdos
 * @file include/query/CompletionService.hpp
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
 *  CompletionService.hpp : Competion query
 *
 */
#ifndef _ZPDS_QUERY_COMPLETION_SERVICE_HPP_
#define _ZPDS_QUERY_COMPLETION_SERVICE_HPP_

#include "query/QueryBase.hpp"
#include "store/CompletionService.hpp"
#include <xapian.h>

namespace zpds {
namespace query {

template <class HttpServerT>
class CompletionService : public zpds::query::ServiceBase<HttpServerT> {
public:

	/**
	* CompletionService : constructor
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

	CompletionService(
	    zpds::utils::SharedTable::pointer stptr,
	    typename std::shared_ptr<HttpServerT> server,
	    ::zpds::query::HelpQuery::pointer helpquery,
	    unsigned int scope)
		: zpds::query::ServiceBase<HttpServerT>(ZPDS_SERVICE_SCOPE_HTTP | ZPDS_SERVICE_SCOPE_HTTPS)
	{
		if (!(this->myscope & scope)) return; // scope mismatch

		helpquery->add({scope, "GET _query/api/v1/completion/{lang}/{profile}?{params}", {
				"for completion local by profile ",
				"Parameters:",
				"q : query text",
				"f : set 1 if fulltext aka no partial",
				"l : set 1 if local should be distance sorted",
				"ps: page size aka no of results",
				"sc: selected country code",
				"ss: selected state code",
				"st: selected city",
				"sd: selected data_city",
				"sa: selected area",
				"sz: selected pincode",
				"sx: selected longitude",
				"sy: selected latitude",
				"sp: selected location priority if 1",
				"dc: detected country code",
				"ds: detected state code",
				"dt: detected city",
				"dd: detected data_city",
				"da: detected area",
				"dz: detected pincode",
				"dx: detected longitude",
				"dy: detected latitude",
				"dp: detected location priority if 1"
			}
		});

		server->resource["/_query/api/v1/completion/(en)/(.*)$"]["GET"]
		=[this,stptr](typename HttpServerT::RespPtr response, typename HttpServerT::ReqPtr request) {
			ZPDS_PARALLEL_ONE([this,stptr,response,request] {
				uint64_t currtime = ZPDS_CURRTIME_MS;
				bool ok=false;

				try
				{
					auto params = urldecode( request->query_string );
					zpds::query::CompletionParamsT localparams;
					auto cdata = localparams.mutable_cdata();

					// start query type shreos
					if ( request->path_match[1] == "en" ) cdata->set_scope( ::zpds::search::SCOPE_LOCAL );
					else if ( request->path_match[1] == "osm" ) cdata->set_scope( ::zpds::search::SCOPE_OSMDATA );
					else if ( request->path_match[1] == "shop" ) cdata->set_scope( ::zpds::search::SCOPE_PRODUCT );
					else if ( request->path_match[1] == "social" ) cdata->set_scope( ::zpds::search::SCOPE_SOCIAL );
					else if ( request->path_match[1] == "news" ) cdata->set_scope( ::zpds::search::SCOPE_NEWS );
					else if ( request->path_match[1] == "movie" ) cdata->set_scope( ::zpds::search::SCOPE_MOVIE );
					else if ( request->path_match[1] == "video" ) cdata->set_scope( ::zpds::search::SCOPE_VIDEO );
					else if ( request->path_match[1] == "image" ) cdata->set_scope( ::zpds::search::SCOPE_IMAGE );
					else throw ::zpds::InitialException("Invalid Query Name");

					// check profile
					if ( request->path_match[2].length()== 0 )
						throw ::zpds::InitialException("Invalid Profile Name");
					cdata->set_name( request->path_match[2] );

					// start query
					if ( params.find("q") != params.end() )
						cdata->set_query( params.at("q"));
					if ( params.find("f") != params.end() )
						cdata->set_is_fulltext( std::strtoul ( params.at("f").c_str(), nullptr, 10) > 0 );
					if ( params.find("l") != params.end() )
						cdata->set_local_by_dist( std::strtoul ( params.at("l").c_str(), nullptr, 10) > 0 );
					if ( params.find("ps") != params.end() )
						cdata->set_ps( std::strtoul ( params.at("ps").c_str(), nullptr, 10) );
					else
						cdata->set_ps( 10 );

					if ( params.find("sc") != params.end() )
						cdata->mutable_sel()->set_ccode( params.at("sc"));
					if ( params.find("ss") != params.end() )
						cdata->mutable_sel()->set_scode( params.at("ss"));
					if ( params.find("st") != params.end() )
						cdata->mutable_sel()->set_city( params.at("st"));
					if ( params.find("sd") != params.end() )
						cdata->mutable_sel()->set_data_city( params.at("sd"));
					if ( params.find("sa") != params.end() )
						cdata->mutable_sel()->set_area( params.at("sa"));
					if ( params.find("sz") != params.end() )
						cdata->mutable_sel()->set_pincode( params.at("sz"));

					if ( params.find("sy") != params.end() )
						cdata->mutable_sel()->set_lat( std::strtod( params.at("sy").c_str(), NULL) );
					if ( params.find("sx") != params.end() )
						cdata->mutable_sel()->set_lon( std::strtod(params.at("sx").c_str(), NULL) );
					if ( params.find("sp") != params.end() )
						cdata->mutable_sel()->set_prior( std::strtoul ( params.at("sp").c_str(), nullptr, 10) > 0 );

					if ( params.find("dc") != params.end() )
						cdata->mutable_cur()->set_ccode( params.at("dc"));
					if ( params.find("ds") != params.end() )
						cdata->mutable_cur()->set_scode( params.at("ds"));
					if ( params.find("dt") != params.end() )
						cdata->mutable_cur()->set_city( params.at("dt"));
					if ( params.find("dd") != params.end() )
						cdata->mutable_cur()->set_data_city( params.at("dd"));
					if ( params.find("da") != params.end() )
						cdata->mutable_cur()->set_area( params.at("da"));
					if ( params.find("dz") != params.end() )
						cdata->mutable_cur()->set_pincode( params.at("dz"));

					if ( params.find("dy") != params.end() )
						cdata->mutable_cur()->set_lat( std::strtod( params.at("dy").c_str(), NULL) );
					if ( params.find("dx") != params.end() )
						cdata->mutable_cur()->set_lon( std::strtod(params.at("dx").c_str(), NULL) );
					if ( params.find("dp") != params.end() )
						cdata->mutable_cur()->set_prior( std::strtoul ( params.at("dp").c_str(), nullptr, 10) > 0 );

					if (!stptr->is_ready.Get()) throw zpds::BadDataException("System Not Ready");

					// action
					zpds::store::CompletionService rs(stptr);
					rs.GetCompleteAction(&localparams);

					// aftermath
					std::string output;
					pb2json(localparams.mutable_cresp(),output);

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

	}

private:

};
} // namespace query
} // namespace zpds

#endif // _ZPDS_QUERY_COMPLETION_SERVICE_HPP_

