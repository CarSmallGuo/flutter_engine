// Copyright (C) 2024 Huawei Device Co., Ltd.
	
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions 
// are met:
	
// 1. Redistributions of source code must retain the above copyright 
// notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright 
// notice, this list of conditions and the following disclaimer in 
// the documentation and/or other materials provided with the distribution.
// 3. Neither the name of the copyright holder nor the names of 
// its contributors may be used to endorse or promote products 
// derived from this software without specific prior written permission.
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 

// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
// TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
// OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
// OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

//
// Created on 2023/6/17.
//
// Node APIs are not fully supported. To solve the compilation error of the
// interface cannot be found, please include "napi/native_api.h".

#ifndef XComponentDemo_native_common_H
#define XComponentDemo_native_common_H

#define NAPI_RETVAL_NOTHING

#define GET_AND_THROW_LAST_ERROR(env)                                \
  do {                                                               \
    const napi_extended_error_info* errorInfo = nullptr;             \
    napi_get_last_error_info((env), &errorInfo);                     \
    bool isPending = false;                                          \
    napi_is_exception_pending((env), &isPending);                    \
    if (!isPending && errorInfo != nullptr) {                        \
      const char* errorMessage = errorInfo->error_message != nullptr \
                                     ? errorInfo->error_message      \
                                     : "empty error message";        \
      napi_throw_error((env), nullptr, errorMessage);                \
    }                                                                \
  } while (0)

#define NAPI_ASSERT_BASE(env, assertion, message, retVal)              \
  do {                                                                 \
    if (!(assertion)) {                                                \
      napi_throw_error((env), nullptr,                                 \
                       "assertion (" #assertion ") failed: " message); \
      return retVal;                                                   \
    }                                                                  \
  } while (0)

#define NAPI_ASSERT(env, assertion, message) \
  NAPI_ASSERT_BASE(env, assertion, message, nullptr)

#define NAPI_ASSERT_RETURN_VOID(env, assertion, message) \
  NAPI_ASSERT_BASE(env, assertion, message, NAPI_RETVAL_NOTHING)

#define NAPI_CALL_BASE(env, theCall, retVal) \
  do {                                       \
    if ((theCall) != napi_ok) {              \
      GET_AND_THROW_LAST_ERROR((env));       \
      return retVal;                         \
    }                                        \
  } while (0)

#define NAPI_CALL(env, theCall) NAPI_CALL_BASE(env, theCall, nullptr)

#define NAPI_CALL_RETURN_VOID(env, theCall) \
  NAPI_CALL_BASE(env, theCall, NAPI_RETVAL_NOTHING)

#define DECLARE_NAPI_PROPERTY(name, val) \
  { (name), nullptr, nullptr, nullptr, nullptr, val, napi_default, nullptr }

#define DECLARE_NAPI_STATIC_PROPERTY(name, val) \
  { (name), nullptr, nullptr, nullptr, nullptr, val, napi_static, nullptr }

#define DECLARE_NAPI_FUNCTION(name, func) \
  { (name), nullptr, (func), nullptr, nullptr, nullptr, napi_default, nullptr }

#define DECLARE_NAPI_FUNCTION_WITH_DATA(name, func, data) \
  { (name), nullptr, (func), nullptr, nullptr, nullptr, napi_default, data }

#define DECLARE_NAPI_STATIC_FUNCTION(name, func) \
  { (name), nullptr, (func), nullptr, nullptr, nullptr, napi_static, nullptr }

#define DECLARE_NAPI_GETTER(name, getter)                               \
  {                                                                     \
    (name), nullptr, nullptr, (getter), nullptr, nullptr, napi_default, \
        nullptr                                                         \
  }

#define DECLARE_NAPI_SETTER(name, setter)                               \
  {                                                                     \
    (name), nullptr, nullptr, nullptr, (setter), nullptr, napi_default, \
        nullptr                                                         \
  }

#define DECLARE_NAPI_GETTER_SETTER(name, getter, setter)                 \
  {                                                                      \
    (name), nullptr, nullptr, (getter), (setter), nullptr, napi_default, \
        nullptr                                                          \
  }

#endif  // XComponentDemo_native_common_H
