/**
 * @project zapdos
 * @file include/crypto/CryptoTraits.hpp
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
 *  CryptoTraits.hpp : Pseudo Traits file for all Sign Key Classes
 *
 */
#ifndef _ZPDS_CRYPTO_CRYPTO_TRAITS_HPP_
#define _ZPDS_CRYPTO_CRYPTO_TRAITS_HPP_

#include <memory>
#include "crypto/AES128CBC.hpp"

#define ENCODEKEY_ALGO_DEFAULT ENCODEKEY_ALGO_AES128CBC

namespace zpds {
namespace crypto {
class CryptoTraits {
public:
	using pointer = std::shared_ptr<CryptoBase>;

	/**
	* Create : create new keys
	*
	* @param ctype
	*   const std::string key algo
	*
	* @return
	*   std::shared_ptr<CryptoBase>
	*
	*/
	static pointer Create(const std::string ctype)
	{
		if (ctype==AES128CBC::GetAlgo()) return std::make_shared<AES128CBC>();
		throw ::zpds::BadCodeException("Bogus Algo in Create");
	}

	/**
	* CreateFromSeed : create from seed
	*
	* @param ctype
	*   const std::string key algo
	*
	* @param seed
	*   const std::string& seed
	*
	* @return
	*   std::shared_ptr<CryptoBase>
	*
	*/
	static pointer CreateFromSeed(const std::string ctype, const std::string& seed)
	{
		if (ctype==AES128CBC::GetAlgo()) return std::make_shared<AES128CBC>(seed);
		throw ::zpds::BadCodeException("Bogus Algo in CreateFromSeed");
	}

	/**
	* CreateWithScope : create single purpose key
	*
	* @param ctype
	*   const std::string key algo
	*
	* @param scope
	*   unsigned int scope
	*
	* @param key
	*   const std::string key
	*
	* @return
	*   std::shared_ptr<CryptoBase>
	*
	*/
	static pointer CreateWithScope(const std::string ctype, unsigned int scope, const std::string& key)
	{
		if (ctype==AES128CBC::GetAlgo()) return std::make_shared<AES128CBC>(scope,key);
		throw ::zpds::BadCodeException("Bogus Algo in CreateWithScope");
	}

	/**
	* Create : only operator
	*
	* @param ctype
	*   const std::string key algo
	*
	* @param pkey
	*   const std::string& public key
	*
	* @param skey
	*   const std::string& private key
	*
	* @return
	*   std::shared_ptr<CryptoBase>
	*
	*/
	static pointer Create(const std::string ctype, const std::string& pkey, const std::string& skey)
	{
		if (ctype==AES128CBC::GetAlgo()) return std::make_shared<AES128CBC>(skey);
		throw ::zpds::BadCodeException("Bogus Algo in Create 2 keys");
	}

};
} // namespace crypto
} // namespace zpds
#endif /* _ZPDS_CRYPTO_CRYPTO_TRAITS_HPP_ */
