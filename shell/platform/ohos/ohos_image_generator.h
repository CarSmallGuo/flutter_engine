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

#ifndef FLUTTER_IMAGE_GENERATOR_H
#define FLUTTER_IMAGE_GENERATOR_H

#include "flutter/common/task_runners.h"
#include "flutter/fml/memory/ref_ptr.h"
#include "flutter/fml/synchronization/waitable_event.h"
#include "flutter/fml/task_runner.h"
#include "flutter/lib/ui/painting/image_generator.h"

#include "napi/native_api.h"
#include "napi/platform_view_ohos_napi.h"
#include "napi_common.h"

namespace flutter {

class OHOSImageGenerator : public ImageGenerator {
 private:
  explicit OHOSImageGenerator(
      sk_sp<SkData> buffer,
      const fml::RefPtr<fml::TaskRunner>& task_runner,
      std::shared_ptr<PlatformViewOHOSNapi> napi_facade);

  static napi_env g_env;

 public:
  static napi_value ImageNativeInit(napi_env env, napi_callback_info info);

  static napi_value NativeImageDecodeCallback(napi_env env,
                                              napi_callback_info info);

  ~OHOSImageGenerator();

  // |ImageGenerator|
  const SkImageInfo& GetInfo() override;

  // |ImageGenerator|
  unsigned int GetFrameCount() const override;

  // |ImageGenerator|
  unsigned int GetPlayCount() const override;

  // |ImageGenerator|
  const ImageGenerator::FrameInfo GetFrameInfo(
      unsigned int frame_index) const override;

  // |ImageGenerator|
  SkISize GetScaledDimensions(float desired_scale) override;

  // |ImageGenerator|
  bool GetPixels(const SkImageInfo& info,
                 void* pixels,
                 size_t row_bytes,
                 unsigned int frame_index,
                 std::optional<unsigned int> prior_frame) override;

  void DecodeImage();

  static std::shared_ptr<ImageGenerator> MakeFromData(
      sk_sp<SkData> data,
      const TaskRunners& task_runners,
      std::shared_ptr<PlatformViewOHOSNapi> napi_facade);

  fml::RefPtr<fml::TaskRunner> GetTaskRunner() const;

 private:
  sk_sp<SkData> data_;
  sk_sp<SkData> software_decoded_data_;
  const fml::RefPtr<fml::TaskRunner> task_runner_;
  SkImageInfo image_info_;

  std::shared_ptr<PlatformViewOHOSNapi> napi_facade_;

  /// Blocks until the header of the image has been decoded and the image
  /// dimensions have been determined.
  fml::ManualResetWaitableEvent header_decoded_latch_;

  /// Blocks until the image has been fully decoded.
  fml::ManualResetWaitableEvent fully_decoded_latch_;

  // block this unconstruct until nativeCallback called
  fml::ManualResetWaitableEvent native_callback_latch_;

  void DoDecodeImage();

  bool IsValidImageData();

  FML_DISALLOW_COPY_ASSIGN_AND_MOVE(OHOSImageGenerator);
};
}  // namespace flutter
#endif