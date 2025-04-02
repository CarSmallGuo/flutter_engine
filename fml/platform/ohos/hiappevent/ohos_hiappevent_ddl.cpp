#include "ohos_hiappevent_ddl.h"

namespace flutter {

    OhosHiappEventDDL::OhosHiappEventDDL() = default;
    OhosHiappEventDDL::~OhosHiappEventDDL() = default;

    CreateProcessorFunc OhosHiappEventDDL::DLLoadCreateProcessorFunc(const char* name)
    {
        LIBHANDLE handler = LOAD_LIB(HiAppEvent_LIB_NAME);
        if (handler == nullptr) {
            return nullptr;
        }

        auto symbol = reinterpret_cast<CreateProcessorFunc>(LOAD_SYM(handler, name));
        if (symbol == nullptr) {
            CLOSE_LIB(handler);
            return nullptr;
        }
        return symbol;
    }

    SetReportRouteFunc OhosHiappEventDDL::DLLoadSetReportRouteFunc(const char* name)
    {
        LIBHANDLE handler = LOAD_LIB(HiAppEvent_LIB_NAME);
        if (handler == nullptr) {
            return nullptr;
        }

        auto symbol = reinterpret_cast<SetReportRouteFunc>(LOAD_SYM(handler, name));
        if (symbol == nullptr) {
            CLOSE_LIB(handler);
            return nullptr;
        }
        return symbol;
    }

    SetReportPoliceFunc OhosHiappEventDDL::DLLoadSetReportPoliceFunc(const char* name)
    {
        LIBHANDLE handler = LOAD_LIB(HiAppEvent_LIB_NAME);
        if (handler == nullptr) {
            return nullptr;
        }

        auto symbol = reinterpret_cast<SetReportPoliceFunc>(LOAD_SYM(handler, name));
        if (symbol == nullptr) {
            CLOSE_LIB(handler);
            return nullptr;
        }
        return symbol;
    }

    SetReportEventFunc OhosHiappEventDDL::DLLoadSetReportEventFunc(const char* name)
    {
        LIBHANDLE handler = LOAD_LIB(HiAppEvent_LIB_NAME);
        if (handler == nullptr) {
            return nullptr;
        }

        auto symbol = reinterpret_cast<SetReportEventFunc>(LOAD_SYM(handler, name));
        if (symbol == nullptr) {
            CLOSE_LIB(handler);
            return nullptr;
        }
        return symbol;
    }

    AddFunc OhosHiappEventDDL::DLLoadAddFunc(const char* name)
    {
        LIBHANDLE handler = LOAD_LIB(HiAppEvent_LIB_NAME);
        if (handler == nullptr) {
            return nullptr;
        }

        auto symbol = reinterpret_cast<AddFunc>(LOAD_SYM(handler, name));
        if (symbol == nullptr) {
            CLOSE_LIB(handler);
            return nullptr;
        }
        return symbol;
    }
} ;// namespace flutter
