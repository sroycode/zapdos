/**
 * @project zapdos
 * @file src/query/ProtoJson.cc
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
 *  ProtoJson.cc : Protobuf to Json implementation (modified from https://github.com/yinqiwen/pbjson )
 *
 */
#include "query/ProtoJson.hpp"
#include <google/protobuf/util/json_util.h>
#include <sstream>

/**
* pb2json : Convert protobuf to json
*
*/
void zpds::query::pb2json(const google::protobuf::Message* msg, std::string& str, bool blank)
{
	google::protobuf::util::JsonPrintOptions options;
	options.add_whitespace = false;
	options.preserve_proto_field_names = true;
	options.always_print_primitive_fields = blank;
	if (google::protobuf::util::MessageToJsonString(*msg, &str, options) != google::protobuf::util::Status::OK)
		throw zpds::BadDataException("Broken Pb2Json");
}

/**
* pb2json : Convert protobuf to json
*
*/
std::string zpds::query::pb2json(const google::protobuf::Message* msg, bool blank)
{
	std::string str;
	google::protobuf::util::JsonPrintOptions options;
	options.add_whitespace = false;
	options.preserve_proto_field_names = true;
	options.always_print_primitive_fields = blank;
	if (google::protobuf::util::MessageToJsonString(*msg, &str, options) != google::protobuf::util::Status::OK)
		throw zpds::BadDataException("Broken Pb2Json");
	return str;
}

/**
* json2pb : Convert json to protobuf
*
*/
bool zpds::query::json2pb(const std::string& json, google::protobuf::Message* msg)
{
	google::protobuf::util::JsonParseOptions options;
	options.ignore_unknown_fields=false;
	return (google::protobuf::util::JsonStringToMessage(json, msg, options) == google::protobuf::util::Status::OK);
}

/**
* err2json : Generate a Json for Error Message
*
*/
std::string zpds::query::err2json(const int errorcode, const std::string error)
{
	std::stringstream ss;
	ss << "{\"errorcode\":" << errorcode << ",\"error\":\"" << error << "\"}";
	return ss.str();
}

