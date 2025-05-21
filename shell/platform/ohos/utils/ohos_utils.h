/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE_HW file.
 */

#ifndef OHOS_UTILS_H
#define OHOS_UTILS_H
#include <string>
#include <map>
#include <string>
#include <vector>
#include <cstring>
#include "flutter/shell/platform/ohos/ohos_logging.h"
#include <dlfcn.h>

using LIBHANDLE = void*;
#define LOAD_LIB(libPath) dlopen(libPath, RTLD_LAZY|RTLD_LOCAL)
#define CLOSE_LIB(libHandle) dlclose(libHandle)
#define LOAD_SYM(libHandle, symbol) dlsym(libHandle, symbol)
#define LOAD_ERROR() dlerror()

#define ARKUI_SUCCEED_CODE 0
#define ARKUI_FAILED_CODE (-1)
#define ARKUI_BAD_PARAM_CODE (-2)
#define ARKUI_OOM_CODE  (-3)

#define CHECK_WITH_RET_NULLPTR(PARAM, FUNC)                    \
    do {                                                       \
        if ((PARAM) == nullptr) {                              \
            LOGE("Error: %{public}s -> %{public}s is nullptr", \
                #FUNC, #PARAM);                                \
            return nullptr;                                    \
        }                                                      \
    } while (false)                                            \

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
        if ((func) == nullptr) {                                      \
            LOGE("Error: Function %{public}s is nullptr, %{public}s", \
                #func, LOAD_ERROR());                                 \
        }                                                             \
    } while (false)                                                   \

#define CHECK_NULL_PTR(PARAM, FUNC)                            \
    do {                                                       \
        if ((PARAM) == nullptr) {                              \
            LOGE("Error: %{public}s -> %{public}s is nullptr", \
                #FUNC, #PARAM);                                \
        }                                                      \
    } while (false)                                            \

#define CHECK_NULL_PTR_WITH_RET(PARAM, FUNC)                   \
    do {                                                       \
        if ((PARAM) == nullptr) {                              \
            LOGE("Error: %{public}s -> %{public}s is nullptr", \
                #FUNC, #PARAM);                                \
            return ARKUI_FAILED_CODE;                          \
        }                                                      \
    } while (false)                                            \

#define CHECK_NULL_PTR_RET_VOID(PARAM, FUNC)                   \
    do {                                                       \
        if ((PARAM) == nullptr) {                              \
            LOGE("Error: %{public}s -> %{public}s is nullptr", \
                #FUNC, #PARAM);                                \
            return;                                            \
        }                                                      \
    } while (false)                                            \

namespace flutter {

class OHOSUtils {
public:
    OHOSUtils();
    ~OHOSUtils();
    
    static bool Contains(const std::string source, const std::string target);
    static void SerializeString(const std::string& str, std::vector<uint8_t>& buffer);
    static std::vector<uint8_t> SerializeStringIntMap(const std::map<std::string, int32_t>& mp);
    static void CharArrayToInt32(const char* str, int32_t& target);
};

}
#endif
