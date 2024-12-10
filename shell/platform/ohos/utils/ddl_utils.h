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

#endif // FOUNDATION_ACE_INTERFACE_INNERKITS_ACE_UTILS_H
