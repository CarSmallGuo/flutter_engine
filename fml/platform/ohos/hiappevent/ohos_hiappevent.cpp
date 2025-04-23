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
static const int vsync_transitions_missed_Size = 2;

std::shared_ptr<OhosHiappEventDDL> OhosHiappEventDDL::GetInstance()
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
    info.missedFrame = std::stoi(argumentValues[vsync_transitions_missed_Size]);
    MissedFrameInfos.push_back(info);
}

int OhosHiappEventDDL::WriteSingleFrame(void)
{
    if (MissedFrameInfos.size() == 0) {
        return -1;
    }

    int64_t endTimeMicros = MissedFrameInfos.front().endTimeMicros;
    int64_t targetTime = MissedFrameInfos.front().targetTime / 1000;
    int64_t lastestTargetTime = MissedFrameInfos.front().lastestTargetTime / 1000;
    int missedFrame = MissedFrameInfos.front().missedFrame;

    if (endTimeMicros < targetTime) {
        FML_LOG(ERROR) << "report error, endTime is less than targetTime";
        return -1;
    }

    int64_t budgetTime = (lastestTargetTime - targetTime) / missedFrame;
    int64_t vsyncStartTime = targetTime - budgetTime;
    ParamList list = OH_HiAppEvent_CreateParamList();
    if (list == nullptr) {
        FML_LOG(ERROR) << "CreateParamList error";
        return -1;
    }

    OH_HiAppEvent_AddStringParam(list, "frameworkName", "FLUTTER");
    OH_HiAppEvent_AddInt32Param(list, "versionCode", 0);
    OH_HiAppEvent_AddInt32Param(list, "missedFrames", missedFrame);
    OH_HiAppEvent_AddInt64Param(list, "startTime", vsyncStartTime);
    OH_HiAppEvent_AddInt64Param(list, "endTime", endTimeMicros);
    OH_HiAppEvent_AddInt64Param(list, "pid", getpid());

    int ret = OH_HiAppEvent_Write("PERFORMANCE", "OTHER_JANK", BEHAVIOR, list);
    if (ret != 0) {
        FML_LOG(ERROR) << "HiAppEvent_Write error, ret = " << ret;
    }

    OH_HiAppEvent_DestroyParamList(list);
    return ret;
}

int OhosHiappEventDDL::WriteStatisticFrame(void)
{
    if (MissedFrameInfos.size() == 0) {
        FML_LOG(ERROR) << "size of MissedFrameInfos is zero";
        return -1;
    }

    ParamList list = OH_HiAppEvent_CreateParamList();
    if (list == nullptr) {
        FML_LOG(ERROR) << "CreateParamList error";
        return -1;
    }

    int totalMissedFrames = 0;
    int maxMissedFrame = 0;
    int targetIndex = 0;
    int index = 0;
    for (auto it = MissedFrameInfos.begin(); it != MissedFrameInfos.end(); it++) {
        totalMissedFrames += (*it).missedFrame;

        if ((*it).missedFrame > maxMissedFrame) {
            maxMissedFrame = (*it).missedFrame;
            targetIndex = index;
        }
        index++;
    }
    // 丢帧最大值
    int64_t maxEndTimeMicros = MissedFrameInfos.at(targetIndex).endTimeMicros;
    int64_t maxTargetTime = MissedFrameInfos.at(targetIndex).targetTime / 1000;
    int64_t maxLastestTargetTime = MissedFrameInfos.at(targetIndex).lastestTargetTime / 1000;

    int64_t maxBudget = (maxLastestTargetTime - maxTargetTime) / maxMissedFrame;
    int64_t maxVsyncStartTime = maxTargetTime - maxBudget;
    int64_t maxDiffTime = maxEndTimeMicros - maxVsyncStartTime;
    int maxFPS = 1000000 / maxBudget;

    // 开始时间 第一次丢帧vsync开始时间
    int64_t frontTargetTime = MissedFrameInfos.front().targetTime / 1000;
    int64_t frontLastestTargetTime = MissedFrameInfos.front().lastestTargetTime / 1000;
    int64_t frontbudgetTime = (frontLastestTargetTime - frontTargetTime) / MissedFrameInfos.front().missedFrame;
    int64_t vsyncStartTime = frontTargetTime - frontbudgetTime;

    // 结束时间 最后一次丢帧vsync结束时间
    int64_t backLastestTargetTime = MissedFrameInfos.back().lastestTargetTime / 1000;

    OH_HiAppEvent_AddStringParam(list, "frameworkName", "FLUTTER");
    OH_HiAppEvent_AddInt32Param(list, "versionCode", 0);

    OH_HiAppEvent_AddInt32Param(list, "maxMissedFrameRate", maxFPS);
    OH_HiAppEvent_AddInt32Param(list, "totalMissedFrames", totalMissedFrames);
    OH_HiAppEvent_AddInt64Param(list, "maxFrameTime", maxDiffTime);
    OH_HiAppEvent_AddInt64Param(list, "startTime", vsyncStartTime);
    OH_HiAppEvent_AddInt64Param(list, "endTime", backLastestTargetTime);
    OH_HiAppEvent_AddInt64Param(list, "pid", getpid());

    int ret = OH_HiAppEvent_Write("PERFORMANCE", "OTHER_JANK_STAT", BEHAVIOR, list);
    if (ret != 0) {
        FML_LOG(ERROR) << "HiAppEvent_Write error, ret = " << ret;
    }

    OH_HiAppEvent_DestroyParamList(list);
    return ret;
}

void OhosHiappEventDDL::WriteSingleFrameFlush(void)
{
    if (!isValid_) {
        FML_LOG(ERROR) << "flush isValid_ false";
        return;
    }

    if (MissedFrameInfos.size() == 0) {
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

    int ret = -1;
    do {
        ret = WriteSingleFrame();
        if (ret != 0) {
            break;
        }
    } while (0);
    destroyProcessor_(processor);
}

void OhosHiappEventDDL::WriteStatisticFrameFlush(void)
{
    if (!isValid_) {
        FML_LOG(ERROR) << "flush isValid_ false";
        return;
    }

    if (MissedFrameInfos.size() == 0) {
        return;
    }

    HiAppEvent_Processor* processor = reinterpret_cast<HiAppEvent_Processor*>(createProcessorFunc_("xperfbridge"));
    if (processor == nullptr) {
        FML_LOG(ERROR) << "processor == nullptr";
        return;
    }

    setReportPoliceFunc_(processor, 1, 1, true, true);
    setReportEventFunc_(processor, "PERFORMANCE", "OTHER_JANK_STAT", true);

    int64_t processorId = addFunc_(processor);
    if (processorId <= 0) {
        FML_LOG(ERROR) << "processorId error";
        destroyProcessor_(processor);
        return;
    }

    int ret = -1;
    do {
        ret = WriteStatisticFrame();
        if (ret != 0) {
            break;
        }
    } while (0);
    destroyProcessor_(processor);
    MissedFrameInfos.clear();
}

};  // namespace hiappevent

};  // namespace fml
