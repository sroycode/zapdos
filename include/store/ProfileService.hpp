/**
 * @project zapdos
 * @file include/store/ProfileService.hpp
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
 *  ProfileService.hpp : Header for profile service
 *
 */
#ifndef _ZPDS_STORE_PROFILE_SERVICE_HPP_
#define _ZPDS_STORE_PROFILE_SERVICE_HPP_

#include <unordered_set>

#include "query/QueryBase.hpp"
#include "store/StoreBase.hpp"

namespace zpds {
namespace store {

class ProfileService : public StoreBase  {
public:
	using StrSetT = std::unordered_set<std::string>;

	/**
	* GetProfileAction : gets profile info if exists
	*
	* @param stptr
	*   ::zpds::utils::SharedTable::pointer stptr
	*
	* @param resp
	*   ::zpds::query::ProfileRespT* prof
	*
	* @return
	*   none throws if not ok
	*/
	void GetProfileAction(::zpds::utils::SharedTable::pointer stptr, ::zpds::query::ProfileRespT* prof);

	/**
	* SetProfileAction : sets profile info if exists
	*
	* @param stptr
	*   ::zpds::utils::SharedTable::pointer stptr
	*
	* @param resp
	*   ::zpds::query::ProfileRespT* prof
	*
	* @return
	*   none throws if not ok
	*/
	void SetProfileAction(::zpds::utils::SharedTable::pointer stptr, ::zpds::query::ProfileRespT* prof);

	/**
	* NewProfileAction : creates or updates profile
	*
	* @param stptr
	*   ::zpds::utils::SharedTable::pointer stptr
	*
	* @param resp
	*   ::zpds::query::ProfileRespT* prof
	*
	* @return
	*   none throws if not ok
	*/
	void NewProfileAction(::zpds::utils::SharedTable::pointer stptr, ::zpds::query::ProfileRespT* prof);

	/**
	* VerifyProfile : get profile by name and verify passkey
	*
	* @param stptr
	*   ::zpds::utils::SharedTable::pointer stptr
	*
	* @param resp
	*   ::zpds::store::ProfileT* prof
	*
	* @return
	*   bool if verified
	*/
	bool VerifyProfile(::zpds::utils::SharedTable::pointer stptr, ::zpds::store::ProfileT* prof);


	/**
	* GetSimpleTemplateAction : gets simple profile template if exists
	*
	* @param stptr
	*   ::zpds::utils::SharedTable::pointer stptr
	*
	* @param resp
	*   ::zpds::query::SimpleProRespT* prof
	*
	* @return
	*   none throws if not ok
	*/
	void GetSimpleTemplateAction(::zpds::utils::SharedTable::pointer stptr, ::zpds::query::SimpleProRespT* prof);

	/**
	* GetAllSimpleTemplateAction : get all simple profile templates
	*
	* @param stptr
	*   ::zpds::utils::SharedTable::pointer stptr
	*
	* @param resp
	*   ::zpds::query::MultiSimpleProRespT* prof
	*
	* @return
	*   none throws if not ok
	*/
	void GetAllSimpleTemplateAction(::zpds::utils::SharedTable::pointer stptr, ::zpds::query::MultiSimpleProRespT* mprof);

	/**
	* SetSimpleTemplateAction : sets simple profile info if exists
	*
	* @param stptr
	*   ::zpds::utils::SharedTable::pointer stptr
	*
	* @param resp
	*   ::zpds::query::SimpleProRespT* prof
	*
	* @return
	*   none throws if not ok
	*/
	void SetSimpleTemplateAction(::zpds::utils::SharedTable::pointer stptr, ::zpds::query::SimpleProRespT* prof);

protected:

	/**
	* GetProfile : get profile by name 
	*
	* @param stptr
	*   ::zpds::utils::SharedTable::pointer stptr
	*
	* @param resp
	*   ::zpds::store::ProfileT* prof
	*
	* @return
	*   bool if found
	*/
	bool GetProfile(::zpds::utils::SharedTable::pointer stptr, ::zpds::store::ProfileT* prof);

	/**
	* GetSimpleTemplate : get simple profile template by name , qtyp
	*
	* @param stptr
	*   ::zpds::utils::SharedTable::pointer stptr
	*
	* @param resp
	*   ::zpds::store::SimpleTemplateT* pt
	*
	* @return
	*   bool if found
	*/
	bool GetSimpleTemplate(::zpds::utils::SharedTable::pointer stptr, ::zpds::store::SimpleTemplateT* ptt);

};
} // namespace store
} // namespace zpds
#endif /* _ZPDS_STORE_PROFILE_SERVICE_HPP_ */
