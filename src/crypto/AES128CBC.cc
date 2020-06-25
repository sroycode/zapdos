/**
 * @project zapdos
 * @file src/crypto/AES128CBC.cc
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
 *  AES128CBC.cc : AES128CBC Sign Key Impl
 *
 */
#include "utils/B64String.hpp"
#include "crypto/AES128CBC.hpp"
#include "utils/Randomer.hpp"
#include "Anchor.hpp"

/**
* GetAlgo : get key algo
*
*/
const std::string zpds::crypto::AES128CBC::GetAlgo()
{
	return ENCODEKEY_ALGO_AES128CBC;
}

/**
* AES128CBC : constructor
*
*/
zpds::crypto::AES128CBC::AES128CBC() : CryptoBase()
{
	CryptoPP::AutoSeededRandomPool prng;
	seckey.resize(CryptoPP::AES::DEFAULT_KEYLENGTH);
	prng.GenerateBlock((CryptoPP::byte*)&seckey[0], CryptoPP::AES::DEFAULT_KEYLENGTH);
	scope=ZPDS_CRYPTO_SCOPE_ENCODE_DECODE;
}

/**
* AES128CBC : constructor with seed is actually symmetric key
*
*/
zpds::crypto::AES128CBC::AES128CBC (const std::string& seed) : CryptoBase()
{
	seckey = ::zpds::utils::B64String::Decode(seed);
	scope=ZPDS_CRYPTO_SCOPE_ENCODE_DECODE;
}

/**
* AES128CBC : constructor with 2 keys for compatibility
*
*/
zpds::crypto::AES128CBC::AES128CBC (const std::string& pubkey_, const std::string& seckey_) : CryptoBase()
{
	seckey = ::zpds::utils::B64String::Decode(seckey_);
	scope=ZPDS_CRYPTO_SCOPE_ENCODE_DECODE;
}

/**
* AES128CBC : constructor with scope and 1 key
*
*/
zpds::crypto::AES128CBC::AES128CBC (unsigned int scope_, const std::string& onekey_) : CryptoBase()
{
	seckey = ::zpds::utils::B64String::Decode(onekey_);
	if (scope_ & ZPDS_CRYPTO_SCOPE_ENCODE_DECODE) {
		scope = ZPDS_CRYPTO_SCOPE_ENCODE_DECODE;
	}
	else if (scope_ & ZPDS_CRYPTO_SCOPE_ENCODE) {
		scope = ZPDS_CRYPTO_SCOPE_ENCODE;
	}
	else if (scope_ & ZPDS_CRYPTO_SCOPE_DECODE) {
		scope = ZPDS_CRYPTO_SCOPE_DECODE;
	}
	else {
		throw zpds::BadCodeException("Undefined constructor for aes-128-cbc");
	}
}

/**
* Encode : return encoded
*
*/
bool zpds::crypto::AES128CBC::Encode(const std::string& input, std::string& output) const
{
	if (!(this->scope & ZPDS_CRYPTO_SCOPE_ENCODE))
		throw zpds::BadCodeException("This class cannot encode");

	if (seckey.length()!=CryptoPP::AES::DEFAULT_KEYLENGTH) return false;
	unsigned char iv[CryptoPP::AES::BLOCKSIZE];
	// CryptoPP::AutoSeededRandomPool prng;
	// prng.GenerateBlock(iv, sizeof(iv));
	{
		std::string ivstr;
		ZPDS_RANDOM_STRING(ivstr,CryptoPP::AES::BLOCKSIZE);
		memcpy(iv, ivstr.c_str(), CryptoPP::AES::BLOCKSIZE);
	}
	output.clear();

	try {
		Anchor anchor;

		// Put the IV at the begining of the output ( just copy the string )
		anchor.Attach(new CryptoPP::StringSink(output));
		anchor.Put( iv, sizeof(iv) );
		anchor.MessageEnd();
		anchor.Detach();

		// rest encode base64
		CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption encryptor((unsigned char *)seckey.c_str(), seckey.length(), iv);
		anchor.Attach(new CryptoPP::StreamTransformationFilter(encryptor,NULL, CryptoPP::StreamTransformationFilter::PKCS_PADDING));
		anchor.Attach(new CryptoPP::Base64Encoder(NULL, false));
		anchor.Attach(new CryptoPP::StringSink(output));

		anchor.Put( (CryptoPP::byte*)input.c_str(), input.length() );
		anchor.MessageEnd();
	}
	catch (const CryptoPP::Exception& e) {
		return false;
	}
	return true;
}

/**
* Decode : return decoded
*
*/
bool zpds::crypto::AES128CBC::Decode(const std::string& input, std::string& output) const
{

	if (!(this->scope & ZPDS_CRYPTO_SCOPE_DECODE))
		throw zpds::BadCodeException("This class cannot decode");

	if (seckey.length()!=CryptoPP::AES::DEFAULT_KEYLENGTH) return false;
	if (input.length()<=CryptoPP::AES::BLOCKSIZE) return false; // no iv
	unsigned char iv[CryptoPP::AES::BLOCKSIZE];
	memcpy(iv, input.c_str(), CryptoPP::AES::BLOCKSIZE);

	try {
		// Convert from b64 and decode
		CryptoPP::CBC_Mode<CryptoPP::AES>::Decryption decryptor((unsigned char *)seckey.c_str(), seckey.length(), iv);

		Anchor anchor;
		anchor.Attach(new CryptoPP::Base64Decoder());
		anchor.Attach(new CryptoPP::StreamTransformationFilter( decryptor, NULL));
		anchor.Attach(new CryptoPP::StringSink(output));

		anchor.Put( (const CryptoPP::byte*)&input[CryptoPP::AES::BLOCKSIZE], input.length()-CryptoPP::AES::BLOCKSIZE );
		anchor.MessageEnd();

	}
	catch (const CryptoPP::Exception& e) {
		return false;
	}
	return true;
}
