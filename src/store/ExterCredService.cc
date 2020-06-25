/**
 * @project zapdos
 * @file src/store/ExterCredService.cc
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
 *  ExterCredService.cc : Exter Credentials Handling impl
 *
 */
#include "store/ExterCredService.hpp"
#include "store/ExterDataTable.hpp"
#include "store/StoreTrans.hpp"
#include "crypto/CryptoTraits.hpp"

/**
* Get : gets the exter user and populates cache
*
*/
void zpds::store::ExterCredService::Get(::zpds::utils::SharedTable::pointer stptr, ::zpds::store::ExterDataT* data) const
{
	std::string temp;
	bool user_found= stptr->dbcache->GetAssoc( EncodeSecondaryKey<std::string>(U_EXTERDATA_NAME, data->name() ), temp);
	if (user_found) {
		user_found = data->ParseFromString(temp);
	}
	else {
		::zpds::store::ExterDataTable user_table{stptr->maindb.Get()};
		user_found = user_table.GetOne(data,::zpds::store::U_EXTERDATA_NAME);
		if (user_found) {
			stptr->dbcache->SetAssoc( EncodeSecondaryKey<std::string>(U_EXTERDATA_NAME, data->name() ), Pack(data) );
			DLOG(INFO) << "Set Cache Exter " << data->name();
		}
	}
	if (!user_found) data->set_notfound(true);
	else data->set_keytype(::zpds::store::K_EXTERDATA);
}

/**
* Pack : shrink to bare essentials for packing into cache
*
*/
std::string zpds::store::ExterCredService::Pack(::zpds::store::ExterDataT* data) const
{
	::zpds::store::ExterDataT tocache;
	tocache.set_id( data->id() );
	tocache.mutable_signkey()->CopyFrom( data->signkey() );
	tocache.set_passkey( data->passkey() );
	tocache.set_manager( data->manager() );
	tocache.set_name( data->name() );
	// tocache.set_is_invited( data->is_invited() );
	tocache.set_is_admin( data->is_admin() );
	tocache.set_can_add_users( data->can_add_users() );
	tocache.set_can_add_tags( data->can_add_tags() );
	tocache.set_can_add_category( data->can_add_category() );
	tocache.set_can_add_items( data->can_add_items() );

	tocache.mutable_profiles()->CopyFrom( data->profiles() );

	tocache.set_locked( data->locked() );
	tocache.set_expired( data->expired() );
	tocache.set_is_deleted( data->is_deleted() );
	std::string packed;
	tocache.SerializeToString(&packed);
	return packed;
}
