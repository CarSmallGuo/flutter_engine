/*
 * Copyright (c) 2023 Hunan OpenValley Digital Industry Development Co., Ltd. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE_KHZG file.
 */

#include "flutter/fml/platform/ohos/napi_util.h"
#include <node_api.h>
#include <cstdarg>
#include <string>

namespace fml {
namespace napi {

std::string NapiGetLastError(napi_env env, napi_status status) {
  std::string ret("Napi Error:");
  ret += status;

  const napi_extended_error_info* error_info;
  napi_get_last_error_info(env, &error_info);
  ret += error_info->error_message;
  return ret;
}
bool NapiIsType(napi_env env, napi_value value, napi_valuetype type) {
  napi_status status;
  napi_valuetype argType;
  status = napi_typeof(env, value, &argType);
  return status == napi_ok && type == argType;
}

bool IsArrayBuffer(napi_env env, napi_value value) {
  napi_status status;
  bool result;

  // Check if the value is an ArrayBuffer
  status = napi_is_arraybuffer(env, value, &result);
  if (status != napi_ok) {
    FML_DLOG(INFO) << "napi_is_arraybuffer: failed:" << status;
    return false;
  }

  // If it's not an ArrayBuffer, check if it's an Array
  if (!result) {
    status = napi_is_array(env, value, &result);
    if (status != napi_ok) {
      FML_DLOG(INFO) << "napi_is_array: failed:"
                     << NapiGetLastError(env, status);
      return false;
    }
  }
  return true;
}

void NapiPrintValueType(napi_env env, napi_value cur) {
  napi_status status;
  napi_valuetype argType;
  int i = 0;
  status = napi_typeof(env, cur, &argType);
  if (status != napi_ok) {
    FML_DLOG(INFO) << "args,gettype: failed:" << status;
    return;
  }
  if (argType == napi_number) {
    FML_DLOG(INFO) << "args,type: " << argType << " number";
  } else if (argType == napi_string) {
    FML_DLOG(INFO) << "args[" << i << "],type: " << argType << " napi_string";
  } else if (argType == napi_boolean) {
    FML_DLOG(INFO) << "args[" << i << "],type: " << argType << " napi_boolean ";
  } else if (argType == napi_object) {
    FML_DLOG(INFO) << "args[" << i << "],type: " << argType << " napi_object";
  } else if (argType == napi_function) {
    FML_DLOG(INFO) << "args[" << i << "],type: " << argType << " napi_function";
  } else if (argType == napi_null) {
    FML_DLOG(INFO) << "args[" << i << "],type: " << argType << " napi_null ";
  } else if (argType == napi_symbol) {
    FML_DLOG(INFO) << "args[" << i << "],type: " << argType << " napi_symbol";
  } else if (argType == napi_external) {
    FML_DLOG(INFO) << "args[" << i << "],type: " << argType << " napi_external";
  } else if (argType == napi_bigint) {
    FML_DLOG(INFO) << "args[" << i << "],type: " << argType << " napi_bigint";
  } else {
    FML_DLOG(INFO) << "args[" << i << "],type: " << argType << " number";
  }
  bool ok = false;
  napi_is_array(env, cur, &ok);
  if (ok) {
    FML_DLOG(INFO) << "args[" << i << "],type: " << argType << " is_array ";
  }
  if (IsArrayBuffer(env, cur)) {
    FML_DLOG(INFO) << "args[" << i << "],type: " << argType
                   << " is_arraybuffer ";
  }
  napi_is_typedarray(env, cur, &ok);
  if (ok) {
    FML_DLOG(INFO) << "args[" << i << "],type: " << argType << " is_typearray";
  }
}

int32_t GetString(napi_env env, napi_value arg, std::string& strValue) {
  napi_status status;

  if (NapiIsType(env, arg, napi_null)) {
    FML_DLOG(INFO) << "napi_null";
    strValue = "";
    return SUCCESS;
  }
  if (!NapiIsType(env, arg, napi_string)) {
    FML_DLOG(ERROR) << "Invalid type:";
    return ERROR_TYPE;
  }

  // 获取字符串长度
  size_t str_len;
  status = napi_get_value_string_utf8(env, arg, nullptr, 0, &str_len);
  if (status != napi_ok) {
    FML_DLOG(ERROR) << "Error get str_len:" << status;
    FML_DLOG(ERROR) << "result str_len:" << str_len;
    return status;
  }

  // 读取字符串
  size_t copy_lenth = str_len + 1;
  std::vector<char> buff(copy_lenth);
  status = napi_get_value_string_utf8(env, arg, static_cast<char*>(buff.data()), copy_lenth, &copy_lenth);
  if (status != napi_ok) {
    FML_DLOG(ERROR) << "Error get string:" << status;
    FML_DLOG(ERROR) << "result size:" << copy_lenth;
    return status;
  }
  strValue.assign(buff.data(), copy_lenth);
  return SUCCESS;
}

}  // end namespace napi
}  // end namespace fml
