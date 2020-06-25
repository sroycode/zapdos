/**
 * @project zapdos
 * @file include/search/SearchBase.hpp
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
 *  SearchBase.hpp : Xapian Search Abstract Base Class Headers
 *
 */
#ifndef _ZPDS_SEARCH_SEARCH_BASE_HPP_
#define _ZPDS_SEARCH_SEARCH_BASE_HPP_

#include "search/BaseUtils.hpp"
#include "search/ReadIndex.hpp"
#include "../proto/Search.pb.h"
#include "../proto/Query.pb.h"
#include "store/HandleSession.hpp"
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>

#define XAP_FORMAT_PLUS "+"

#define XAP_FORMAT_SPPL " +"
#define XAP_FORMAT_SUPL " "
#define XAP_CAN_BEGIN_THRESHOLD 100000

#define XAP_TOLERANCE_ORIGINAL 10
#define XAP_TOLERANCE_CORRECTED_LOW 50
#define XAP_TOLERANCE_CORRECTED_HIGH 1000

#define XAP_VERY_BIG_NO_HACK 10200

namespace zpds {
namespace search {
class SearchBase : virtual public ::zpds::store::HandleSession,
	virtual public BaseUtils, virtual public ReadIndex {

public:

	enum QueryWordsE {
		FULL_WORDS   = 0,
		LAST_PARTIAL = 1,
		ALL_PARTIAL  = 2
	};

	/**
	* CompletionQueryAction : simple query service action
	*
	* @param stptr
	*   ::zpds::utils::SharedTable::pointer stptr
	*
	* @param resp
	*   ::zpds::query::SearchCompletionRespT* resp
	*
	* @return
	*   none
	*/
	virtual void CompletionQueryAction(::zpds::utils::SharedTable::pointer stptr, ::zpds::query::SearchCompletionRespT* resp)=0;

protected:

	/**
	* SanitParams : sanitize user params
	*
	* @param params
	*   UsedParamsT* query params
	*
	* @return
	*   none
	*/
	void SanitParams( UsedParamsT* params) ;

	/**
	* SearchByProfile : executes basic rule Query
	*
	* @param stptr
	*   ::zpds::utils::SharedTable::pointer stptr
	*
	* @param qprof
	*   ::zpds::search::QueryProfT* qprof
	*
	* @param resp
	*   ::zpds::query::SearchCompletionRespT* resp
	*
	* @return
	*   none
	*/
	void SearchByProfile(
	    ::zpds::utils::SharedTable::pointer stptr,
	    ::zpds::search::QueryProfT* qprof,
	    ::zpds::query::SearchCompletionRespT* resp);

	/**
	* GetExterProfile : get the exter profile
	*
	* @param stptr
	*   ::zpds::utils::SharedTable::pointer stptr
	*
	* @param exter
	*   const std::string& exter name
	*
	* @param profile
	*   ::zpds::search::QueryProfT* profile
	*
	* @return
	*   true if found
	*/
	bool GetExterProfile(::zpds::utils::SharedTable::pointer stptr, const std::string& exter, ::zpds::search::QueryProfT* profile);

	/**
	* RuleSearch : do the search from rule, populate ids into cresp
	*
	* @param params
	*   UsedParamsT* query params
	*
	* @param rule
	*   const QueryRuleT* query rules
	*
	* @return
	*   bool if found
	*/
	bool RuleSearch( UsedParamsT* params, const QueryRuleT* rule);


	/**
	* FindNear: check if exists nearby
	*
	* @param qr
	*   ::zpds::search::UsedParamsT* params
	*
	* @param reset
	*   bool reset wordstr
	*
	* @return
	*   bool if at least one match
	*/
	bool FindNear(::zpds::search::UsedParamsT* qr, bool reset);

	/**
	* FindFull: check if exists
	*
	* @param qr
	*   ::zpds::search::UsedParamsT* query
	*
	* @param reset
	*   bool reset wordstr
	*
	* @return
	*   bool if at least one match
	*/
	bool FindFull(::zpds::search::UsedParamsT* qr, bool reset);

	WordVecT wordstr;

	/**
	* SetQuery : reset the wordstr
	*
	* @param query
	*   const std::string& query
	*
	* @return
	*   size_t
	*/
	size_t SetQuery(const std::string& query_);

	/**
	* GetQuerySize : get size of current query
	*
	* @return
	*   size_t
	*/
	size_t GetQuerySize();

	/**
	* GetQueryString: set query
	*
	* @param qtype
	*   QueryWordsE enum type
	*
	* @param extra
	*   const std::string& extra input
	*
	* @param full_prefix
	*   const std::string& full prefix
	*
	* @param part_prefix
	*   const std::string& part prefix
	*
	* @return
	*   std::string
	*/
	std::string GetQueryString(
	    QueryWordsE qtype, const std::string& extra,
	    const std::string& full_prefix, const std::string& part_prefix
	);


	/**
	* EstimateExec: estimate time based on keyword freq
	*
	* @param db
	*   DatabaseT& db
	*
	* @param input
	*   std::string& input
	*
	* @return
	*   uint64_t estimate
	*/
	uint64_t EstimateExec(DatabaseT& db, std::string& input);

};

} // namespace search
} // namespace zpds
#endif  // _ZPDS_SEARCH_SEARCH_BASE_HPP_

