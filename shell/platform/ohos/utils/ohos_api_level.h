/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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
#ifndef FLUTTER_SHELL_PLATFORM_OHOS_ACCESSIBILITY_OHOS_API_LEVEL_H_
#define FLUTTER_SHELL_PLATFORM_OHOS_ACCESSIBILITY_OHOS_API_LEVEL_H_
#include <deviceinfo.h>
namespace flutter {
class ApiLevel {
 public:
  static constexpr int32_t API_11 = 11;
  static constexpr int32_t API_12 = 12;
  static constexpr int32_t API_13 = 13;
  static constexpr int32_t API_14 = 14;
  static constexpr int32_t API_15 = 15;
  static constexpr int32_t API_16 = 16;
  static constexpr int32_t API_17 = 17;
  static constexpr int32_t API_18 = 18;
  static constexpr int32_t API_19 = 19;
  static constexpr int32_t API_20 = 20;
};

class DeviceInfo {
 public:
  static int32_t SdkApiVersion() {
    static int32_t apiVersion = OH_GetSdkApiVersion();
    return apiVersion;
  }
};

}  // namespace flutter
#endif