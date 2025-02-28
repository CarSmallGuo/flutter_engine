/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE_HW file.
 */
#include "ohos_accessibility_ddl.h"

namespace flutter {

OhosAccessibilityDDL::OhosAccessibilityDDL() = default;
OhosAccessibilityDDL::~OhosAccessibilityDDL() = default;

RegisterFunc OhosAccessibilityDDL::DLLoadRegisterFunc(const char* symbolName)
{
    LIBHANDLE handler = LOAD_LIB(ACCESSIBILITY_LIB_NAME);
    if (handler == nullptr) {
        return nullptr;
    }

    auto symbol = reinterpret_cast<RegisterFunc>(LOAD_SYM(handler, symbolName));
    if (symbol == nullptr) {
        CLOSE_LIB(handler);
        return nullptr;
    }
    return symbol;
}

SendAsyncEventFunc OhosAccessibilityDDL::DLLoadSendAsyncEventFunc(const char* symbolName)
{
    LIBHANDLE handler = LOAD_LIB(ACCESSIBILITY_LIB_NAME);
    if (handler == nullptr) {
        return nullptr;
    }

    auto symbol = reinterpret_cast<SendAsyncEventFunc>(LOAD_SYM(handler, symbolName));
    if (symbol == nullptr) {
        CLOSE_LIB(handler);
        return nullptr;
    }
    return symbol;
}

GetElemFunc OhosAccessibilityDDL::DLLoadGetElemFunc(const char* symbolName)
{
    LIBHANDLE handler = LOAD_LIB(ACCESSIBILITY_LIB_NAME);
    if (handler == nullptr) {
        return nullptr;
    }

    auto symbol = reinterpret_cast<GetElemFunc>(LOAD_SYM(handler, symbolName));
    if (symbol == nullptr) {
        CLOSE_LIB(handler);
        return nullptr;
    }
    return symbol;
}

CreateElemInfoFunc OhosAccessibilityDDL::DLLoadCreateElemInfoFunc(const char* symbolName)
{
    LIBHANDLE handler = LOAD_LIB(ACCESSIBILITY_LIB_NAME);
    if (handler == nullptr) {
        return nullptr;
    }

    auto symbol = reinterpret_cast<CreateElemInfoFunc>(LOAD_SYM(handler, symbolName));
    if (symbol == nullptr) {
        CLOSE_LIB(handler);
        return nullptr;
    }
    return symbol;
}

CreateEventInfoFunc OhosAccessibilityDDL::DLLoadCreateEventInfoFunc(const char* symbolName)
{
    LIBHANDLE handler = LOAD_LIB(ACCESSIBILITY_LIB_NAME);
    if (handler == nullptr) {
        return nullptr;
    }

    auto symbol = reinterpret_cast<CreateEventInfoFunc>(LOAD_SYM(handler, symbolName));
    if (symbol == nullptr) {
        CLOSE_LIB(handler);
        return nullptr;
    }
    return symbol;
}

DestroyElemFunc OhosAccessibilityDDL::DLLoadDestroyElemFunc(const char* symbolName)
{
    LIBHANDLE handler = LOAD_LIB(ACCESSIBILITY_LIB_NAME);
    if (handler == nullptr) {
        return nullptr;
    }

    auto symbol = reinterpret_cast<DestroyElemFunc>(LOAD_SYM(handler, symbolName));
    if (symbol == nullptr) {
        CLOSE_LIB(handler);
        return nullptr;
    }
    return symbol;
}

DestroyEventFunc OhosAccessibilityDDL::DLLoadDestroyEventFunc(const char* symbolName)
{
    LIBHANDLE handler = LOAD_LIB(ACCESSIBILITY_LIB_NAME);
    if (handler == nullptr) {
        return nullptr;
    }

    auto symbol = reinterpret_cast<DestroyEventFunc>(LOAD_SYM(handler, symbolName));
    if (symbol == nullptr) {
        CLOSE_LIB(handler);
        return nullptr;
    }
    return symbol;
}


SetElemIntFunc OhosAccessibilityDDL::DLLoadSetElemIntFunc(const char* symbolName)
{
    LIBHANDLE handler = LOAD_LIB(ACCESSIBILITY_LIB_NAME);
    if (handler == nullptr) {
        return nullptr;
    }

    auto symbol = reinterpret_cast<SetElemIntFunc>(LOAD_SYM(handler, symbolName));
    if (symbol == nullptr) {
        CLOSE_LIB(handler);
        return nullptr;
    }
    return symbol;
}
SetElemStringFunc OhosAccessibilityDDL::DLLoadSetElemStringFunc(const char* symbolName)
{
    LIBHANDLE handler = LOAD_LIB(ACCESSIBILITY_LIB_NAME);
    if (handler == nullptr) {
        return nullptr;
    }

    auto symbol = reinterpret_cast<SetElemStringFunc>(LOAD_SYM(handler, symbolName));
    if (symbol == nullptr) {
        CLOSE_LIB(handler);
        return nullptr;
    }
    return symbol;
}
SetElemBoolFunc OhosAccessibilityDDL::DLLoadSetElemBoolFunc(const char* symbolName)
{
    LIBHANDLE handler = LOAD_LIB(ACCESSIBILITY_LIB_NAME);
    if (handler == nullptr) {
        return nullptr;
    }

    auto symbol = reinterpret_cast<SetElemBoolFunc>(LOAD_SYM(handler, symbolName));
    if (symbol == nullptr) {
        CLOSE_LIB(handler);
        return nullptr;
    }
    return symbol;
}
SetElemChildFunc OhosAccessibilityDDL::DLLoadSetElemChildFunc(const char* symbolName)
{
    LIBHANDLE handler = LOAD_LIB(ACCESSIBILITY_LIB_NAME);
    if (handler == nullptr) {
        return nullptr;
    }

    auto symbol = reinterpret_cast<SetElemChildFunc>(LOAD_SYM(handler, symbolName));
    if (symbol == nullptr) {
        CLOSE_LIB(handler);
        return nullptr;
    }
    return symbol;
}


SetElemOperActionsFunc OhosAccessibilityDDL::DLLoadSetElemOperActionsFunc(const char* symbolName)
{
    LIBHANDLE handler = LOAD_LIB(ACCESSIBILITY_LIB_NAME);
    if (handler == nullptr) {
        return nullptr;
    }

    auto symbol = reinterpret_cast<SetElemOperActionsFunc>(LOAD_SYM(handler, symbolName));
    if (symbol == nullptr) {
        CLOSE_LIB(handler);
        return nullptr;
    }
    return symbol;
}
SetElemSreenRectFunc OhosAccessibilityDDL::DLLoadSetElemSreenRectFunc(const char* symbolName)
{
    LIBHANDLE handler = LOAD_LIB(ACCESSIBILITY_LIB_NAME);
    if (handler == nullptr) {
        return nullptr;
    }

    auto symbol = reinterpret_cast<SetElemSreenRectFunc>(LOAD_SYM(handler, symbolName));
    if (symbol == nullptr) {
        CLOSE_LIB(handler);
        return nullptr;
    }
    return symbol;
}
SetEventFunc OhosAccessibilityDDL::DLLoadSetEventFunc(const char* symbolName)
{
    LIBHANDLE handler = LOAD_LIB(ACCESSIBILITY_LIB_NAME);
    if (handler == nullptr) {
        return nullptr;
    }

    auto symbol = reinterpret_cast<SetEventFunc>(LOAD_SYM(handler, symbolName));
    if (symbol == nullptr) {
        CLOSE_LIB(handler);
        return nullptr;
    }
    return symbol;
}
SetEventElemFunc OhosAccessibilityDDL::DLLoadSetEventElemFunc(const char* symbolName)
{
    LIBHANDLE handler = LOAD_LIB(ACCESSIBILITY_LIB_NAME);
    if (handler == nullptr) {
        return nullptr;
    }

    auto symbol = reinterpret_cast<SetEventElemFunc>(LOAD_SYM(handler, symbolName));
    if (symbol == nullptr) {
        CLOSE_LIB(handler);
        return nullptr;
    }
    return symbol;
}

SetReqFocusFunc OhosAccessibilityDDL::DLLoadSetReqFocusFunc(const char* symbolName)
{
    LIBHANDLE handler = LOAD_LIB(ACCESSIBILITY_LIB_NAME);
    if (handler == nullptr) {
        return nullptr;
    }

    auto symbol = reinterpret_cast<SetReqFocusFunc>(LOAD_SYM(handler, symbolName));
    if (symbol == nullptr) {
        CLOSE_LIB(handler);
        return nullptr;
    }
    return symbol;
}

SetEventStringFunc OhosAccessibilityDDL::DLLoadSetEventStringFunc(const char* symbolName)
{
    LIBHANDLE handler = LOAD_LIB(ACCESSIBILITY_LIB_NAME);
    if (handler == nullptr) {
        return nullptr;
    }

    auto symbol = reinterpret_cast<SetEventStringFunc>(LOAD_SYM(handler, symbolName));
    if (symbol == nullptr) {
        CLOSE_LIB(handler);
        return nullptr;
    }
    return symbol;
}

GetNativeA11yProvider OhosAccessibilityDDL::DLLoadGetNativeA11yProvider(const char* symbolName)
{
    LIBHANDLE handler = LOAD_LIB(ACCESSIBILITY_LIB_NAME);
    if (handler == nullptr) {
        return nullptr;
    }

    auto symbol = reinterpret_cast<GetNativeA11yProvider>(LOAD_SYM(handler, symbolName));
    if (symbol == nullptr) {
        CLOSE_LIB(handler);
        return nullptr;
    }
    return symbol;
}

GetFindActionArgs OhosAccessibilityDDL::DLLoadGetFindActionArgs(const char* symbolName)
{
    LIBHANDLE handler = LOAD_LIB(ACCESSIBILITY_LIB_NAME);
    if (handler == nullptr) {
        return nullptr;
    }

    auto symbol = reinterpret_cast<GetFindActionArgs>(LOAD_SYM(handler, symbolName));
    if (symbol == nullptr) {
        CLOSE_LIB(handler);
        return nullptr;
    }
    return symbol;
}
}