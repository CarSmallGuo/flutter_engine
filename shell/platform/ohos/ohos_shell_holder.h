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

#ifndef OHOS_SHELL_HOLDER_H
#define OHOS_SHELL_HOLDER_H
#define FML_USED_ON_EMBEDDER
#include "flutter/assets/asset_manager.h"
#include "flutter/fml/macros.h"
#include "flutter/fml/unique_fd.h"
#include "flutter/lib/ui/window/viewport_metrics.h"
#include "flutter/runtime/platform_data.h"
#include "flutter/shell/common/run_configuration.h"
#include "flutter/shell/common/shell.h"
#include "flutter/shell/common/thread_host.h"

#include "flutter/assets/asset_resolver.h"
#include "flutter/common/settings.h"
#include "flutter/shell/platform/ohos/napi/platform_view_ohos_napi.h"
#include "flutter/shell/platform/ohos/platform_view_ohos.h"

#include "napi_common.h"
#include "ohos_asset_provider.h"
#include "ohos_image_generator.h"

namespace flutter {

class OHOSShellHolder {
 public:
  OHOSShellHolder(const flutter::Settings& settings,
                  std::shared_ptr<PlatformViewOHOSNapi> napi_facade,
                  void* plateform_loop);

  ~OHOSShellHolder();
  bool IsValid() const;

  const flutter::Settings& GetSettings() const;

  fml::WeakPtr<PlatformViewOHOS> GetPlatformView();

  void NotifyLowMemoryWarning();

  void Launch(std::unique_ptr<OHOSAssetProvider> apk_asset_provider,
              const std::string& entrypoint,
              const std::string& libraryUrl,
              const std::vector<std::string>& entrypoint_args);

  std::unique_ptr<OHOSShellHolder> Spawn(
      std::shared_ptr<PlatformViewOHOSNapi> napi_facade,
      const std::string& entrypoint,
      const std::string& libraryUrl,
      const std::string& initial_route,
      const std::vector<std::string>& entrypoint_args) const;

  const std::shared_ptr<PlatformMessageHandler>& GetPlatformMessageHandler()
      const {
    return shell_->GetPlatformMessageHandler();
  }

  const std::weak_ptr<VsyncWaiter> GetVsyncWaiter() const {
    return shell_->GetVsyncWaiter();
  }

 private:
  std::optional<RunConfiguration> BuildRunConfiguration(
      const std::string& entrypoint,
      const std::string& libraryUrl,
      const std::vector<std::string>& entrypoint_args) const;

  const flutter::Settings settings_;
  fml::WeakPtr<PlatformViewOHOS> platform_view_;
  std::shared_ptr<ThreadHost> thread_host_;
  std::unique_ptr<Shell> shell_;
  uint64_t next_pointer_flow_id_ = 0;

  std::unique_ptr<OHOSAssetProvider> assetProvider_;

  std::shared_ptr<PlatformViewOHOSNapi> napi_facade_;

  OHOSShellHolder(const flutter::Settings& settings,
                  const std::shared_ptr<PlatformViewOHOSNapi>& napi_facade,
                  const std::shared_ptr<ThreadHost>& thread_host,
                  std::unique_ptr<Shell> shell,
                  std::unique_ptr<OHOSAssetProvider> apk_asset_provider,
                  const fml::WeakPtr<PlatformViewOHOS>& platform_view);

  static void ThreadDestructCallback(void* value);

  FML_DISALLOW_COPY_AND_ASSIGN(OHOSShellHolder);
};
}  // namespace flutter

#endif
