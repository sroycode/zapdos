/**
 * @project zapdos
 * @file include/utils/BaseUtils.hpp
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
 *  BaseUtils.hpp :   For Standard Inclusion
 *
 */
#ifndef _ZPDS_UTILS_BASEUTILS_HPP_
#define _ZPDS_UTILS_BASEUTILS_HPP_

#include <glog/logging.h>
#include <string>
#include <memory>
#include <exception>
#include <chrono>
#define ZPDS_CURRTIME_MS std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()
#define ZPDS_CURRTIME_MUS std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count()

#define ZPDS_M_ERROR_MIN            101
#define M_UNKNOWN                   101
#define M_BAD_JSON                  102
#define M_INVALID_PARAM             104
#define M_MISSING_PARAMS            105
#define M_NOT_JSON                  106
#define M_BAD_BACKEND               107
#define ZPDS_M_ERROR_MAX            107

#define ZPDS_SERVICE_SCOPE_HTTP  0x0001
#define ZPDS_SERVICE_SCOPE_HTTPS 0x0002

#define ZPDS_DEFAULT_GEOHASH_PRECISION 5
#define ZPDS_PERM_GEOHASH_PRECISION 5
#define ZPDS_TEMP_GEOHASH_PRECISION 7

#define ZPDS_MAX_RULE_WEIGHT 1000

#define ZPDS_TEMP_SYSTEM_MAXSPEED_KM 500.0

#define ZPDS_UPLOAD_MAX_RECORDS 10000
#define ZPDS_DEFAULT_LANG  "en"

namespace zpds {
class BaseException : public std::exception {
protected:
	std::string msg_;
	int err_;
public:
	explicit BaseException() : msg_(""), err_(0) {}
	explicit BaseException(const char* msg) : msg_(msg), err_(0) {}
	explicit BaseException(std::string& msg) : msg_(msg), err_(0) {}
	explicit BaseException(std::string msg) : msg_(msg), err_(0) {}
	explicit BaseException(const char* msg, int err) : msg_(msg),err_(err) {}
	explicit BaseException(std::string& msg, int err) : msg_(msg),err_(err) {}
	explicit BaseException(std::string msg, int err) : msg_(msg),err_(err) {}
	virtual ~BaseException() throw () {}
	virtual const char* what() const throw ()
	{
		return msg_.c_str();
	}
	virtual int ecode() const throw ()
	{
		return err_;
	}
};

class InitialException : public BaseException {
	using BaseException::BaseException;
};
class ConfigException : public BaseException {
	using BaseException::BaseException;
};
class NotFoundException : public BaseException {
	using BaseException::BaseException;
};
class BadQueryException : public BaseException {
	using BaseException::BaseException;
};
class BadDataException : public BaseException {
	using BaseException::BaseException;
};
class BadJsonException : public BaseException {
	using BaseException::BaseException;
};
class BadCodeException : public BaseException {
	using BaseException::BaseException;
};
class LocalException : public BaseException {
	using BaseException::BaseException;
};

#if __GNUC__ >= 4
	#define ZPDS_EXPORT_PUBLIC __attribute__ ((visibility ("default")))
	#define ZPDS_EXPORT_LOCAL  __attribute__ ((visibility ("hidden")))
#else
	#define ZPDS_EXPORT_PUBLIC
	#define ZPDS_EXPORT_LOCAL
#endif

} // namespace zpds
#endif // _ZPDS_UTILS_BASEUTILS_HPP_
