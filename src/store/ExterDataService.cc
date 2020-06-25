/**
 * @project zapdos
 * @file src/store/ExterDataService.cc
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
 *  ExterDataService.cc : Exter Data Manipulation impl
 *
 */
#include <unordered_map>
#include <unordered_set>

#include "store/ExterDataService.hpp"
#include "store/TempNameCache.hpp"

#include "store/ExterDataTable.hpp"
#include "crypto/CryptoTraits.hpp"

#include "store/StoreTrans.hpp"

/**
* AdminCreateAction : create exter user from data
*
*/
void zpds::store::ExterDataService::AdminCreateAction(
    ::zpds::utils::SharedTable::pointer stptr, ::zpds::store::ExterDataT* user, bool update, bool is_admin, bool has_profile)
{

	if (!stptr->is_master.Get())
		throw ::zpds::BadCodeException("Create exter is master only");

	::zpds::store::ExterDataT data;
	if (user->name().empty())
		throw ::zpds::BadDataException("Name cannot be empty",M_INVALID_PARAM);

	data.set_name( SanitNSLower(user->name()) );
	::zpds::store::ExterDataTable ext_table{stptr->maindb.Get()};
	bool admin_found = ext_table.GetOne(&data,::zpds::store::U_EXTERDATA_NAME);

	// dont create if exists
	if (admin_found && (!update) ) return;

	uint64_t currtime = ZPDS_CURRTIME_MS;
	// new user
	if (!admin_found) {
		// if passkey is blank refuse to create
		if (user->passkey().empty())
			throw ::zpds::BadDataException("Passkey cannot be empty",M_INVALID_PARAM);
		data.set_name( user->name() );
		data.set_created_at( currtime );
		data.set_notfound(false);
		// create signkey, this will not change
		auto signkey = data.mutable_signkey();
		::zpds::crypto::CryptoTraits::pointer signer = ::zpds::crypto::CryptoTraits::Create(ENCODEKEY_ALGO_DEFAULT);

		signkey->set_pubkey ( signer->GetPublicKey() );
		signkey->set_seckey ( signer->GetSecretKey() );
		signkey->set_algo ( ENCODEKEY_ALGO_DEFAULT );

		data.set_id(stptr->maincounter.GetNext() );
		data.set_manager(ZPDS_DEFAULT_ADMIN);
	}

	data.set_updated_at( currtime );

	// set admin flag if is_admin
	data.set_is_admin( is_admin );
	// set profile default if has_profile
	if ( has_profile ) 
		SetDefaultProfile(data.mutable_profiles());

	// set password, refuse if new password is blank
	if (user->passkey().empty())
		throw ::zpds::BadDataException("Passkey cannot be empty",M_INVALID_PARAM);
	data.set_passkey( user->passkey() );

	zpds::store::TransactionT trans;
	if (!ext_table.AddRecord(&data,&trans,admin_found))
		throw ::zpds::BadDataException("Cannot Insert admin data");
	zpds::store::StoreTrans storetrans(currtime);
	trans.set_updater( ZPDS_DEFAULT_ADMIN );
	storetrans.Commit(stptr,&trans,true); // as master
}

/**
* AllowedFieldsFilter : sets update variable
*
*/
int32_t zpds::store::ExterDataService::AllowedFieldsFilter(int32_t update )
{
	uint32_t xupdate=0;
	if (update & ZPDS_UDFLD_SIGNKEY) xupdate+=ZPDS_UDFLD_SIGNKEY;
	if (update & ZPDS_UDFLD_PASSKEY) xupdate+=ZPDS_UDFLD_PASSKEY;
	if (update & ZPDS_UDFLD_ROLE) xupdate+=ZPDS_UDFLD_ROLE;
	if (update & ZPDS_UDFLD_INFO) xupdate+=ZPDS_UDFLD_INFO;
	if (update & ZPDS_UDFLD_GNAME) xupdate+=ZPDS_UDFLD_GNAME;
	if (update & ZPDS_UDFLD_MOBILE) xupdate+=ZPDS_UDFLD_MOBILE;
	if (update & ZPDS_UDFLD_EMAIL) xupdate+=ZPDS_UDFLD_EMAIL;
	if (update & ZPDS_UDFLD_PROFILES) xupdate+=ZPDS_UDFLD_PROFILES;
	// if (update & ZPDS_UDFLD_INVITED) xupdate+=ZPDS_UDFLD_INVITED;
	if (update & ZPDS_UDFLD_TAGS) xupdate+=ZPDS_UDFLD_TAGS;
	// if (update & ZPDS_UDFLD_SUBSCR) xupdate+=ZPDS_UDFLD_SUBSCR;
	// if (update & ZPDS_UDFLD_CATEGORIES) xupdate+=ZPDS_UDFLD_CATEGORIES;
	return xupdate;
}

/**
* PrepareData : sets exterdata, updater is expected to be prefilled
*
*/
bool zpds::store::ExterDataService::PrepareData(
    ::zpds::utils::SharedTable::pointer stptr,
    ::zpds::store::ExterDataT* updater,
    ::zpds::store::ExterDataT* data,
    int32_t action, int32_t fields, bool merge)
{

	if ( updater->id() == 0 ) {
		throw ::zpds::BadDataException("Updater cannot be found",M_INVALID_PARAM);
	}

	::zpds::store::ExterDataTable exter_table{stptr->maindb.Get()};
	::zpds::store::ExterDataT cdata;
	if (data->name().empty())
		throw ::zpds::BadDataException("Name needed",M_INVALID_PARAM);
	cdata.set_name( data->name() );

	bool user_found = exter_table.GetOne(&cdata,::zpds::store::U_EXTERDATA_NAME);
	bool&& user_not_found = !user_found;

	if (user_found && action==ZPDS_UACTION_CREATE)
		throw ::zpds::BadDataException("Name already exists, cannot create",M_INVALID_PARAM);

	if ((user_not_found) && action==ZPDS_UACTION_UPDATE)
		throw ::zpds::BadDataException("Name doesnot exist, cannot update",M_INVALID_PARAM);

	if ((user_not_found) && action==ZPDS_UACTION_DELETE)
		throw ::zpds::BadDataException("Name doesnot exist, cannot delete",M_INVALID_PARAM);

	if ( (!updater->is_admin()) && user_found && cdata.locked() )
		throw ::zpds::BadDataException("Exter Data is locked, cannot edit",M_INVALID_PARAM);

	int32_t perms = 0;
	// Logic: check if self or admin
	if (updater->name() == data->name() )
		perms = ZPDS_WRITR_IS_SELF;
	else if (updater->is_admin())
		perms = ZPDS_WRITR_IS_ADMIN;


	// handle delete
	if (action==ZPDS_UACTION_DELETE && ( ZPDS_WRITR_IS_ADMIN )) {
		cdata.set_is_deleted( data->is_deleted() );
		cdata.set_expired( data->expired() );
		cdata.set_locked( data->locked() );
		cdata.Swap(data);
		return user_found;
	}

	// proceed ahead if not delete
	bool changes=false;

	// Modify: signkey
	if ( (fields & ZPDS_UDFLD_SIGNKEY) && ( perms & ( ZPDS_WRTPED_SIGNKEY ) ) ) {
		// create signkey, this will not change TODO: provision to change
		auto signkey = cdata.mutable_signkey();
		::zpds::crypto::CryptoTraits::pointer signer = ::zpds::crypto::CryptoTraits::Create(ENCODEKEY_ALGO_DEFAULT);
		signkey->set_pubkey ( signer->GetPublicKey() );
		signkey->set_seckey ( signer->GetSecretKey() );
		signkey->set_algo ( ENCODEKEY_ALGO_DEFAULT );
		changes=true;
	}

#define LOCAL_HANDLE_STRING(AAA) if (!(merge && data->AAA().empty())) { cdata.set_##AAA( data->AAA() ); changes=true; }
#define LOCAL_HANDLE_ENUM(AAA) if (!(merge && (int)data->AAA()==0)) { cdata.set_##AAA( data->AAA() ); changes=true; }
#define LOCAL_HANDLE_NUMBER(AAA) if (!(merge && data->AAA()==0)) { cdata.set_##AAA( data->AAA() ); changes=true; }

	// Modify: passkey
	if ( (fields & ZPDS_UDFLD_PASSKEY) && ( perms & ( ZPDS_WRTPED_PASSKEY ) ) ) {
		if (data->passkey().empty() )
			throw ::zpds::BadDataException("Passkey is blank");
		LOCAL_HANDLE_STRING(passkey)
	}

	// Modify: role
	if ( (fields & ZPDS_UDFLD_ROLE) && ( perms & ( ZPDS_WRTPED_ROLE ) ) ) {
		cdata.set_can_add_tags( data->can_add_tags() );
		cdata.set_can_add_category( data->can_add_category() );
		cdata.set_can_add_users( data->can_add_users() );
		cdata.set_can_add_items( data->can_add_items() );
		changes=true;
	}

	// Modify: description
	if ( (fields & ZPDS_UDFLD_INFO) && ( perms & ( ZPDS_WRTPED_INFO ) ) ) {
		LOCAL_HANDLE_STRING(description)
	}

	// Modify: given_name
	if ( (fields & ZPDS_UDFLD_GNAME) && ( perms & ( ZPDS_WRTPED_GNAME ) ) ) {
		LOCAL_HANDLE_STRING(given_name)
	}

	// Modify: subscription

	// Modify: mobile + primary_mobile
	if ( (fields & ZPDS_UDFLD_MOBILE) && ( perms & ( ZPDS_WRTPED_MOBILE ) ) ) {
		LOCAL_HANDLE_STRING(primary_mobile)
	}

	// Modify: email + primary_email
	if ( (fields & ZPDS_UDFLD_EMAIL) && ( perms & ( ZPDS_WRTPED_EMAIL ) ) ) {
		LOCAL_HANDLE_STRING(primary_email)
	}

	// Modify: profiles ( allow merge )
	if ( (fields & ZPDS_UDFLD_PROFILES) && ( perms & ( ZPDS_WRTPED_PROFILES ) ) ) {
		if ( HandleQueryProfiles(data->mutable_profiles(),cdata.mutable_profiles(),merge) )
			changes=true;
	}

	// Modify: invited

	// Modify: tags ( allow merge )
	if ( (fields & ZPDS_UDFLD_TAGS) && ( perms & ( ZPDS_WRTPED_TAGS ))) {
		if ( HandleUserTags(stptr,data->mutable_tags(),cdata.mutable_tags(), {GetKeyType()},merge) )
			changes=true;
	}

	// Modify: categories ( allow merge )

	// add id name if not found
	if (user_not_found && (action==ZPDS_UACTION_CREATE || action==ZPDS_UACTION_UPSERT) ) {
		cdata.set_notfound( false );
	}

#undef LOCAL_HANDLE_STRING
#undef LOCAL_HANDLE_ENUM
#undef LOCAL_HANDLE_NUMBER

	// finally swap
	cdata.Swap(data);
	return user_found;

}

/**
* ManageDataAction : update externaldata
*
*/
void zpds::store::ExterDataService::ManageDataAction(
    ::zpds::utils::SharedTable::pointer stptr,
    ::zpds::query::UserDataRespT* resp)

{
	if (!stptr->is_master.Get())
		throw ::zpds::BadCodeException("ExterData change is master only");

	auto status = resp->mutable_status();
	int32_t action=0;
	bool merge = false;
	std::tie(action,merge)=WriteActionFilter(resp->action());
	int fields = AllowedFieldsFilter(resp->fields());


	::zpds::store::ExterDataT updater;
	updater.set_name( resp->username() );
	updater.set_sessionkey( resp->sessionkey() );
	updater.set_keytype( ::zpds::store::K_EXTERDATA );
	if (! CheckSession(stptr, &updater, true) )
		throw zpds::BadDataException("Invalid updater or Invalid session",M_INVALID_PARAM);

	// just add the payload to payloads for easy processing
	if (!resp->payload().name().empty())
		resp->add_payloads()->Swap(resp->mutable_payload());
	status->set_inputcount( resp->payloads_size() );

	// start addition , first format the data , then update
	uint64_t currtime = ZPDS_CURRTIME_MS;

	zpds::store::TransactionT trans;
	::zpds::store::ExterDataTable ext_table{stptr->maindb.Get()};

	std::unordered_set<std::string> forbidden { ZPDS_FORBIDDEN_USERNAMES };
	::zpds::store::TempNameCache namecache{stptr};

	// update payloads if exists
	for (size_t i = 0 ; i<resp->payloads_size(); ++i)	{
		auto rdata = resp->mutable_payloads(i);
		if ( rdata->name().empty() )
			throw zpds::BadDataException("Name not found: " + rdata->name(),M_INVALID_PARAM);
		if (rdata->name() != SanitNSLower(rdata->name()) )
			throw zpds::BadDataException("Name invalid, lowercase and no spaces: " + rdata->name(),M_INVALID_PARAM);

		bool ext_found = PrepareData(stptr,&updater,rdata,action,fields,merge);

		// reserve this name
		if (namecache.CheckLocal(::zpds::store::K_EXTERDATA,rdata->name()))
			throw zpds::BadDataException("Duplicate name found: " + rdata->name(),M_INVALID_PARAM);
		if (!namecache.ReserveName(::zpds::store::K_EXTERDATA,rdata->name(),false))
			throw zpds::BadDataException("This name is being worked on: " + rdata->name(),M_INVALID_PARAM);

		if (! ext_found) {
			if ( forbidden.find(rdata->name()) != forbidden.end() )
				throw zpds::BadDataException("Name not valid: " + rdata->name(),M_INVALID_PARAM);
			rdata->set_manager( updater.name() );
			rdata->set_id(stptr->maincounter.GetNext() );
			rdata->set_created_at( currtime );
		}

		rdata->set_updated_at( currtime );
		status->set_updatecount( status->updatecount() + 1 );

		if (!ext_table.AddRecord(rdata,&trans,ext_found))
			throw ::zpds::BadDataException("Cannot Insert exter data");
	}

	// commit to table
	zpds::store::StoreTrans storetrans(currtime);
	trans.set_updater( updater.name() );
	storetrans.Commit(stptr,&trans,true); // as master

	status->set_success(true);
}

/**
* ReadData : sets userdata for read , assuming reader is set
*
*/
void zpds::store::ExterDataService::ReadData(
    ::zpds::utils::SharedTable::pointer stptr,
    ::zpds::store::ExterDataT* reader,
    ::zpds::store::ExterDataT* data, int32_t fields)
{

	::zpds::store::ExterDataTable exter_table{stptr->maindb.Get()};
	::zpds::store::ExterDataT cdata;

	if ( data->name().empty())
		throw ::zpds::BadDataException("Name needed",M_INVALID_PARAM);
	cdata.set_name( data->name() );

	bool user_found = exter_table.GetOne(&cdata,::zpds::store::U_EXTERDATA_NAME);

	if (!user_found) {
		data->set_notfound(true);
		return;
	}


	// Since reader is always exter default is other
	::zpds::query::ReaderTypeE rtype = ::zpds::query::R_OTHER;
	// check if admin, manager or self
	if ( reader->is_admin() )
		rtype = ::zpds::query::R_ADMIN;
	else if ( reader->name() == cdata.manager() )
		rtype = ::zpds::query::R_CREATR;
	else if ( reader->name() == cdata.name() )
		rtype = ::zpds::query::R_SELF;

	int32_t perms = ReaderTypeFilter(rtype);

	// Read: signkey disabled
	if ( (fields & ZPDS_UDFLD_SIGNKEY) && ( perms & ( ZPDS_READED_SIGNKEY ) ) ) {
		// data->mutable_signkey()->Swap( cdata.mutable_signkey() );
	}

	// Read: passkey disabled
	if ( (fields & ZPDS_UDFLD_PASSKEY) && ( perms & ( ZPDS_READED_PASSKEY ) ) ) {
		// data->set_passkey( cdata.passkey() );
	}

	// Read: role
	if ( (fields & ZPDS_UDFLD_ROLE) && ( perms & ( ZPDS_READED_ROLE ) ) ) {
		data->set_can_add_tags( cdata.can_add_tags() );
		data->set_can_add_category( cdata.can_add_category() );
		data->set_can_add_users( cdata.can_add_users() );
		data->set_can_add_items( cdata.can_add_items() );
	}

	// Read: description
	if ( (fields & ZPDS_UDFLD_INFO) && ( perms & ( ZPDS_READED_INFO ) ) ) {
		data->set_description( cdata.description() );
	}

	// Read: given_name
	if ( (fields & ZPDS_UDFLD_GNAME) && ( perms & ( ZPDS_READED_GNAME ) ) ) {
		data->set_given_name( cdata.given_name() );
	}

	// Read: subscription

	// Read: mobile + primary_mobile
	if ( (fields & ZPDS_UDFLD_MOBILE) && ( perms & ( ZPDS_READED_MOBILE ) ) ) {
		data->set_primary_mobile(cdata.primary_mobile());
	}

	// Read: email + primary_email
	if ( (fields & ZPDS_UDFLD_EMAIL) && ( perms & ( ZPDS_READED_EMAIL ) ) ) {
		data->set_primary_email(cdata.primary_email());
	}

	// Read: profiles
	if ( (fields & ZPDS_UDFLD_PROFILES) && ( perms & ( ZPDS_READED_PROFILES ) ) ) {
		data->mutable_profiles()->Swap(cdata.mutable_profiles() );
	}

	// Read: invited

	// Read: tags
	if ( (fields & ZPDS_UDFLD_TAGS) && ( perms & ( ZPDS_READED_TAGS ))) {
		for (auto i=0; i< cdata.tags_size(); ++i) {
			auto tag = data->add_tags();
			tag->set_name( cdata.tags(i).name() );
			tag->set_value( cdata.tags(i).value() );
		}
	}

	// Read: categories

}

/**
* ReadDataAction : read externaldata
*
*/
void zpds::store::ExterDataService::ReadDataAction(
    ::zpds::utils::SharedTable::pointer stptr,
    ::zpds::query::UserDataRespT* resp)

{
	int fields = AllowedFieldsFilter(resp->fields());
	auto status = resp->mutable_status();

	::zpds::store::ExterDataT reader;
	reader.set_name( resp->username() );
	reader.set_sessionkey( resp->sessionkey() );
	reader.set_keytype( ::zpds::store::K_EXTERDATA );
	if (! CheckSession(stptr, &reader, true) )
		throw zpds::BadDataException("Invalid reader or Invalid session",M_INVALID_PARAM);

	// update payload if exists
	if (!resp->payload().name().empty()) {
		auto rdata = resp->mutable_payload();
		ReadData(stptr,&reader,rdata,fields);
		status->set_inputcount( status->inputcount() + ( rdata->notfound() ? 0 : 1 ) );
	}

	// update payloads if exists
	for (auto i = 0 ; i<resp->payloads_size(); ++i)	{
		if (resp->payloads(i).name().empty()) continue;
		auto rdata = resp->mutable_payloads(i);
		ReadData(stptr,&reader,rdata,fields);
		status->set_inputcount( status->inputcount() + ( rdata->notfound() ? 0 : 1 ) );
	}

	status->set_success(true);
}

/**
* GetKeyType : handles keytype for tags
*
*/
::zpds::store::KeyTypeE zpds::store::ExterDataService::GetKeyType() const
{
	return ::zpds::store::K_EXTERDATA;
}

/**
* ResetPassAction : update password
*
*/
void zpds::store::ExterDataService::ResetPassAction(::zpds::utils::SharedTable::pointer stptr, ::zpds::query::ResetRespT* resp)
{
	if (!stptr->is_master.Get())
		throw ::zpds::BadCodeException("Reset pass is master only");

	if (resp->username().empty())
		throw ::zpds::BadDataException("Name cannot be empty",M_INVALID_PARAM);
	if (resp->newpass().empty())
		throw ::zpds::BadDataException("Newpass cannot be empty",M_INVALID_PARAM);
	if (resp->oldpass().empty() && resp->otp().empty())
		throw ::zpds::BadDataException("Need oldpass or OTP",M_INVALID_PARAM);

	::zpds::store::ExterDataT data;
	data.set_name( resp->username());
	::zpds::store::ExterDataTable ext_table{stptr->maindb.Get()};
	bool user_found = ext_table.GetOne(&data,::zpds::store::U_EXTERDATA_NAME);
	if (!user_found)
		throw ::zpds::BadDataException("This user does not exist",M_INVALID_PARAM);

	if (resp->otp().empty() && data.passkey() != resp->oldpass() )
		throw ::zpds::BadDataException("Old password is bad",M_INVALID_PARAM);

	if (resp->oldpass().empty() && data.last_otp() != resp->otp() )
		throw ::zpds::BadDataException("OTP mismatch",M_INVALID_PARAM);

	uint64_t currtime = ZPDS_CURRTIME_MS;
	data.set_passkey( resp->newpass() );
	data.set_updated_at( currtime );

	zpds::store::TransactionT trans;
	if (!ext_table.AddRecord(&data,&trans,user_found))
		throw ::zpds::BadDataException("Cannot Insert passkey data");
	zpds::store::StoreTrans storetrans(currtime);
	trans.set_updater( ZPDS_DEFAULT_ADMIN );
	storetrans.Commit(stptr,&trans,true); // as master
	resp->mutable_status()->set_success(true);
}

