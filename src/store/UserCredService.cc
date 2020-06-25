/**
 * @project zapdos
 * @file src/store/UserCredService.cc
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
 *  UserCredService.cc : User Credentials handling impl
 *
 */
#include "store/UserCredService.hpp"
#include "store/UserDataTable.hpp"
#include "utils/PrintWith.hpp"

/**
* Get : gets the user
*
*
*/
void zpds::store::UserCredService::Get(::zpds::utils::SharedTable::pointer stptr, ::zpds::store::UserDataT* data) const
{
	std::string temp;
	bool user_found= stptr->dbcache->GetAssoc( EncodeSecondaryKey<std::string>(U_USERDATA_NAME, data->name() ), temp);
	if (user_found) {
		user_found = data->ParseFromString(temp);
	}
	else {
		::zpds::store::UserDataTable user_table{stptr->maindb.Get()};
		user_found = user_table.GetOne(data,::zpds::store::U_USERDATA_NAME);
		if (user_found) {
			stptr->dbcache->SetAssoc( EncodeSecondaryKey<std::string>(U_USERDATA_NAME, data->name() ), Pack(data) );
			DLOG(INFO) << "Set Cache User " << data->name();
			// add all user categories to cache with ts
			for (auto i = 0; i < data->categories_size(); ++i) {
				stptr->dbcache->SetAssoc(
				    EncodeSecondaryKey<std::string,std::string>(F_USERDATA_NAME_CATEGORY, data->name(), data->categories(i)),
				    ::zpds::utils::PrintWithPipe::String(data->updated_at()), false);
			}
		}
	}
	if (!user_found) data->set_notfound(true);
	else data->set_keytype(::zpds::store::K_USERDATA);
}


/**
* Pack : shrink to bare essentials for packing into cache, dont alter data
*
*/
std::string zpds::store::UserCredService::Pack(::zpds::store::UserDataT* data) const
{
	::zpds::store::UserDataT tocache;
	tocache.set_id( data->id() );
	tocache.set_manager( data->manager() );
	tocache.mutable_signkey()->CopyFrom( data->signkey() );
	tocache.set_passkey( data->passkey() );
	tocache.set_name( data->name() );
	tocache.set_lang( data->lang() );
	tocache.set_is_invited( data->is_invited() );

	tocache.set_subscription( data->subscription() );

	tocache.mutable_categories()->CopyFrom( data->categories() );

	// set updated_at to compare
	tocache.set_updated_at( data->updated_at() );

	tocache.set_locked( data->locked() );
	tocache.set_expired( data->expired() );
	tocache.set_is_deleted( data->is_deleted() );
	std::string packed;
	tocache.SerializeToString(&packed);
	return packed;
}

/**
* ValidateUserCategories : gets the user categories
*
*/
bool zpds::store::UserCredService::ValidateUserCategories(::zpds::utils::SharedTable::pointer stptr,
        ::zpds::store::UserDataT* data,
        ::google::protobuf::RepeatedPtrField< std::string>* categories) const
{
	// puts in cache if not already
	if ( data->id() == 0 ) Get(stptr,data);
	if (data->notfound() || data->is_deleted() ) return false;
	// compare only latest updated_at which gets set with user change
	std::string to_compare = ::zpds::utils::PrintWithPipe::String(data->updated_at());

	for (auto i=0; i<categories->size(); ++i) {
		auto var = categories->Mutable(i);
		std::string updated_at;
		bool cat_found = stptr->dbcache->GetAssoc(
		                     EncodeSecondaryKey<std::string,std::string>(F_USERDATA_NAME_CATEGORY, data->name(), categories->Get(i)),
		                     updated_at);
		if (cat_found && updated_at==to_compare)  return true;
	}
	return false;
}

