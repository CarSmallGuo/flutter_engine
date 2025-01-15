/*
* Copyright (c) 2023 Hunan OpenValley Digital Industry Development Co., Ltd. All rights reserved.
* Use of this source code is governed by a BSD-style license that can be
* found in the LICENSE_KHZG file.
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