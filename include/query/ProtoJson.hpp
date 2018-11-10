/**
 * @project zapdos
 * @file include/query/ProtoJson.hpp
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
 *  ProtoJson.hpp :  Headers for Protobuf<->Json implementation (modified from https://github.com/yinqiwen/pbjson )
 *
 */
#ifndef _ZPDS_QUERY_PROTOJSON_HPP_
#define _ZPDS_QUERY_PROTOJSON_HPP_

#include <string>
#include <vector>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>

#include "utils/BaseUtils.hpp"

namespace zpds {
namespace query {

/**
* pb2json : Convert protobuf to json
*
* @param msg
*   const google::protobuf::Message* from msg pointer
*
* @param str
*   std::string& to string
*
* @param blank
*   bool print blank default false
*
* @return
*   none
*/
void pb2json(const google::protobuf::Message* msg, std::string& str, bool blank=false);

/**
* pb2json : Convert protobuf to json
*
* @param msg
*   const google::protobuf::Message* from msg pointer
*
* @param blank
*   bool print blank default false
*
* @return
*   std::string
*/
std::string pb2json(const google::protobuf::Message* msg, bool blank=false);

/**
* json2pb : Convert json to protobuf
*
* @param json
*   std::string& from string
*
* @param msg
*   google::protobuf::Message* to msg pointer
*
* @return
*   bool status
*/
bool json2pb(const std::string& json, google::protobuf::Message* msg);

/**
* err2json : Generate a Json for Error Message
*
* @param errorcode
*   const int errorcode
*
* @param error
*   std::string error string
*
* @return
*   str::string
*/
std::string err2json(const int errorcode, const std::string error);

}
}

#endif // _ZPDS_QUERY_PROTOJSON_HPP_ 
