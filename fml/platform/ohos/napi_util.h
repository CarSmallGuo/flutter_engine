/*
 * Copyright (c) 2023 Hunan OpenValley Digital Industry Development Co., Ltd. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE_KHZG file.
 */

#ifndef OHOS_FLUTTER_NAPI_UTIL_H
#define OHOS_FLUTTER_NAPI_UTIL_H
#include <string.h>
#include <uv.h>
#include <string>
#include <vector>
#include "flutter/fml/logging.h"
#include "napi/native_api.h"

namespace fml {
namespace napi {
enum {
  SUCCESS = 0,
  ERROR_TYPE = -100,
  ERROR_NULL,
};

int32_t GetString(napi_env env, napi_value arg, std::string& strValue);

/**
 *  打印napi_value @cur 的类型信息
 */
void NapiPrintValueType(napi_env env, napi_value cur);

/**
 * 判断napi value  类型
 */
bool NapiIsType(napi_env env, napi_value value, napi_valuetype type);
/**

}  // namespace fml
#endif
