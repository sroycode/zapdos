/**
 * @project zapdos
 * @file src/store/ProfileService.cc
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
 *  ProfileService.cc : Implementation of UpdateTraffic Service
 *
 */

#include <unordered_map>
#include <unordered_set>

#include "store/ProfileService.hpp"
#include "store/ProfileTable.hpp"
#include "store/SimpleTemplateTable.hpp"

#include "store/StoreTrans.hpp"


/**
* GetProfile : get profile by name
*
*/
bool zpds::store::ProfileService::GetProfile(
    ::zpds::utils::SharedTable::pointer stptr, ::zpds::store::ProfileT* prof)
{
	bool prof_found=false;
	if (stptr->dont_use_cache.Get()) {
		::zpds::store::ProfileTable prof_table{stptr->maindb.Get()};
		prof_found = prof_table.GetOne(prof,::zpds::store::U_PROFILE_NAME);
	}
	else {
		std::string temp;
		prof_found = stptr->dbcache->GetAssoc( EncodeSecondaryKey<std::string>(U_PROFILE_NAME, prof->name() ), temp);
		if (prof_found)
			prof_found = prof->ParseFromString(temp);
	}

	return prof_found;
}

/**
* GetProfileAction : gets profile info if exists
*
*/
void zpds::store::ProfileService::GetProfileAction(
    ::zpds::utils::SharedTable::pointer stptr, ::zpds::query::ProfileRespT* prof)
{
	::zpds::store::ProfileT prt;
	prt.set_name( prof->name() );
	::zpds::store::ProfileTable prt_table{stptr->maindb.Get()};
	bool prof_found = prt_table.GetOne(&prt,::zpds::store::U_PROFILE_NAME);
	if (prof_found) {
		prof->set_updated_at ( prt.updated_at() );
		prof->set_update ( prt.update() );
		prof->set_not_found( false );
	}
	else {
		prof->set_not_found( true );
	}
}

/**
* SetProfileAction : sets profile info if exists
*
*/
void zpds::store::ProfileService::SetProfileAction(
    ::zpds::utils::SharedTable::pointer stptr, ::zpds::query::ProfileRespT* prof)
{
	::zpds::store::ProfileT prt;
	prt.set_name( prof->name() );
	::zpds::store::ProfileTable prt_table{stptr->maindb.Get()};
	bool prof_found = prt_table.GetOne(&prt,::zpds::store::U_PROFILE_NAME);
	if (!prof_found)
		throw ::zpds::BadDataException("Cannot find this profile");
	if ( prt.passkey() != prof->oldpass() )
		throw ::zpds::BadDataException("Old password does not match");

	prt.set_updated_at ( prof->updated_at() );
	prt.set_passkey ( prof->newpass() );
	::zpds::store::TransactionT trans;
	if (!prt_table.AddRecord(&prt,&trans,prof_found))
		throw ::zpds::BadDataException("Cannot Insert data");
	::zpds::store::StoreTrans storetrans;
	storetrans.Commit(stptr,&trans,true); // as master
}

/**
* NewProfileAction : creates or updates profile
*
*/
void zpds::store::ProfileService::NewProfileAction(
    ::zpds::utils::SharedTable::pointer stptr, ::zpds::query::ProfileRespT* prof)
{
	::zpds::store::ProfileT prt;
	prt.set_name( prof->name() );
	::zpds::store::ProfileTable prt_table{stptr->maindb.Get()};
	bool prof_found = prt_table.GetOne(&prt,::zpds::store::U_PROFILE_NAME);

	prt.set_updated_at ( prof->updated_at() );
	prt.set_passkey ( prof->newpass() );
	prt.set_update ( prof->update() );
	if (!prof_found) {
		prt.set_id( stptr->maincounter.GetNext() );
		prt.set_name ( prof->name() );
		prt.set_created_at ( prof->updated_at() );
	}
	::zpds::store::TransactionT trans;
	if (!prt_table.AddRecord(&prt,&trans,prof_found))
		throw ::zpds::BadDataException("Cannot Add Record");
	::zpds::store::StoreTrans storetrans;
	storetrans.Commit(stptr,&trans,true); // as master
}

/**
* VerifyProfile: verifies passkey
*
*/
bool zpds::store::ProfileService::VerifyProfile(
    ::zpds::utils::SharedTable::pointer stptr, ::zpds::store::ProfileT* prof, bool update)
{
	::zpds::store::ProfileT prt;
	prt.set_name( prof->name() );
	if ( GetProfile(stptr,&prt) ) {
		if (update && prt.update()==0) return false; 
		return ( prt.passkey() == prof->passkey() );
	}
	return false;
}

/**
* GetSimpleTemplate : get profile template by name , qtyp
*
*/
bool zpds::store::ProfileService::GetSimpleTemplate(
    ::zpds::utils::SharedTable::pointer stptr, ::zpds::store::SimpleTemplateT* ptt)
{
	bool prof_found=false;
	if (stptr->dont_use_cache.Get()) {
		::zpds::store::SimpleTemplateTable ptt_table{stptr->maindb.Get()};
		prof_found = ptt_table.GetOne(ptt,::zpds::store::U_SIMPLETEMPLATE_NAME_QTYP);
	}
	else {
		std::string temp;
		prof_found = stptr->dbcache->GetAssoc(
		                 EncodeSecondaryKey<std::string,uint64_t>(U_SIMPLETEMPLATE_NAME_QTYP, ptt->name(), ptt->qtyp() ), temp);
		if (prof_found)
			prof_found = ptt->ParseFromString(temp);
	}

	return prof_found;
}

/**
* GetSimpleTemplateAction : gets profile template info if exists
*
*/
void zpds::store::ProfileService::GetSimpleTemplateAction(
    ::zpds::utils::SharedTable::pointer stptr, ::zpds::query::SimpleProRespT* prof)
{
	::zpds::store::SimpleTemplateT ptt;
	ptt.set_name( prof->name() );
	ptt.set_qtyp( prof->qtyp() );
	bool prof_found = GetSimpleTemplate(stptr,&ptt);

	if (prof_found) {
		ptt.mutable_qprof()->Swap( prof->mutable_qprof() ); // swap
		prof->set_ignore( ptt.ignore() );
		prof->set_updated_at ( ptt.updated_at() );
		prof->set_not_found( false );
	}
	else {
		prof->set_not_found( true );
	}

}

/**
* GetAllSimpleTemplateAction : gets all profile templates
*
*/
void zpds::store::ProfileService::GetAllSimpleTemplateAction(
    ::zpds::utils::SharedTable::pointer stptr, ::zpds::query::MultiSimpleProRespT* mprof)
{
	for ( auto i = 1 ; i <= ::zpds::search::QueryTypeE_MAX ; ++i ) {
		if (! ::zpds::search::QueryTypeE_IsValid(i) ) continue;

		::zpds::store::SimpleTemplateT ptt;
		ptt.set_name( mprof->name() );
		ptt.set_qtyp(  ::zpds::search::QueryTypeE (i) );
		bool prof_found = GetSimpleTemplate(stptr,&ptt);

		auto qtpl = mprof->add_qtpls();
		qtpl->set_qtyp ( ptt.qtyp() );
		if (prof_found) {
			ptt.mutable_qprof()->Swap( qtpl->mutable_qprof() ); // swap
			qtpl->set_ignore( ptt.ignore() );
			qtpl->set_updated_at ( ptt.updated_at() );
			qtpl->set_not_found( false );
		}
		else {
			qtpl->set_not_found( true );
		}
	}

}

/**
* SetSimpleTemplateAction : sets profile template
*
*/
void zpds::store::ProfileService::SetSimpleTemplateAction(
    ::zpds::utils::SharedTable::pointer stptr, ::zpds::query::SimpleProRespT* prof)
{
	// first verify password
	::zpds::store::ProfileT prt;
	prt.set_name( prof->name() );
	prt.set_passkey( prof->passkey() );
	if (!VerifyProfile(stptr,&prt))
		throw ::zpds::BadDataException("Cannot find this profile or passwd does not match");

	// check sanity
	std::unordered_map<int64_t, ::zpds::search::OrderTypeE > cmap;
	std::unordered_set<std::string> tfset;

	for (auto i =0 ; i < prof->qprof().orders_size() ; ++i ) {
		if ( prof->qprof().orders(i).weight() > ZPDS_MAX_RULE_WEIGHT )
			throw ::zpds::BadDataException("Weight cannot exceed max value: " + std::to_string(ZPDS_MAX_RULE_WEIGHT) );
		if ( prof->qprof().orders(i).weight() > 0 ) {
			auto it = cmap.find( prof->qprof().orders(i).weight() );
			if ( it == cmap.end() ) {
				cmap[ prof->qprof().orders(i).weight() ] = prof->qprof().orders(i).order_type();
			}
			else {
				if (it->second != prof->qprof().orders(i).order_type() )
					throw ::zpds::BadDataException("Same weight OrderTypeE mismatch");
			}
		}
		// rec_tagid
		auto jt = tfset.find( prof->qprof().orders(i).rec_tagid() );
		if ( jt != tfset.end() )
			throw ::zpds::BadDataException("rec_tagid cannot be repeated");
		tfset.insert( prof->qprof().orders(i).rec_tagid() );
	}

	// now find template
	::zpds::store::SimpleTemplateT ptt;
	ptt.set_name( prof->name() );
	ptt.set_qtyp( prof->qtyp() );
	::zpds::store::SimpleTemplateTable ptt_table{stptr->maindb.Get()};
	bool prof_found = ptt_table.GetOne(&ptt,::zpds::store::U_SIMPLETEMPLATE_NAME_QTYP);

	if (!prof_found) ptt.set_id( stptr->maincounter.GetNext() );
	ptt.set_ignore( prof->ignore() );
	ptt.mutable_qprof()->Swap( prof->mutable_qprof() ); // swap
	ptt.set_updated_at ( prof->updated_at() );
	::zpds::store::TransactionT trans;
	if (!ptt_table.AddRecord(&ptt,&trans,prof_found))
		throw ::zpds::BadDataException("Cannot Insert data");
	::zpds::store::StoreTrans storetrans;
	storetrans.Commit(stptr,&trans,true); // as master
}

