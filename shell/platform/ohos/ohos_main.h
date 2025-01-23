/*
Copyright (c) 2023 Hunan OpenValley Digital Industry Development Co., Ltd.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of pngout nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef __OHOS__MAIN__H
#define __OHOS__MAIN__H
#define FML_USED_ON_EMBEDDER
#include "flutter/common/settings.h"
#include "flutter/fml/macros.h"
#include "flutter/runtime/dart_service_isolate.h"
#include "napi/native_api.h"
#include "napi_common.h"
#include "ohos_image_generator.h"

namespace flutter {
class OhosMain {
 public:
  ~OhosMain();
  static OhosMain& Get();
  const flutter::Settings& GetSettings() const;
  static napi_value NativeInit(napi_env env, napi_callback_info info);
  static bool IsEmulator();

 private:
  const flutter::Settings settings_;
  DartServiceIsolate::CallbackHandle observatory_uri_callback_;

  explicit OhosMain(const flutter::Settings& settings);

  static napi_value Init(napi_env env, napi_callback_info info);

  void SetupObservatoryUriCallback(napi_env env, napi_callback_info info);
  FML_DISALLOW_COPY_AND_ASSIGN(OhosMain);
};
}  // namespace flutter
#endif
