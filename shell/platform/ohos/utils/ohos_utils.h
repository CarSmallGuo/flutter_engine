// Copyright (C) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef OHOS_UTILS_H
#define OHOS_UTILS_H
#include <string>
#include <map>
#include <string>
#include <vector>
#include <cstring>
#include "flutter/shell/platform/ohos/ohos_logging.h"
namespace flutter {

class OHOSUtils {
public:
    OHOSUtils();
    ~OHOSUtils();
    
    static bool Contains(const std::string source, const std::string target);
    static void SerializeString(const std::string& str, std::vector<uint8_t>& buffer);
    static std::vector<uint8_t> SerializeStringIntMap(const std::map<std::string, int32_t>& mp);
    static void CharArrayToInt32(const char* str, int32_t& target);
};

}
#endif
