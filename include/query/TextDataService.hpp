/**
 * @project zapdos
 * @file include/query/TextDataService.hpp
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
 *  TextDataService.hpp : Competion query endpoints for text data
 *
 */
#ifndef _ZPDS_QUERY_TEXTDATA_SERVICE_HPP_
#define _ZPDS_QUERY_TEXTDATA_SERVICE_HPP_

#include "query/QueryBase.hpp"
#include "store/TextDataService.hpp"
#include <xapian.h>

namespace zpds {
namespace query {

template <class HttpServerT>
class TextDataService : public zpds::query::ServiceBase<HttpServerT> {
public:

	/**
	* TextDataService : constructor
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

	TextDataService(
	    zpds::utils::SharedTable::pointer stptr,
	    typename std::shared_ptr<HttpServerT> server,
	    ::zpds::query::HelpQuery::pointer helpquery,
	    unsigned int scope)
		: zpds::query::ServiceBase<HttpServerT>(ZPDS_SERVICE_SCOPE_HTTP | ZPDS_SERVICE_SCOPE_HTTPS)
	{
		if (!(this->myscope & scope)) return; // scope mismatch

		helpquery->add({scope, "GET _query/api/v1/textdata/{profile}?{params}", {
				"for completion local by profile ",
				"Parameters:",
				"q : query text",
				"lang : language default en",
				"f : set 1 if fulltext aka no partial",
				"limit : page size aka no of results",
				"lon: detected longitude",
				"lat: detected latitude",
				"ccode : country code",
				"city : city "
			}
		});

		server->resource["/_query/api/v1/textdata/(.*)$"]["GET"]
		=[this,stptr](typename HttpServerT::RespPtr response, typename HttpServerT::ReqPtr request) {
			ZPDS_PARALLEL_ONE([this,stptr,response,request] {
				uint64_t currtime = ZPDS_CURRTIME_MS;
				bool ok=false;

				try
				{
					auto params = urldecode( request->query_string );
					zpds::query::TextDataParamsT localparams;
					auto cdata = localparams.mutable_cdata();

					// check profile
					if ( request->path_match[1].length()== 0 )
						throw ::zpds::InitialException("Invalid Profile Name");
					cdata->set_name( request->path_match[1] );

					// lang default EN
					cdata->set_lang(::zpds::search::LangTypeE::EN);
					if ( params.find("lang") != params.end() )
					{
						std::string lang = params.at("lang");
						boost::algorithm::to_upper(lang);
						const google::protobuf::EnumDescriptor *descriptor = zpds::search::LangTypeE_descriptor();
						if ( descriptor->FindValueByName( lang ) )
							cdata->set_lang( zpds::search::LangTypeE ( descriptor->FindValueByName(lang)->number() ) );
						else
							throw ::zpds::InitialException("This language is not supported");
					}

					// query
					if ( params.find("q") != params.end() )
						cdata->set_raw_query( params.at("q"));
					// fulltext
					if ( params.find("f") != params.end() )
						cdata->set_full_words( std::strtoul ( params.at("f").c_str(), nullptr, 10) > 0 );
					// limit
					cdata->set_items( 10 );
					if ( params.find("limit") != params.end() )
						cdata->set_items( std::strtoul ( params.at("limit").c_str(), nullptr, 10) );

					// lon
					if ( params.find("lon") != params.end() )
						cdata->mutable_cur()->set_lon( std::strtod( params.at("lon").c_str(), NULL) );
					// lat
					if ( params.find("lat") != params.end() )
						cdata->mutable_cur()->set_lat( std::strtod( params.at("lat").c_str(), NULL) );

					// dont_use lat lon
					if ( params.find("lon") == params.end() || params.find("lon") == params.end() )
						cdata->mutable_cur()->set_dont_use( true );

					// ccode
					if ( params.find("ccode") != params.end() )
						cdata->mutable_cur()->set_ccode( params.at("ccode"));

					// city
					if ( params.find("city") != params.end() )
						cdata->mutable_cur()->set_city( params.at("city"));

					if (!stptr->is_ready.Get()) throw zpds::BadDataException("System Not Ready");

					// action
					zpds::store::TextDataService rs(stptr);
					rs.GetCompleteAction(&localparams);

					// aftermath
					std::string output;
					pb2json(localparams.mutable_cresp(),output);
					// pb2json(&localparams,output);

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

#endif // _ZPDS_QUERY_TEXTDATA_SERVICE_HPP_
