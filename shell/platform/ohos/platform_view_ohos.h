/*
 * Copyright 2013 The Flutter Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef OHOS_PLATFORM_VIEW_H
#define OHOS_PLATFORM_VIEW_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <multimedia/image_framework/image_mdk.h>
#include <multimedia/image_framework/image_pixel_map_mdk.h>

#include "flutter/fml/memory/weak_ptr.h"
#include "flutter/lib/ui/window/platform_message.h"
#include "flutter/shell/common/platform_view.h"
#include "flutter/shell/platform/ohos/context/ohos_context.h"
#include "flutter/shell/platform/ohos/napi/platform_view_ohos_napi.h"
#include "flutter/shell/platform/ohos/ohos_external_texture_gl.h"
#include "flutter/shell/platform/ohos/platform_message_handler_ohos.h"
#include "flutter/shell/platform/ohos/surface/ohos_native_window.h"
#include "flutter/shell/platform/ohos/surface/ohos_snapshot_surface_producer.h"
#include "flutter/shell/platform/ohos/surface/ohos_surface.h"
#include "flutter/shell/platform/ohos/vsync_waiter_ohos.h"
#include "flutter/shell/platform/ohos/accessibility/ohos_semantics_bridge.h"

namespace flutter {

enum class OHOS_THREAD_TYPE {
    OHOS_THREAD_TYPE_PLATFORM,
    OHOS_THREAD_TYPE_UI,
    OHOS_THREAD_TYPE_RASTER,
    OHOS_THREAD_TYPE_IO,
};

class OhosSurfaceFactoryImpl : public OhosSurfaceFactory {
 public:
  OhosSurfaceFactoryImpl(const std::shared_ptr<OHOSContext>& context,
                         bool enable_impeller);

  ~OhosSurfaceFactoryImpl() override;

  std::unique_ptr<OHOSSurface> CreateSurface() override;

 private:
  const std::shared_ptr<OHOSContext>& ohos_context_;
  const bool enable_impeller_;
};

class PlatformViewOHOS final : public PlatformView {
 public:
  PlatformViewOHOS(PlatformView::Delegate& delegate,
                   const flutter::TaskRunners& task_runners,
                   const std::shared_ptr<PlatformViewOHOSNapi>& napi_facade,
                   bool use_software_rendering,
                   uint8_t msaa_samples);

  PlatformViewOHOS(PlatformView::Delegate& delegate,
                   const flutter::TaskRunners& task_runners,
                   const std::shared_ptr<PlatformViewOHOSNapi>& napi_facade,
                   const std::shared_ptr<flutter::OHOSContext>& OHOS_context);

  ~PlatformViewOHOS() override;

  void NotifyCreate(fml::RefPtr<OHOSNativeWindow> native_window);

  void NotifySurfaceWindowChanged(fml::RefPtr<OHOSNativeWindow> native_window);

  void NotifyChanged(const SkISize& size);

  // |PlatformView|
  void NotifyDestroyed() override;

  // todo
  void DispatchPlatformMessage(std::string name,
                               void* message,
                               int messageLenth,
                               int reponseId);

  void DispatchEmptyPlatformMessage(std::string name, int reponseId);

  void DispatchSemanticsAction(int id,
                               int action,
                               void* actionData,
                               int actionDataLenth);
  void RegisterExternalTextureByImage(
      int64_t texture_id,
      ImageNative* image);

  static void OnNativeImageFrameAvailable(void* data);

  std::shared_ptr<OHOSExternalTexture> CreateExternalTexture(int64_t texture_id);

  uint64_t RegisterExternalTexture(int64_t texture_id);

  void SetTextureBufferSize(int64_t texture_id, int32_t width, int32_t height);

  void NotifyTextureResizing(int64_t texture_id, int32_t width, int32_t height);

  void RegisterExternalTextureByPixelMap(int64_t texture_id, NativePixelMap* pixelMap);
  
  void SetExternalTextureBackGroundPixelMap(int64_t texture_id, NativePixelMap* pixelMap);

  void SetExternalTextureBackGroundColor(int64_t texture_id, uint32_t color);

  void UnRegisterExternalTexture(int64_t texture_id);

  // |PlatformView|
  PointerDataDispatcherMaker GetDispatcherMaker() override;

  void LoadDartDeferredLibrary(
      intptr_t loading_unit_id,
      std::unique_ptr<const fml::Mapping> snapshot_data,
      std::unique_ptr<const fml::Mapping> snapshot_instructions) override;

  // |PlatformView|
  void LoadDartDeferredLibraryError(intptr_t loading_unit_id,
                                    const std::string error_message,
                                    bool transient) override;

  void UpdateAssetResolverByType(
      std::unique_ptr<AssetResolver> updated_asset_resolver,
      AssetResolver::AssetResolverType type) override;

  const std::shared_ptr<OHOSContext>& GetOHOSContext() {
    return ohos_context_;
  }

  std::shared_ptr<PlatformMessageHandler> GetPlatformMessageHandler()
      const override {
    return platform_message_handler_;
  }

  void OnTouchEvent(std::shared_ptr<std::string[]> touchPacketString, int size);

  void RunTask(OHOS_THREAD_TYPE type, const fml::closure& task);

  // accessibitliy
  void SetSemanticsBridge(std::shared_ptr<SemanticsBridge> bridge,
                          std::shared_ptr<std::mutex> mutex);
  void AccessibilityAnnounce(std::unique_ptr<char[]>& message);
  void AccessibilityOnTap(int32_t nodeId);
  void AccessibilityOnLongPress(int32_t nodeId);
  void AccessibilityOnTooltip(std::unique_ptr<char[]>& message);
  void OnAccessibilityStateChange(bool state);
  void SetNavigation(bool isNavigation);
  void SetAccessibleNavigation(bool isAccessibleNavigation);
  void SetBoldText(double fontWeightScale);

  void SimulateTouchEvent(SemanticsNodeExtend* node);

 private:
  const std::shared_ptr<PlatformViewOHOSNapi> napi_facade_;
  std::shared_ptr<OHOSContext> ohos_context_;

  std::shared_ptr<OHOSSurface> ohos_surface_;
  std::shared_ptr<PlatformMessageHandlerOHOS> platform_message_handler_;

  std::shared_ptr<OhosSurfaceFactoryImpl> surface_factory_;
  std::map<int64_t, std::shared_ptr<OHOSExternalTextureGL>> external_texture_gl_;

  std::atomic<bool> isDestroyed_;

  // accessibility
  std::queue<std::pair<flutter::SemanticsNodeUpdates,
                       flutter::CustomAccessibilityActionUpdates>>
      semantics_queue_;
  std::shared_ptr<SemanticsBridge> bridge_;
  std::shared_ptr<std::mutex> bridge_mutex_;
  int32_t accessibility_feature_flags_ = 0;
  bool is_accessibility_navigation_ = false;

  bool GetDestroyed();

  void SetDestroyed(bool isDestroyed_);

  void UpdateSemantics(
      flutter::SemanticsNodeUpdates update,
      flutter::CustomAccessibilityActionUpdates actions) override;

  void HandlePlatformMessage(
      std::unique_ptr<flutter::PlatformMessage> message) override;

  void OnPreEngineRestart() const override;

  std::unique_ptr<VsyncWaiter> CreateVSyncWaiter() override;

  std::unique_ptr<Surface> CreateRenderingSurface() override;

  std::shared_ptr<ExternalViewEmbedder> CreateExternalViewEmbedder() override;

  std::unique_ptr<SnapshotSurfaceProducer> CreateSnapshotSurfaceProducer()
      override;

  sk_sp<GrDirectContext> CreateResourceContext() const override;

  void ReleaseResourceContext() const override;

  std::shared_ptr<impeller::Context> GetImpellerContext() const override;

  std::unique_ptr<std::vector<std::string>> ComputePlatformResolvedLocales(
      const std::vector<std::string>& supported_locale_data) override;

  void RequestDartDeferredLibrary(intptr_t loading_unit_id) override;

  void InstallFirstFrameCallback();

  void FireFirstFrameCallback();

  FML_DISALLOW_COPY_AND_ASSIGN(PlatformViewOHOS);
};

}  // namespace flutter
#endif
