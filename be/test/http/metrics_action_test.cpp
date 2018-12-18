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

#include "http/action/metrics_action.h"

#include <gtest/gtest.h>

#include "http/http_request.h"
#include "http/http_response.h"
#include "http/http_channel.h"
#include "util/metrics.h"

namespace doris {

// Mock part
const char* s_expect_response = nullptr;

void HttpChannel::send_reply(
        HttpRequest* request, HttpStatus status, const std::string& content) {
    ASSERT_STREQ(s_expect_response, content.c_str());
}

class MetricsActionTest : public testing::Test {
public:
    MetricsActionTest() { }
    virtual ~MetricsActionTest() {
    }
    void SetUp() override {
        _evhttp_req = evhttp_request_new(nullptr, nullptr);
    }
    void TearDown() override {
        if (_evhttp_req != nullptr) {
            evhttp_request_free(_evhttp_req);
        }
    }
private:
    evhttp_request* _evhttp_req = nullptr;
};

TEST_F(MetricsActionTest, prometheus_output) {
    MetricRegistry registry("test");
    IntGauge cpu_idle;
    cpu_idle.set_value(50);
    registry.register_metric("cpu_idle", &cpu_idle);
    IntCounter put_requests_total;
    put_requests_total.increment(2345);
    registry.register_metric("requests_total",
                             MetricLabels().add("type", "put").add("path", "/sports"),
                             &put_requests_total);
    s_expect_response =
        "# TYPE test_cpu_idle gauge\n"
        "test_cpu_idle 50\n"
        "# TYPE test_requests_total counter\n"
        "test_requests_total{path=\"/sports\",type=\"put\"} 2345\n";
    HttpRequest request(_evhttp_req);
    MetricsAction action(&registry);
    action.handle(&request);
}

TEST_F(MetricsActionTest, prometheus_no_prefix) {
    MetricRegistry registry("");
    IntGauge cpu_idle;
    cpu_idle.set_value(50);
    registry.register_metric("cpu_idle", &cpu_idle);
    s_expect_response =
        "# TYPE cpu_idle gauge\n"
        "cpu_idle 50\n";
    HttpRequest request(_evhttp_req);
    MetricsAction action(&registry);
    action.handle(&request);
}

TEST_F(MetricsActionTest, prometheus_no_name) {
    MetricRegistry registry("test");
    IntGauge cpu_idle;
    cpu_idle.set_value(50);
    registry.register_metric("", &cpu_idle);
    s_expect_response = "";
    HttpRequest request(_evhttp_req);
    MetricsAction action(&registry);
    action.handle(&request);
}

}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
