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

#include "flutter/shell/platform/ohos/platform_message_handler_ohos.h"
#include "flutter/fml/make_copyable.h"

namespace flutter {

PlatformMessageHandlerOHOS::PlatformMessageHandlerOHOS(
    const std::shared_ptr<PlatformViewOHOSNapi>& napi_facede,
    fml::RefPtr<fml::TaskRunner> platform_task_runner)
    : napi_facade_(napi_facede),
      platform_task_runner_(std::move(platform_task_runner)) {}

void PlatformMessageHandlerOHOS::HandlePlatformMessage(
    std::unique_ptr<flutter::PlatformMessage> message) {
  int response_id = next_response_id_.fetch_add(1);
  if (auto response = message->response()) {
    std::lock_guard lock(pending_responses_mutex_);
    pending_responses_[response_id] = response;
  }

  platform_task_runner_->PostTask(
      fml::MakeCopyable([response_id = response_id, napi_facede = napi_facade_,
                         message = std::move(message)]() mutable {
        napi_facede->FlutterViewHandlePlatformMessage(response_id,
                                                      std::move(message));
      }));
}

void PlatformMessageHandlerOHOS::InvokePlatformMessageResponseCallback(
    int response_id,
    std::unique_ptr<fml::Mapping> mapping) {
  if (!response_id) {
    return;
  }
  // TODO(gaaclarke): Move the jump to the ui thread here from
  // PlatformMessageResponseDart so we won't need to use a mutex anymore.
  fml::RefPtr<flutter::PlatformMessageResponse> message_response;
  {
    std::lock_guard lock(pending_responses_mutex_);
    auto it = pending_responses_.find(response_id);
    if (it == pending_responses_.end()) {
      return;
    }
    message_response = std::move(it->second);
    pending_responses_.erase(it);
  }

  message_response->Complete(std::move(mapping));
}

void PlatformMessageHandlerOHOS::InvokePlatformMessageEmptyResponseCallback(
    int response_id) {
  if (!response_id) {
    return;
  }
  fml::RefPtr<flutter::PlatformMessageResponse> message_response;
  {
    std::lock_guard lock(pending_responses_mutex_);
    auto it = pending_responses_.find(response_id);
    if (it == pending_responses_.end()) {
      return;
    }
    message_response = std::move(it->second);
    pending_responses_.erase(it);
  }
  message_response->CompleteEmpty();
}

}  // namespace flutter