#ifndef OHOS_HIAPPEVENT_DDL_H
#define OHOS_HIAPPEVENT_DDL_H

#include <hiappevent/hiappevent.h>
#include "ddl_hiappevent.h"

namespace flutter {

using CreateProcessorFunc = HiAppEvent_Processor*(*)(const char* name);
using SetReportRouteFunc = int(*)(HiAppEvent_Processor* processor, const char* appId, const char* routeInfo);
using SetReportPoliceFunc = int(*)(HiAppEvent_Processor* processor, int periodReport, int batchReport, bool onStartReport, bool onBackgroundReport);
using SetReportEventFunc = int(*)(HiAppEvent_Processor* processor, const char* domain, const char* name, bool isRealTime);
using AddFunc = int64_t(*)(HiAppEvent_Processor* processor);

class OhosHiappEventDDL {
public:
    OhosHiappEventDDL();
    ~OhosHiappEventDDL();

    static constexpr char HiAppEvent_LIB_NAME[] = "libhiappevent_ndk.z.so";

    static CreateProcessorFunc DLLoadCreateProcessorFunc(const char* name);
    static SetReportRouteFunc DLLoadSetReportRouteFunc(const char* name);
    static SetReportPoliceFunc DLLoadSetReportPoliceFunc(const char* name);
    static SetReportEventFunc DLLoadSetReportEventFunc(const char* name);
    static AddFunc DLLoadAddFunc(const char* name);
}; // class OhosHiappEventDDL

};// namespcae flutter

#endif // OHOS_HIAPPEVENT_DDL_H