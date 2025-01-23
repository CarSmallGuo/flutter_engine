/*
Copyright (c) 2023 Huawei Device Co., Ltd.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of pngout nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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

#define CHECK_NULL_PTR_RET_VOID(PARAM, FUNC)                   \
    do {                                                       \
        if (PARAM == nullptr) {                                \
            LOGE("Error: %{public}s -> %{public}s is nullptr", \
                #FUNC, #PARAM);                                \
            return;                                            \
        }                                                      \
    } while (false)                                            \

#endif // FOUNDATION_ACE_INTERFACE_INNERKITS_ACE_UTILS_H
