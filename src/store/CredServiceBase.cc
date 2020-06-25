/**
 * @project zapdos
 * @file src/store/CredServiceBase.cc
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
 *  CredServiceBase.cc : 
 *
 */
#include "google/protobuf/descriptor.h"
#include "store/CredServiceBase.hpp"
#include "crypto/CryptoTraits.hpp"

/**
* GetSession: get session key
*
*/
void zpds::store::CredServiceBase::GetSession(::zpds::utils::SharedTable::pointer stptr, ::zpds::query::LoginRespT* data) const
{
	if ( data->username().empty() )
		throw zpds::InitialException("username variable needed for exter session",M_INVALID_PARAM);

	if ( data->password().empty() )
		throw zpds::InitialException("password variable needed for exter session",M_INVALID_PARAM);

	zpds::store::UserDataT user; // common data type
	user.set_name( data->username() );
	Get(stptr,&user); // inherited function gets
	if (user.notfound() || user.passkey() != data->password())
		throw zpds::BadDataException("No such user or Invalid passkey",M_INVALID_PARAM);

	// add session key here
	::zpds::store::UserDataT xuser;
	xuser.set_name( user.name() );
	xuser.set_updated_at( ZPDS_CURRTIME_MS );
	auto signkey = user.mutable_signkey();

	auto signer = ::zpds::crypto::CryptoTraits::Create(signkey->algo(), signkey->pubkey(), signkey->seckey());
	std::string tmpstr,sessionkey;
	xuser.SerializeToString(&tmpstr);
	signer->Encode(tmpstr,sessionkey);
	data->set_sessionkey(sessionkey);

	// clear the password keys
	data->clear_password();
}

/**
* CheckSession : check the session key
*
*/
bool zpds::store::CredServiceBase::CheckSession(::zpds::utils::SharedTable::pointer stptr, ::zpds::store::UserDataT* data) const
{
	if ( data->name().empty() )
		throw zpds::InitialException("username variable needed for exter check",M_INVALID_PARAM);

	if ( data->sessionkey().empty() )
		throw zpds::InitialException("sessionkey variable needed for exter check",M_INVALID_PARAM);

	std::string sessionkey = data->sessionkey(); // dont swap here

	Get(stptr,data); // inherited function gets
	if (data->notfound())
		throw zpds::BadDataException("No such user or Invalid session",M_INVALID_PARAM);

	// decode here
	auto signkey = data->mutable_signkey();
	auto signer = ::zpds::crypto::CryptoTraits::Create(signkey->algo(), signkey->pubkey(), signkey->seckey());
	std::string tmpstr;
	signer->Decode(sessionkey,tmpstr);
	::zpds::store::UserDataT xuser;
	xuser.ParseFromString(tmpstr);

	if (xuser.name() != data->name())
		throw zpds::BadDataException("No such user or Invalid session",M_INVALID_PARAM);

	// Key Expires in 
	uint64_t currtime = ZPDS_CURRTIME_MS;

	// Treat future keys as invalid
	if ( xuser.updated_at() > currtime  )
		throw zpds::BadDataException("No such user or Invalid session",M_INVALID_PARAM);

	if ( currtime - xuser.updated_at() > ZPDS_SESSION_LIFETIME_MS )
		throw zpds::BadDataException("Session Key has Expired",M_INVALID_PARAM);

	return true;

}

