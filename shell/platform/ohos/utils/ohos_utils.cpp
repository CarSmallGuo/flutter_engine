/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE_HW file.
 */
#include "ohos_utils.h"
namespace flutter {

OHOSUtils::OHOSUtils() {};
OHOSUtils::~OHOSUtils() {};

/**
 * 判断源字符串是否包含目标字符串
 */
bool OHOSUtils::Contains(const std::string& source,
                         const std::string& target)
{
    return source.find(target) != std::string::npos;
}

/**
 * 将string序列化为uint8字节数组
 */
void OHOSUtils::SerializeString(const std::string& str, std::vector<uint8_t>& buffer)
{
    // store the length of the string as a uint8_t
    uint32_t length = str.size();
    buffer.insert(buffer.end(),
                  reinterpret_cast<uint8_t*>(&length),
                  reinterpret_cast<uint8_t*>(&length) + sizeof(length));
    // store the actual string data
    buffer.insert(buffer.end(), str.begin(), str.end());
}

}