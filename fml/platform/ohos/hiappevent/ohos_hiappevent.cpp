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
#include "ohos_hiappevent.h"

#include <thread>
#include <dlfcn.h>
#include <deviceinfo.h>
#include "flutter/fml/logging.h"


namespace fml{

namespace hiappevent{
OhosHiappEventDDL* OhosHiappEventDDL::instance_ = nullptr;

OhosHiappEventDDL* OhosHiappEventDDL::GetInstance() {
    if (instance_ == nullptr) {
        instance_ = new OhosHiappEventDDL();
    }
    return instance_;
}

OhosHiappEventDDL::OhosHiappEventDDL(void)
{
    apiVrsion_ = OH_GetSdkApiVersion();
    return;
}

OhosHiappEventDDL::~OhosHiappEventDDL() = default;

void OhosHiappEventDDL::apiGet(void) {
    libhiappevent_ndk_handler_ = 
        dlopen(HiAppEvent_LIB_NAME, RTLD_LAZY | RTLD_LOCAL);
    if (libhiappevent_ndk_handler_ == nullptr) {
      FML_LOG(ERROR) << "dlopen libhiappevent_ndk_handler_ failed";
      return;
    }

    do {
        createProcessorFunc_ = reinterpret_cast<CreateProcessorFunc>(
            dlsym(libhiappevent_ndk_handler_,"OH_HiAppEvent_CreateProcessor"));
            if (createProcessorFunc_ == nullptr) {
                FML_LOG(ERROR) << "createProcessorFunc_ error";
                break;
            }

        setReportRouteFunc_ = reinterpret_cast<SetReportRouteFunc>(
            dlsym(libhiappevent_ndk_handler_, "OH_HiAppEvent_SetReportRoute"));
            if (setReportRouteFunc_ == nullptr) {
                FML_LOG(ERROR) << "setReportRouteFunc_ error";
                break;
            }

        setReportPoliceFunc_ = reinterpret_cast<SetReportPoliceFunc>(
            dlsym(libhiappevent_ndk_handler_, "OH_HiAppEvent_SetReportPolicy"));
            if (setReportPoliceFunc_ == nullptr) {
                FML_LOG(ERROR) << "setReportPoliceFunc_ error";
                break;
            }

        setReportEventFunc_ = reinterpret_cast<SetReportEventFunc>(
            dlsym(libhiappevent_ndk_handler_, "OH_HiAppEvent_SetReportEvent"));
            if (setReportEventFunc_ == nullptr) {
                FML_LOG(ERROR) << "setReportEventFunc_ error";
                break;
            }

        addFunc_ = reinterpret_cast<AddFunc>(
            dlsym(libhiappevent_ndk_handler_, "OH_HiAppEvent_AddProcessor"));
            if (addFunc_ == nullptr) {
                FML_LOG(ERROR) << "addFunc_ error";
                break;
            }

        addStringParam_ = reinterpret_cast<AddStringParam>(
            dlsym(libhiappevent_ndk_handler_, "OH_HiAppEvent_AddStringParam"));
            if (addStringParam_ == nullptr) {
                FML_LOG(ERROR) << "addStringParam_ error";
                break;
            }

        addInt32Param_ = reinterpret_cast<AddInt32Param>(
            dlsym(libhiappevent_ndk_handler_, "OH_HiAppEvent_AddInt32Param"));
            if (addInt32Param_ == nullptr) {
                FML_LOG(ERROR) << "addInt32Param_ error";
                break;
            }

        addInt64Param_ = reinterpret_cast<AddInt64Param>(
            dlsym(libhiappevent_ndk_handler_, "OH_HiAppEvent_AddInt64Param"));
            if (addInt64Param_ == nullptr) {
                FML_LOG(ERROR) << "addInt64Param_ error";
                break;
            }

        write_ = reinterpret_cast<Write>(
            dlsym(libhiappevent_ndk_handler_, "OH_HiAppEvent_Write"));
            if (write_ == nullptr) {
                FML_LOG(ERROR) << "write_ error";
                break;
            }

        destroyParamList_ = reinterpret_cast<DestroyParamList>(
            dlsym(libhiappevent_ndk_handler_, "OH_HiAppEvent_DestroyParamList"));
            if (destroyParamList_ == nullptr) {
                FML_LOG(ERROR) << "destroyParamList_ error";
                break;
            }

        destroyProcessor_ = reinterpret_cast<DestroyProcessor>(
            dlsym(libhiappevent_ndk_handler_, "OH_HiAppEvent_DestroyProcessor"));
            if (destroyProcessor_ == nullptr) {
                FML_LOG(ERROR) << "destroyProcessor_ error";
                break;
            }
        isValid_ = true;
      } while (0);   
      return;
}

void OhosHiappEventDDL::init(void) {
    if (apiVrsion_ < 18) {
        FML_LOG(ERROR) << "apiVrsion error" << apiVrsion_;
        return;
    }

    if (isInit_) {
        FML_LOG(INFO) << "Initialization has been completed";
        return;
    }

    std::thread hiappeventThread(&OhosHiappEventDDL::apiGet, this);
    hiappeventThread.join();
    isInit_ = true;

    return;
}

void OhosHiappEventDDL::reportMissedFrameEvent(int64_t timestamp_micros, 
                                               const char** argument_values,
                                               intptr_t argument_count) {

    if (!isValid_) {
      FML_LOG(ERROR) << "reportMissedFrameEvent isValid_ false";
      return;
    }

    if (MissedFrameInfos.size() == 20) {
      FML_LOG(INFO) << "vector stop push_back";
      return;
    }else if (MissedFrameInfos.size() > 20) {
      return;
    }

    if (argument_values < 3) {
        FML_LOG(ERROR) << "Array data overflow";
        return;
    }

    MissedFrameInfo info;
    info.timestamp_micros = timestamp_micros;
    info.time_tmp = std::stoll(argument_values[0]);
    info.lastest_time_tmp = std::stoll(argument_values[0]);
    info.vsync_transitions_missed = std::stoi(argument_values[2]);
    MissedFrameInfos.push_back(info);
  }

  void OhosHiappEventDDL::flush(void) {
    if (!isValid_) {
      FML_LOG(ERROR) << "flush isValid_ false";
      return;
    }

  HiAppEvent_Processor* processor = reinterpret_cast<HiAppEvent_Processor*>(
      createProcessorFunc_("xperfbridge"));
  if(processor == nullptr) {
      FML_LOG(ERROR)<<"processor == nullptr";
      destroyProcessor_(processor);
      return;
  }

    setReportRouteFunc_(processor,"flutter",nullptr);
    setReportPoliceFunc_(processor,1,1,true,true);
    setReportEventFunc_(processor,"PERFORMANCE","OTHER_JANK",true);

    int64_t processorId = addFunc_(processor);
    if (processorId <= 0){
      FML_LOG(ERROR)<<"processorId error";
      destroyProcessor_(processor);
      return;
  }


  for (auto it = MissedFrameInfos.begin();it != MissedFrameInfos.end(); it++) {
    int64_t time_tmp = (*it).time_tmp;
    time_tmp = time_tmp / 1000;

    int64_t lastest_time_tmp = (*it).lastest_time_tmp;
    lastest_time_tmp = lastest_time_tmp / 1000;

    int64_t timestamp_micros = (*it).timestamp_micros;
    if (timestamp_micros < time_tmp) {
        FML_LOG(ERROR) << "report error";
        destroyProcessor_(processor);
        return;
    }

    int vsync_transitions_missed = (*it).vsync_transitions_missed;
    int64_t frame_budget_micros = 
        (lastest_time_tmp - time_tmp) / vsync_transitions_missed;

    int64_t vsyncStartTime = time_tmp - frame_budget_micros;

    ParamList list = OH_HiAppEvent_CreateParamList();
    if (list == nullptr) {
        FML_LOG(ERROR)<<"list error";
        destroyProcessor_(processor);
        return;
    }
    addStringParam_(list,"frameworkName","FLUTTER");
    addInt32Param_(list,"versionCode",0);
    addInt32Param_(list,"missedFrames",vsync_transitions_missed);
    addInt64Param_(list,"startTime",vsyncStartTime);
    addInt64Param_(list,"endTime",timestamp_micros);
    addInt64Param_(list,"pid",getpid());

    write_("PERFORMANCE","OTHER_JANK",BEHAVIOR,list);

    destroyParamList_(list);
    }

    destroyProcessor_(processor);
    MissedFrameInfos.clear();
}
}; //namespace hiappevent

}; // namespace fml
