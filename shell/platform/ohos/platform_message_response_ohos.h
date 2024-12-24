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

#ifndef PLATFORM_MESSAGE_REPONSE_OHOS_H
#define PLATFORM_MESSAGE_REPONSE_OHOS_H

#include "flutter/fml/macros.h"
#include "flutter/fml/task_runner.h"
#include "flutter/lib/ui/window/platform_message_response.h"
#include "flutter/shell/platform/ohos/napi/platform_view_ohos_napi.h"

#include "flutter/shell/platform/ohos/napi/platform_view_ohos_napi.h"

namespace flutter {

class PlatformMessageResponseOHOS : public flutter::PlatformMessageResponse {
 public:
  // |flutter::PlatformMessageResponse|
  void Complete(std::unique_ptr<fml::Mapping> data) override;

  // |flutter::PlatformMessageResponse|
  void CompleteEmpty() override;

 private:
  PlatformMessageResponseOHOS(
      int response_id,
      std::shared_ptr<PlatformViewOHOSNapi> napi_facade,
      fml::RefPtr<fml::TaskRunner> platform_task_runner);

  ~PlatformMessageResponseOHOS() override;

  const int response_id_;
  const std::shared_ptr<PlatformViewOHOSNapi> napi_facade_;
  const fml::RefPtr<fml::TaskRunner> platform_task_runner_;

  FML_FRIEND_MAKE_REF_COUNTED(PlatformMessageResponseOHOS);
  FML_DISALLOW_COPY_AND_ASSIGN(PlatformMessageResponseOHOS);
};

}  // namespace flutter
#endif