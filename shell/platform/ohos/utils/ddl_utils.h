/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef DDL_UTILS_H
#define DDL_UTILS_H

#include <dlfcn.h>
using LIBHANDLE = void*;
#define LOAD_LIB(libPath) dlopen(libPath, RTLD_LAZY|RTLD_LOCAL)
#define CLOSE_LIB(libHandle) dlclose(libHandle)
#define LOAD_SYM(libHandle, symbol) dlsym(libHandle, symbol)
#define LOAD_ERROR() dlerror()

#define ARKUI_SUCCEED_CODE 0
#define ARKUI_FAILED_CODE -1
#define ARKUI_BAD_PARAM_CODE -2
#define ARKUI_OOM_CODE  -3

#define ARKUI_ACCESSIBILITY_CALL_CHECK(X)                                 \
    do {                                                                  \
        int32_t RET = X;                                                  \
        if (RET != ARKUI_SUCCEED_CODE) {                                  \
            LOGE("Failed function %{public}s call, error code:%{public}d",\
                #X, RET);                                                 \
        }                                                                 \
    }  while (false)                                                      \

#define CHECK_DLL_NULL_PTR(func)                                      \
    do {                                                              \
        if (func == nullptr) {                                        \
            LOGE("Error: Function %{public}s is nullptr, %{public}s", \
                #func, LOAD_ERROR());                                 \
        }                                                             \
    } while (false)                                                   \

#define CHECK_NULL_PTR(PARAM, FUNC)                            \
    do {                                                       \
        if (PARAM == nullptr) {                                \
            LOGE("Error: %{public}s -> %{public}s is nullptr", \
                #FUNC, #PARAM);                                \
        }                                                      \
    } while (false)                                            \

#define CHECK_NULL_PTR_WITH_RET(PARAM, FUNC)                   \
    do {                                                       \
        if (PARAM == nullptr) {                                \
            LOGE("Error: %{public}s -> %{public}s is nullptr", \
                #FUNC, #PARAM);                                \
            return ARKUI_FAILED_CODE;                          \
        }                                                      \
    } while (false)                                            \

#endif // FOUNDATION_ACE_INTERFACE_INNERKITS_ACE_UTILS_H
