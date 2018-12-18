// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.

#pragma once

#include <string>

#include "http/http_common.h"

namespace doris {

class HttpRequest;
class HttpAuthInfo;

std::string encode_basic_auth(const std::string& user, const std::string& passwd);
// parse Basic authorization
// return true, if request contain valid basic authorization.
// Otherwise return fasle
bool parse_basic_auth(const HttpRequest& req, std::string* user, std::string* passwd);

bool parse_basic_auth(const HttpRequest& req, HttpAuthInfo* auth);

template<class T>
void set_http_auth(T* req, const HttpAuthInfo& auth) {
    req->user = auth.user;
    req->passwd = auth.passwd;
    if (!auth.cluster.empty()) {
        req->__set_cluster(auth.cluster);
    }
    req->__set_user_ip(auth.user_ip);
}

}
