/*
 * Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE_HW file.
 */

#include "ohos_hiappevent.h"

#include <dlfcn.h>
#include <thread>
#include <unistd.h>
#include <deviceinfo.h>
#include "flutter/fml/logging.h"

namespace fml {

namespace hiappevent {

static std::shared_ptr<OhosHiappEventDDL> instance_ = nullptr;
static std::once_flag instanceFlag_;

static constexpr char HiAppEvent_LIB_NAME[] = "libhiappevent_ndk.z.so";
static const int Missed_Frame_Infos_Size = 10;
static const int Required_Api_Version = 18;
static const int Argument_Size = 3;

OhosHiappEventDDL* OhosHiappEventDDL::GetInstance()
{
    std::call_once(instanceFlag_, [&] {

        instance_ = std::shared_ptr<OhosHiappEventDDL> (new OhosHiappEventDDL());
        
    });

    return instance_;
}

OhosHiappEventDDL::OhosHiappEventDDL(void)
{
    apiVersion_ = OH_GetSdkApiVersion();
    return;
}

OhosHiappEventDDL::~OhosHiappEventDDL() = default;

void OhosHiappEventDDL::DDLInit(void)
{
    libHiappeventHandler_ = dlopen(HiAppEvent_LIB_NAME, RTLD_LAZY | RTLD_LOCAL);
    if (libHiappeventHandler_ == nullptr) {
        FML_LOG(ERROR) << "dlopen libHiappeventHandler_ failed";
        return;
    }

    do {
        createProcessorFunc_ = reinterpret_cast<CreateProcessorFunc>(
            dlsym(libHiappeventHandler_, "OH_HiAppEvent_CreateProcessor"));
        if (createProcessorFunc_ == nullptr) {
            FML_LOG(ERROR) << "createProcessorFunc_ error";
            break;
        }

        setReportRouteFunc_ = reinterpret_cast<SetReportRouteFunc>(
            dlsym(libHiappeventHandler_, "OH_HiAppEvent_SetReportRoute"));
        if (setReportRouteFunc_ == nullptr) {
            FML_LOG(ERROR) << "setReportRouteFunc_ error";
            break;
        }

        setReportPoliceFunc_ = reinterpret_cast<SetReportPoliceFunc>(
            dlsym(libHiappeventHandler_, "OH_HiAppEvent_SetReportPolicy"));
        if (setReportPoliceFunc_ == nullptr) {
            FML_LOG(ERROR) << "setReportPoliceFunc_ error";
            break;
        }

        setReportEventFunc_ = reinterpret_cast<SetReportEventFunc>(
            dlsym(libHiappeventHandler_, "OH_HiAppEvent_SetReportEvent"));
        if (setReportEventFunc_ == nullptr) {
            FML_LOG(ERROR) << "setReportEventFunc_ error";
            break;
        }

        addFunc_ = reinterpret_cast<AddFunc>(
            dlsym(libHiappeventHandler_, "OH_HiAppEvent_AddProcessor"));
        if (addFunc_ == nullptr) {
            FML_LOG(ERROR) << "addFunc_ error";
            break;
        }

        destroyProcessor_ = reinterpret_cast<DestroyProcessor>(
            dlsym(libHiappeventHandler_, "OH_HiAppEvent_DestroyProcessor"));
        if (destroyProcessor_ == nullptr) {
            FML_LOG(ERROR) << "destroyProcessor_ error";
            break;
        }

        isValid_ = true;
    } while (0);
    return;
}

void OhosHiappEventDDL::Init(void)
{
    if (apiVersion_ < Required_Api_Version) {
        return;
    }

    if (isInit_) {
        FML_LOG(INFO) << "Initialization has been completed";
        return;
    }

    std::thread hiappeventThread(&OhosHiappEventDDL::DDLInit, this);
    hiappeventThread.join();
    isInit_ = true;
    return;
}

void OhosHiappEventDDL::ReportJANKEvent(int64_t endTimeMicros,
    const char** argumentValues,
    int argumentCount)
{
    if (!isValid_) {
        FML_LOG(ERROR) << "ReportJANKEvent isValid_ false";
        return;
    }

    if (argumentCount < Argument_Size) {
        FML_LOG(ERROR) << "Array data overflow";
        return;
    }

    if (MissedFrameInfos.size() == Missed_Frame_Infos_Size) {
        // MissedFrameInfos is full.
        FML_LOG(INFO) << "vector stop push_back";
        return;
    } else if (MissedFrameInfos.size() > Missed_Frame_Infos_Size) {
        return;
    }

    /*  argumentValues
        [0]:frame_target_time
        [1]:current_frame_target_time
        [2]:vsync_transitions_missed
     */
    MissedFrameInfo info;
    info.endTimeMicros = endTimeMicros;
    info.targetTime = std::stoll(argumentValues[0]);
    info.lastestTargetTime = std::stoll(argumentValues[1]);
    info.missedFrame = std::stoi(argumentValues[2]);
    MissedFrameInfos.push_back(info);
}

void OhosHiappEventDDL::Flush(void)
{
    if (!isValid_) {
        FML_LOG(ERROR) << "flush isValid_ false";
        return;
    }

    HiAppEvent_Processor* processor = reinterpret_cast<HiAppEvent_Processor*>(createProcessorFunc_("xperfbridge"));
    if (processor == nullptr) {
        FML_LOG(ERROR) << "processor == nullptr";
        return;
    }

    setReportPoliceFunc_(processor, 1, 1, true, true);
    setReportEventFunc_(processor, "PERFORMANCE", "OTHER_JANK", true);

    int64_t processorId = addFunc_(processor);
    if (processorId <= 0) {
        FML_LOG(ERROR) << "processorId error";
        destroyProcessor_(processor);
        return;
    }

    for (auto it = MissedFrameInfos.begin(); it != MissedFrameInfos.end(); it++) {
        int64_t endTimeMicros = (*it).endTimeMicros;

        int64_t targetTime = (*it).targetTime / 1000;

        int64_t lastestTargetTime = (*it).lastestTargetTime / 1000;

        int missedFrame = (*it).missedFrame;

        if (endTimeMicros < targetTime) {
            FML_LOG(ERROR) << "report error, endTime is less than targetTime";
            destroyProcessor_(processor);
            return;
        }

        int64_t budgetTime = (lastestTargetTime - targetTime) / missedFrame;
        int64_t vsyncStartTime = targetTime - budgetTime;

        ParamList list = OH_HiAppEvent_CreateParamList();
        if (list == nullptr) {
            FML_LOG(ERROR) << "CreateParamList error";
            destroyProcessor_(processor);
            return;
        }
        OH_HiAppEvent_AddStringParam(list, "frameworkName", "FLUTTER");
        OH_HiAppEvent_AddInt32Param(list, "versionCode", 0);
        OH_HiAppEvent_AddInt32Param(list, "missedFrames", missedFrame);
        OH_HiAppEvent_AddInt64Param(list, "startTime", vsyncStartTime);
        OH_HiAppEvent_AddInt64Param(list, "endTime", endTimeMicros);
        OH_HiAppEvent_AddInt64Param(list, "pid", getpid());

        OH_HiAppEvent_Write("PERFORMANCE", "OTHER_JANK", BEHAVIOR, list);

        OH_HiAppEvent_DestroyParamList(list);
    }
    destroyProcessor_(processor);
    MissedFrameInfos.clear();
}
};  // namespace hiappevent

};  // namespace fml
