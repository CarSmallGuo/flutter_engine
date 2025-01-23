/*
Copyright (C) 2024 Huawei Device Co., Ltd.

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

#ifndef OHOS_ACCESSIBILITY_DDL_H
#define OHOS_ACCESSIBILITY_DDL_H
#include <arkui/native_interface_accessibility.h>
#include <ace/xcomponent/native_interface_xcomponent.h>
#include "flutter/shell/platform/ohos/utils/ddl_utils.h"

namespace flutter {

using RegisterFunc = int32_t (*)(ArkUI_AccessibilityProvider*, ArkUI_AccessibilityProviderCallbacks*);
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