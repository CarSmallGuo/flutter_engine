/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "flutter/fml/platform/ohos/napi_util.h"
#include "gtest/gtest.h"

namespace fml {
namespace {
TEST(NapiUtilTest, NapiUtil01) {
    napi_env env = nullptr;
    napi_value value = nullptr;
    EXPECT_EQ(napi::NapiIsNull(env, value), true);
}

TEST(NapiUtilTest, NapiUtil02) {
    napi_env env = nullptr;
    napi_value value = nullptr;
    std::string str = "test";
    EXPECT_EQ(napi::GetString(env, value, str), napi::ERROR_TYPE);
}

TEST(NapiUtilTest, NapiUtil03) {
    napi_env env = nullptr;
    napi_value value = nullptr;
    std::vector<std::string> vectorStr = {};
    EXPECT_NE(napi::GetArrayString(env, value, vectorStr), napi::SUCCESS);
}

TEST(NapiUtilTest, NapiUtil04) {
    napi_env env = nullptr;
    napi_value value = nullptr;
    void** message = nullptr;
    size_t lenth = 0;
    EXPECT_NE(napi::GetArrayBuffer(env, value, message, &lenth), napi::SUCCESS);
}

TEST(NapiUtilTest, NapiUtil05) {
    napi_env env = nullptr;
    napi_value value = nullptr;
    EXPECT_EQ(napi::NapiIsType(env, value, napi_number), false);
}

TEST(NapiUtilTest, NapiUtil06) {
    napi_env env = nullptr;
    napi_ref ref_napi_obj = nullptr;
    EXPECT_GT(napi::InvokeJsMethod(env, ref_napi_obj, nullptr, 0, nullptr), 0);
}

}  // namespace
}  // namespace fml