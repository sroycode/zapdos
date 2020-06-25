/**
 * @project zapdos
 * @file include/crypto/AES128CBC.hpp
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
 *  AES128CBC.hpp : AES128CBC Sign Key Headers, derived from base
 *
 */
#ifndef _ZPDS_CRYPTO_AES128CBC_HPP_
#define _ZPDS_CRYPTO_AES128CBC_HPP_

#define ENCODEKEY_ALGO_AES128CBC "aes-128-cbc"
#include "crypto/CryptoBase.hpp"

namespace zpds {
namespace crypto {
class AES128CBC : virtual public CryptoBase {
public:

	/**
	* GetAlgo : get key algo
	*
	* @return
	*   const std::string algo
	*/
	const static std::string GetAlgo();

	/**
	* AES128CBC : constructor
	*
	*/
	AES128CBC();

	/**
	* AES128CBC : constructor with seed
	*
	* @param seed
	*   std::string& seed
	*
	*/
	explicit AES128CBC(const std::string& seed);

	/**
	* AES128CBC : constructor with 2 keys
	*
	* @param pubkey_
	*   std::string& public key
	*
	* @param seckey_
	*   std::string& secret key
	*
	*/
	explicit AES128CBC(const std::string& pubkey_, const std::string& seckey_);

	/**
	* AES128CBC : constructor with scope and 1 key for verification only
	*
	* @param scope_
	*   unsigned int scope
	*
	* @param onekey_
	*   std::string& verification or decoding key
	*
	*/
	explicit AES128CBC( unsigned int scope_, const std::string& onekey_);


	AES128CBC(const AES128CBC&) = delete;
	AES128CBC& operator=(const AES128CBC&) = delete;

	/**
	* Encode : return encoded
	*
	* @param
	*   const std::string& input
	*
	* @param
	*   std::string& output
	*
	* @return
	*   bool success
	*/
	virtual bool Encode(const std::string& input, std::string& output) const override;

	/**
	* Decode : return decoded
	*
	* @param
	*   const std::string& input
	*
	* @param
	*   std::string& output
	*
	* @return
	*   bool success
	*/
	virtual bool Decode(const std::string& input, std::string& output) const override;

protected:

};
} // namespace crypto
} // namespace zpds
#endif /* _ZPDS_CRYPTO_AES128CBC_HPP_ */
