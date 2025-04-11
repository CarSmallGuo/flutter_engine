/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.	
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
#ifndef OHOS_HIAPPEVENT_H
#define OHOS_HIAPPEVENT_H

#include <vector>
#include <hiappevent/hiappevent.h>

namespace fml{

namespace hiappevent{

using CreateProcessorFunc = HiAppEvent_Processor* (*)(const char* name);
using SetReportRouteFunc = int (*)(HiAppEvent_Processor* processor,
                                  const char* appId,
                                  const char* routeInfo);
using SetReportPoliceFunc = int (*)(HiAppEvent_Processor* processor,
                                    int periodReport,
                                    int batchReport,
                                    bool onStartReport,
                                    bool onBackgroundReport);
using SetReportEventFunc = int (*)(HiAppEvent_Processor* processor,
                                  const char* domain,
                                  const char* name,
                                  bool isRealTime);
using AddFunc = int64_t (*)(HiAppEvent_Processor* processor);
using AddStringParam = ParamList (*)(ParamList list,
                                     const char* name,
                                     const char* str);
using AddInt32Param = ParamList (*)(ParamList list,
                                    const char* name,
                                    int32_t num);
using AddInt64Param = ParamList (*)(ParamList list,
                                    const char* name,
                                    int64_t num);
using Write = int(*)(const char* domain,
                     const char* name,
                     enum EventType type,
                     const ParamList list);
using DestroyParamList = void(*)(ParamList list);
using DestroyProcessor = void(*)(HiAppEvent_Processor* processor);

typedef struct  MissedFrameInfo{
    int64_t timestamp_micros;
    int64_t time_tmp;
    int64_t lastest_time_tmp;
    int vsync_transitions_missed;
}MissedFrameInfo;


class OhosHiappEventDDL {
public:
    OhosHiappEventDDL(void);
    ~OhosHiappEventDDL();

    static constexpr char HiAppEvent_LIB_NAME[] = "libhiappevent_ndk.z.so";

    void init(void);

    static OhosHiappEventDDL* GetInstance(void);

    void reportMissedFrameEvent(int64_t timestamp_micros, const char** argument_values, intptr_t argument_count,);

    void flush(void);

    void apiGet(void);

private:

    void* libhiappevent_ndk_handler_ = nullptr;
    CreateProcessorFunc createProcessorFunc_ = nullptr;
    SetReportRouteFunc setReportRouteFunc_ = nullptr;
    SetReportPoliceFunc setReportPoliceFunc_ = nullptr;
    SetReportEventFunc setReportEventFunc_ = nullptr;
    AddFunc addFunc_ = nullptr;
    AddStringParam addStringParam_ = nullptr;
    AddInt32Param addInt32Param_ = nullptr;
    AddInt64Param addInt64Param_ = nullptr;
    Write write_ = nullptr;
    DestroyParamList destroyParamList_ = nullptr;
    DestroyProcessor destroyProcessor_ = nullptr;
    

    int apiVrsion_ = 0;

    static OhosHiappEventDDL* instance_;

    bool isValid_ = false;

    bool isInit_ = false;

    std::vector<MissedFrameInfo> MissedFrameInfos;

};

};
};


#endif
