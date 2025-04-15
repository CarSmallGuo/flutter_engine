/*
 * Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE_HW file.
 */

#ifndef OHOS_HIAPPEVENT_H
#define OHOS_HIAPPEVENT_H

#include <vector>
#include <hiappevent/hiappevent.h>

namespace fml {

namespace hiappevent {

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

using DestroyProcessor = void(*)(HiAppEvent_Processor* processor);

typedef struct MissedFrameInfo {
    int64_t endTimeMicros; // unit: ms
    int64_t targetTime; // unit: us
    int64_t lastestTargetTime; // unit: us
    int missedFrame;
} MissedFrameInfo;


class OhosHiappEventDDL {
public:
    ~OhosHiappEventDDL();

    void Init(void);

    static std::shared_ptr<OhosHiappEventDDL> GetInstance(void);

    void ReportJANKEvent(int64_t endTimeMicros, const char** argumentValues, int argumentCount);

    void Flush(void);

private:
    OhosHiappEventDDL(void);

    void DDLInit(void);

    void* libHiappeventHandler_ = nullptr;

    CreateProcessorFunc createProcessorFunc_ = nullptr;
    SetReportRouteFunc setReportRouteFunc_ = nullptr;
    SetReportPoliceFunc setReportPoliceFunc_ = nullptr;
    SetReportEventFunc setReportEventFunc_ = nullptr;
    AddFunc addFunc_ = nullptr;
    DestroyProcessor destroyProcessor_ = nullptr;

    int apiVersion_ = 0;

    bool isValid_ = false;

    bool isInit_ = false;

    std::vector<MissedFrameInfo> MissedFrameInfos;

};

}; // namespace hiappevent
}; // namespace fml

#endif
