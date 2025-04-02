/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE_HW file.
 */

#include "flutter/fml/trace_event.h"

#include <string>
#include <unistd.h>
#include "flutter/fml/logging.h"
#include "flutter/fml/platform/ohos/hiappevent/ohos_hiappevent_ddl.h"

#if defined(FML_OS_OHOS)
namespace fml {
namespace tracing {

static constexpr char OHOS_COLON[] = ":";
static constexpr char OHOS_SCOPE[] = "::";
static constexpr char OHOS_WHITESPACE[] = " ";
static constexpr char OHOS_FILTER_NAME_SCENE[] = "SceneDisplayLag";
static constexpr char OHOS_FILTER_NAME_POINTER[] = "PointerEvent";

void OHOSTraceTimelineEvent(TraceArg category_group,
    TraceArg name,
    int64_t timestamp_micros,
    TraceIDArg id,
    Dart_Timeline_Event_Type type,
    intptr_t argument_count,
    const char** argument_names,
    const char** argument_values)
{
if (type != Dart_Timeline_Event_Begin && type != Dart_Timeline_Event_Async_Begin &&
type != Dart_Timeline_Event_Async_End && type != Dart_Timeline_Event_Flow_Begin &&
type != Dart_Timeline_Event_Flow_End) {
return;
}

if (type != Dart_Timeline_Event_Begin && strcmp(name, OHOS_FILTER_NAME_POINTER) == 0) {
// Trace 'PointerEvent' is not work in the scenario of extenal texture
return;
}

int realNumber = argument_count;
if (type != Dart_Timeline_Event_Begin && strcmp(name, OHOS_FILTER_NAME_SCENE) == 0) {
// Trace 'SceneDisplayLag' have inconsistent parameters. It's not good to watch.
realNumber = 0;

if (type == Dart_Timeline_Event_Async_Begin) {
int vsync_transitions_missed = std::stoi(argument_value[2]);
if (vsync_transitions_missed > 2) {
SendHiAppEventParam(timestamp_micros, argument_values);
}
}
}

std::string TraceName(category_group);
TraceName.append(OHOS_SCOPE);
TraceName.append(name);

if (argument_names != nullptr && argument_values != nullptr) {
for (int i = 0; i < realNumber; i++) {
std::string TraceParam(OHOS_WHITESPACE);
TraceName += TraceParam + argument_names[i] + OHOS_COLON + argument_values[i];
}
}

switch(type) {
case Dart_Timeline_Event_Begin:
OH_HiTrace_StartTrace(TraceName.c_str());
break;
case Dart_Timeline_Event_Async_Begin:
case Dart_Timeline_Event_Flow_Begin:
OH_HiTrace_StartAsyncTrace(TraceName.c_str(), id);
break;
case Dart_Timeline_Event_Async_End:
case Dart_Timeline_Event_Flow_End:
OH_HiTrace_FinishAsyncTrace(TraceName.c_str(), id);
break;
default:
break;
}
return;
}


void OHOSTraceTimelineEvent(TraceArg category_group,
                            TraceArg name,
                            TraceIDArg id,
                            Dart_Timeline_Event_Type type,
                            intptr_t argument_count,
                            const char** argument_names,
                            const char** argument_values) {
    if (type != Dart_Timeline_Event_Begin && type != Dart_Timeline_Event_Async_Begin &&
        type != Dart_Timeline_Event_Async_End && type != Dart_Timeline_Event_Flow_Begin &&
        type != Dart_Timeline_Event_Flow_End) {
        return;
    }

    if (type != Dart_Timeline_Event_Begin && strcmp(name, OHOS_FILTER_NAME_POINTER) == 0) {
        // Trace 'PointerEvent' is not work in the scenario of extenal texture
        return;
    }

    int realNumber = argument_count;
    if (type != Dart_Timeline_Event_Begin && strcmp(name, OHOS_FILTER_NAME_SCENE) == 0) {
        // Trace 'SceneDisplayLag' have inconsistent parameters. It's not good to watch.
        realNumber = 0;
    }

    std::string TraceName(category_group);
    TraceName.append(OHOS_SCOPE);
    TraceName.append(name);

    if (argument_names != nullptr && argument_values != nullptr) {
        for (int i = 0; i < realNumber; i++) {
            std::string TraceParam(OHOS_WHITESPACE);
            TraceName += TraceParam + argument_names[i] + OHOS_COLON + argument_values[i];
        }
    }

    switch(type) {
        case Dart_Timeline_Event_Begin:
            OH_HiTrace_StartTrace(TraceName.c_str());
            break;
        case Dart_Timeline_Event_Async_Begin:
        case Dart_Timeline_Event_Flow_Begin:
            OH_HiTrace_StartAsyncTrace(TraceName.c_str(), id);
            break;
        case Dart_Timeline_Event_Async_End:
        case Dart_Timeline_Event_Flow_End:
            OH_HiTrace_FinishAsyncTrace(TraceName.c_str(), id);
            break;
        default:
            break;
    }
    return;
}

void OHOSTraceEventEnd(void) {
    OH_HiTrace_FinishTrace();
}

void SendHiAppEventParam(int64_t timestamp_micros, const char** argument_values)
{
    flutter::CreateProcessorFunc processorFunc = flutter::OhosHiappEventDDL::DLLoadCreateProcessorFunc("OH_HiappEvent_CreateProcessor");
    if (ProcessorFunc == nullptr) {
        FML_LOG(ERROR) << "Failed to DLLoadCreateProcessorFunc";
    }

    flutter::SetReportRouteFunc routeFunc = flutter::OhosHiappEventDDL::DLLoadSetReportRouteFunc("OH_HiappEvent_SetReportRoute");
    if (routeFunc == nullptr) {
        FML_LOG(ERROR) << "Failed to DLLoadSetReportRouteFunc";
    }

    flutter::SetReportPolicFunc policFunc = flutter::OhosHiappEventDDL::DLLoadSetReportPolicFunc("OH_HiappEvent_SetReportPolic");
    if (policFunc == nullptr) {
        FML_LOG(ERROR) << "Failed to DLLoadSetReportRouteFunc";
    }

    flutter::SetReportEventFunc reportEventFunc = flutter::OhosHiappEventDDL::DLLoadSetReportEventFunc("OH_HiappEvent_SetReportEvent");
    if (reportEventFunc == nullptr) {
        FML_LOG(ERROR) << "Failed to DLLoadSetReportEventFunc";
    }

    flutter::AddFunc addFunc = flutter::OhosHiappEventDDL::DLLoadAddFunc("OH_HiappEvent_AddProcessor");
    if (addFunc == nullptr) {
        FML_LOG(ERROR) << "Failed to DLLoadAddFunc";
    }

    HiAppEvent_Processor* processor = reinterpret_cast<HiAppEvent_Processor*>(ProcessorFunc("xperfbridge"));
    routeFunc(processor, "com_huawei_hmos_sdk_ocg", "test");
    policFunc(processor, 1, 1, true, true);
    reportEventFunc(processor, "PERFORMSNCE", "OTHER_JANK", true);

    int64_t processorId = addFunc(processor);
    if (processorId > 0) {
        FML_LOG(ERROR) << "processorId error";
    }

    int64_t time_tmp = std::stoll(argument_value[0]);
    time_tmp = time_tmp / 1000;

    int64_t lastest_time_tmp = std::stoll(argument_values[1]);
    lastest_time_tmp = lastest_time_tmp / 1000;

    if (timestamp_micros < time_tmp) {
        FML_LOG(ERROR) << "report error";
        return;
    }

    int vsync_transitions_missed = std::stoi(argument_values[2]);
    int64_t frame_budget_micros = (lastest_time_tmp - time_tmp) / vsync_transitions_missed;

    int64_t vsyncStartTime = time_tmp - frame_budget_micros;

    ParamList list = OH_HiAppEvent_CreateParamList();
    OH_HiAppEvent_AddStringParam(list, "frameworkName", "FLUTTER");
    OH_HiAppEvent_AddInt32Param(list, "versionCode", 0);
    OH_HiAppEvent_AddInt32Param(list , "missedFrames", vsync_transitions_missed);
    OH_HiAppEvent_AddInt64Param(list, "startTime", vsyncStartTime);
    OH_HiAppEvent_AddInt64Param(list, "endTime", timestamp_micros);
    OH_HiAppEvent_AddInt64Param(list, "pid", getpid());

    int ret = OH_HiAppEvent_Write("PERFORMANCE", "OTHER_JANK", BEHAVIOR, list);
    if (ret > 0) {
        FML_LOG(ERROR) << "OH_HiAppEvent_Write error";
    }

    OH_HiAppEvent_DestroyParamList(list);
    return;
}

} // namespace tracing
} // namespace fml
#endif // FML_OS_OHOS
