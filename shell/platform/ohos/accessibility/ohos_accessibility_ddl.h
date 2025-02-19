/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE_HW file.
 */
#ifndef OHOS_ACCESSIBILITY_DDL_H
#define OHOS_ACCESSIBILITY_DDL_H
#include <arkui/native_interface_accessibility.h>
#include <ace/xcomponent/native_interface_xcomponent.h>
#include "flutter/shell/platform/ohos/utils/ddl_utils.h"

namespace flutter {

using RegisterFunc = int32_t (*)(ArkUI_AccessibilityProvider*, ArkUI_AccessibilityProviderCallbacks*);
using RegisterWithInstanceFunc = int32_t (*)(const char*, ArkUI_AccessibilityProvider*, ArkUI_AccessibilityProviderCallbacksWithInstance*);
using SendAsyncEventFunc = void (*)(ArkUI_AccessibilityProvider*, ArkUI_AccessibilityEventInfo*, void (*)(int32_t));
using GetElemFunc = ArkUI_AccessibilityElementInfo* (*)(ArkUI_AccessibilityElementInfoList*);

using CreateElemInfoFunc = ArkUI_AccessibilityElementInfo* (*)(void);
using CreateEventInfoFunc = ArkUI_AccessibilityEventInfo* (*)(void);
using DestroyElemFunc = void (*)(ArkUI_AccessibilityElementInfo*);
using DestroyEventFunc = void (*)(ArkUI_AccessibilityEventInfo*);

using SetElemIntFunc = int32_t (*)(ArkUI_AccessibilityElementInfo*, int32_t);
using SetElemStringFunc = int32_t (*)(ArkUI_AccessibilityElementInfo*, const char*);
using SetEventStringFunc = int32_t (*)(ArkUI_AccessibilityEventInfo*, const char*);
using SetElemBoolFunc = int32_t (*)(ArkUI_AccessibilityElementInfo*, bool);
using SetElemChildFunc = int32_t (*)(ArkUI_AccessibilityElementInfo*, int32_t, int64_t*);

using SetElemOperActionsFunc = int32_t (*)(ArkUI_AccessibilityElementInfo*, int32_t, ArkUI_AccessibleAction*);
using SetElemSreenRectFunc = int32_t (*)(ArkUI_AccessibilityElementInfo*, ArkUI_AccessibleRect*);
using SetEventFunc = int32_t (*)(ArkUI_AccessibilityEventInfo*, ArkUI_AccessibilityEventType);
using SetEventElemFunc = int32_t (*)(ArkUI_AccessibilityEventInfo*, ArkUI_AccessibilityElementInfo*);

using SetReqFocusFunc = int32_t (*)(ArkUI_AccessibilityEventInfo*, int32_t);
using GetNativeA11yProvider = int32_t (*)(OH_NativeXComponent*, ArkUI_AccessibilityProvider**);
using GetFindActionArgs = int32_t (*)(ArkUI_AccessibilityActionArguments*, const char*, char**);

class OhosAccessibilityDDL {
public:
    OhosAccessibilityDDL();
    ~OhosAccessibilityDDL();

    static constexpr char ACCESSIBILITY_LIB_NAME[] = "libflutter_accessibility.so";
    
    static RegisterFunc DLLoadRegisterFunc(const char* symbolName);
    static RegisterWithInstanceFunc DLLoadRegisterWithInstanceFunc(const char* symbolName);
    static SendAsyncEventFunc DLLoadSendAsyncEventFunc(const char* symbolName);
    static GetElemFunc DLLoadGetElemFunc(const char* symbolName);

    static CreateElemInfoFunc DLLoadCreateElemInfoFunc(const char* symbolName);
    static CreateEventInfoFunc DLLoadCreateEventInfoFunc(const char* symbolName);
    static DestroyElemFunc DLLoadDestroyElemFunc(const char* symbolName);
    static DestroyEventFunc DLLoadDestroyEventFunc(const char* symbolName);
    
    static SetElemIntFunc DLLoadSetElemIntFunc(const char* symbolName);
    static SetElemStringFunc DLLoadSetElemStringFunc(const char* symbolName);
    static SetEventStringFunc DLLoadSetEventStringFunc(const char* symbolName);
    static SetElemBoolFunc DLLoadSetElemBoolFunc(const char* symbolName);
    static SetElemChildFunc DLLoadSetElemChildFunc(const char* symbolName);

    static SetElemOperActionsFunc DLLoadSetElemOperActionsFunc(const char* symbolName);
    static SetElemSreenRectFunc DLLoadSetElemSreenRectFunc(const char* symbolName);
    static SetEventFunc DLLoadSetEventFunc(const char* symbolName);
    static SetEventElemFunc DLLoadSetEventElemFunc(const char* symbolName);
    static SetReqFocusFunc DLLoadSetReqFocusFunc(const char* symbolName);

    static GetNativeA11yProvider DLLoadGetNativeA11yProvider(const char* symbolName);
    static GetFindActionArgs DLLoadGetFindActionArgs(const char* symbolName);
};

}  // namespace flutter
#endif