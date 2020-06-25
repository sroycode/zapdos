/**
 * @project zapdos
 * @file src/crypto/Anchor.hpp
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
 *  Anchor.hpp : Anchor object for CryptoPP
 *
 */
#ifndef _ZPDS_CRYPTO_ANCHOR_HPP_
#define _ZPDS_CRYPTO_ANCHOR_HPP_

#include <cstdlib>
#include <cryptopp/osrng.h>
#include <cryptopp/base64.h>
#include <cryptopp/aes.h>
#include <cryptopp/filters.h>
#include <cryptopp/modes.h>
#include <cryptopp/cryptlib.h>

#ifndef CRYPTOPP_NO_GLOBAL_BYTE
namespace CryptoPP {
typedef unsigned char byte;
}
#endif  // CRYPTOPP_NO_GLOBAL_BYTE

namespace zpds {
namespace crypto {

class Anchor : public CryptoPP::Bufferless<CryptoPP::Filter> {
public:
	Anchor(CryptoPP::BufferedTransformation* attachment = NULL)
	{
		Detach(attachment);
	};
	size_t Put2(const CryptoPP::byte* inString, size_t length, int messageEnd, bool blocking )
	{
		return AttachedTransformation()->Put2(inString, length, messageEnd, blocking );
	}
};

} // namespace crypto
} // namespace zpds

#endif /* _ZPDS_CRYPTO_ANCHOR_HPP_ */
