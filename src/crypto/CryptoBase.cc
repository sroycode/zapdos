/**
 * @project zapdos
 * @file src/crypto/CryptoBase.cc
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
 *  CryptoBase.cc : Abstract Base Class for all Sign keys Impl
 *
 */
#include "crypto/CryptoBase.hpp"
#include "utils/B64String.hpp"

/**
* CryptoBase : constructor
*
*/
zpds::crypto::CryptoBase::CryptoBase () : scope(0) {}

/**
* GetAlgo : get key algo
*
*/
const std::string zpds::crypto::CryptoBase::GetAlgo()
{
	throw zpds::BadCodeException("Base class CryptoBase should never be called for GetAlgo");
}

/**
* GetSeed : return seed of private key
*
*/
std::string zpds::crypto::CryptoBase::GetSeed() const
{
	throw zpds::BadCodeException("GetSeed is not Implemented for this class");
}

/**
* GetPublicKey : return public key
*
*/
std::string zpds::crypto::CryptoBase::GetPublicKey() const
{
	return ::zpds::utils::B64String::Encode(pubkey);
}

/**
* GetSecretKey : return private key
*
*/
std::string zpds::crypto::CryptoBase::GetSecretKey() const
{
	return ::zpds::utils::B64String::Encode(seckey);
}

/**
* GetSignature : return signature
*
*/
std::string zpds::crypto::CryptoBase::GetSignature(const std::string& input) const
{
	throw zpds::BadCodeException("This class cannot Sign");
}

/**
* VerifySignature : return signature
*
*/
bool zpds::crypto::CryptoBase::VerifySignature(const std::string& signature, const std::string& input) const
{
	throw zpds::BadCodeException("This class cannot Verify");
}

/**
* Encode : return encoded
*
*/
bool zpds::crypto::CryptoBase::Encode(const std::string& input, std::string& output) const
{
	throw zpds::BadCodeException("This class cannot Encode");
}

/**
* Decode : return decoded
*
*/
bool zpds::crypto::CryptoBase::Decode(const std::string& input, std::string& output) const
{
	throw zpds::BadCodeException("This class cannot Decode");
}
