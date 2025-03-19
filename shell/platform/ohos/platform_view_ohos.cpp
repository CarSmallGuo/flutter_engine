/*
 * Copyright 2013 The Flutter Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "flutter/shell/platform/ohos/platform_view_ohos.h"
#include "flutter/fml/make_copyable.h"
#include "flutter/lib/ui/window/viewport_metrics.h"
#include "flutter/shell/common/shell_io_manager.h"
#include "flutter/shell/platform/ohos/ohos_context_gl_skia.h"
#include "flutter/shell/platform/ohos/ohos_surface_gl_skia.h"
#include "flutter/shell/platform/ohos/ohos_surface_software.h"
#include "flutter/shell/platform/ohos/platform_message_response_ohos.h"
#include "napi_common.h"
#include "ohos_external_texture_gl.h"
#include "ohos_logging.h"
#include "flutter/shell/platform/ohos/platform_view_ohos_delegate.h"
#include <GLES2/gl2ext.h>


namespace flutter {

// This global map's key is (PlatformViewOHOS-ptr + texture_id) because there
// may be many platformViews.
std::map<uint64_t, PlatformViewOHOS*> g_texture_platformview_map;

OhosSurfaceFactoryImpl::OhosSurfaceFactoryImpl(
    const std::shared_ptr<OHOSContext>& context,
    bool enable_impeller)
    : ohos_context_(context), enable_impeller_(enable_impeller) {}

OhosSurfaceFactoryImpl::~OhosSurfaceFactoryImpl() = default;

std::unique_ptr<OHOSSurface> OhosSurfaceFactoryImpl::CreateSurface() {
  switch (ohos_context_->RenderingApi()) {
    case OHOSRenderingAPI::kSoftware:
      return std::make_unique<OHOSSurfaceSoftware>(ohos_context_);
    case OHOSRenderingAPI::kOpenGLES:
      if (enable_impeller_) {
        FML_LOG(ERROR) << "It does not support Impeller.";
        return nullptr;
      } else {
        FML_LOG(INFO) << "OhosSurfaceFactoryImpl::OhosSurfaceGLSkia ";
        return std::make_unique<OhosSurfaceGLSkia>(ohos_context_);
      }
    default:
      FML_DCHECK(false);
      return nullptr;
  }
}

std::unique_ptr<OHOSContext> CreateOHOSContext(
    bool use_software_rendering,
    const flutter::TaskRunners& task_runners,
    uint8_t msaa_samples,
    bool enable_impeller) {
  if (use_software_rendering) {
    return std::make_unique<OHOSContext>(OHOSRenderingAPI::kSoftware);
  }
  if (enable_impeller) {
    FML_LOG(ERROR) << "It does not support Impeller.";
    return nullptr;
  }

  return std::make_unique<OhosContextGLSkia>(
      OHOSRenderingAPI::kOpenGLES, fml::MakeRefCounted<OhosEnvironmentGL>(),
      task_runners, msaa_samples);
}

PlatformViewOHOS::PlatformViewOHOS(
    PlatformView::Delegate& delegate,
    const flutter::TaskRunners& task_runners,
    const std::shared_ptr<PlatformViewOHOSNapi>& napi_facade,
    bool use_software_rendering,
    uint8_t msaa_samples)
    : PlatformViewOHOS(
          delegate,
          task_runners,
          napi_facade,
          CreateOHOSContext(
              use_software_rendering,
              task_runners,
              msaa_samples,
              delegate.OnPlatformViewGetSettings().enable_impeller)) {}

PlatformViewOHOS::PlatformViewOHOS(
    PlatformView::Delegate& delegate,
    const flutter::TaskRunners& task_runners,
    const std::shared_ptr<PlatformViewOHOSNapi>& napi_facade,
    const std::shared_ptr<flutter::OHOSContext>& ohos_context)
    : PlatformView(delegate, task_runners),
      napi_facade_(napi_facade),
      ohos_context_(ohos_context),
      platform_message_handler_(new PlatformMessageHandlerOHOS(
          napi_facade,
          task_runners_.GetPlatformTaskRunner())) {
  if (ohos_context_) {
    FML_CHECK(ohos_context_->IsValid())
        << "Could not create surface from invalid Android context.";
    LOGI("ohos_surface_ end 1");
    surface_factory_ = std::make_shared<OhosSurfaceFactoryImpl>(
        ohos_context_, delegate.OnPlatformViewGetSettings().enable_impeller);
    LOGI("ohos_surface_ end 2");
    ohos_surface_ = surface_factory_->CreateSurface();
    LOGI("ohos_surface_ end 3");
    FML_CHECK(ohos_surface_ && ohos_surface_->IsValid())
        << "Could not create an OpenGL, Vulkan or Software surface to set "
           "up "
           "rendering.";
  }
}

PlatformViewOHOS::~PlatformViewOHOS() {
  FML_LOG(INFO) << "PlatformViewOHOS::~PlatformViewOHOS";
  for (auto const &it : external_texture_gl_) {
    if (it.second != nullptr) {
      FML_LOG(INFO) << " nativeImage of textureId " << it.first << " will destroy";
      // if (it.second->nativeImage_ != nullptr) { //TODOTEST
      //   OH_NativeImage_Destroy(&(it.second->nativeImage_));
      //   it.second->nativeImage_ = nullptr;
      // }
    }
  }
  external_texture_gl_.clear();
}

void PlatformViewOHOS::NotifyCreate(
    fml::RefPtr<OHOSNativeWindow> native_window) {
  LOGI("NotifyCreate start");
  SetDestroyed(false);
  if (ohos_surface_) {
    InstallFirstFrameCallback();
    LOGI("NotifyCreate start1");
    fml::AutoResetWaitableEvent latch;
    fml::TaskRunner::RunNowOrPostTask(
        task_runners_.GetRasterTaskRunner(),
        [&latch, surface = ohos_surface_.get(),
         native_window = std::move(native_window), this]() {
          if (GetDestroyed()) {
            LOGW("NotifyCreate, GetDestroyed is true, ignore this call.");
          } else {
            LOGI("NotifyCreate start4");
            surface->SetNativeWindow(native_window);
          }
          latch.Signal();
        });
    latch.Wait();
  }

  PlatformView::NotifyCreated();
}

void PlatformViewOHOS::NotifySurfaceWindowChanged(
    fml::RefPtr<OHOSNativeWindow> native_window) {
  LOGI("PlatformViewOHOS NotifySurfaceWindowChanged enter");
  if (ohos_surface_) {
    fml::AutoResetWaitableEvent latch;
    fml::TaskRunner::RunNowOrPostTask(
        task_runners_.GetRasterTaskRunner(),
        [&latch, surface = ohos_surface_.get(),
         native_window = std::move(native_window), this]() {
          if (GetDestroyed()) {
            LOGW("NotifySurfaceWindowChanged, GetDestroyed is true, ignore this call.");
          } else {
            surface->TeardownOnScreenContext();
            surface->SetNativeWindow(native_window);
          }
          latch.Signal();
        });
    latch.Wait();
  }
}

void PlatformViewOHOS::NotifyChanged(const SkISize& size) {
    //Do nothing, because SetViewportMetrics has notified window size change event
    //If raster thread post task, Synchronization signal block application main thread
    //(https://gitee.com/openharmony-sig/flutter_engine/issues/IBI4PK?from=project-issue)
    return;
}

bool PlatformViewOHOS::GetDestroyed() {
  return isDestroyed_;
}

void PlatformViewOHOS::SetDestroyed(bool isDestroyed) {
  isDestroyed_ = isDestroyed;
}

// |PlatformView|
void PlatformViewOHOS::NotifyDestroyed() {
  SetDestroyed(true);
  LOGI("PlatformViewOHOS NotifyDestroyed enter");
  PlatformView::NotifyDestroyed();
  if (ohos_surface_) {
    fml::AutoResetWaitableEvent latch;
    fml::TaskRunner::RunNowOrPostTask(
        task_runners_.GetRasterTaskRunner(),
        [&latch, surface = ohos_surface_.get()]() {
          surface->TeardownOnScreenContext();
          latch.Signal();
        });
    latch.Wait();
  }
}

// todo

void PlatformViewOHOS::DispatchPlatformMessage(std::string name,
                                               void* message,
                                               int messageLenth,
                                               int reponseId) {
  FML_DLOG(INFO) << "DispatchPlatformMessage（" << name << "," << messageLenth
                 << "," << reponseId;
  fml::MallocMapping mapMessage =
      fml::MallocMapping::Copy(message, messageLenth);

  fml::RefPtr<flutter::PlatformMessageResponse> response;
  response = fml::MakeRefCounted<PlatformMessageResponseOHOS>(
      reponseId, napi_facade_, task_runners_.GetPlatformTaskRunner());

  PlatformView::DispatchPlatformMessage(
      std::make_unique<flutter::PlatformMessage>(
          std::move(name), std::move(mapMessage), std::move(response)));
}

void PlatformViewOHOS::DispatchEmptyPlatformMessage(std::string name,
                                                    int reponseId) {
  FML_DLOG(INFO) << "DispatchEmptyPlatformMessage（" << name << ""
                 << "," << reponseId;
  fml::RefPtr<flutter::PlatformMessageResponse> response;
  response = fml::MakeRefCounted<PlatformMessageResponseOHOS>(
      reponseId, napi_facade_, task_runners_.GetPlatformTaskRunner());

  PlatformView::DispatchPlatformMessage(
      std::make_unique<flutter::PlatformMessage>(std::move(name),
                                                 std::move(response)));
}

void PlatformViewOHOS::DispatchSemanticsAction(int id,
                                               int action,
                                               void* actionData,
                                               int actionDataLenth) {
  FML_DLOG(INFO) << "DispatchSemanticsAction -> id=" << id << ", action=" << action << ", actionDataLenth"
                 << actionDataLenth;
  auto args_vector = fml::MallocMapping::Copy(actionData, actionDataLenth);

  PlatformView::DispatchSemanticsAction(
      id, static_cast<flutter::SemanticsAction>(action),
      std::move(args_vector));
}

// |PlatformView|
void PlatformViewOHOS::LoadDartDeferredLibrary(
    intptr_t loading_unit_id,
    std::unique_ptr<const fml::Mapping> snapshot_data,
    std::unique_ptr<const fml::Mapping> snapshot_instructions) {
  FML_DLOG(INFO) << "LoadDartDeferredLibrary:" << loading_unit_id;
  delegate_.LoadDartDeferredLibrary(loading_unit_id, std::move(snapshot_data),
                                    std::move(snapshot_instructions));
}

void PlatformViewOHOS::LoadDartDeferredLibraryError(
    intptr_t loading_unit_id,
    const std::string error_message,
    bool transient) {
  FML_DLOG(INFO) << "LoadDartDeferredLibraryError:" << loading_unit_id << ":"
                 << error_message;
  delegate_.LoadDartDeferredLibraryError(loading_unit_id, error_message,
                                         transient);
}

// |PlatformView|
void PlatformViewOHOS::UpdateAssetResolverByType(
    std::unique_ptr<AssetResolver> updated_asset_resolver,
    AssetResolver::AssetResolverType type) {
  FML_DLOG(INFO) << "UpdateAssetResolverByType";
  delegate_.UpdateAssetResolverByType(std::move(updated_asset_resolver), type);
}

// |PlatformView|
void PlatformViewOHOS::UpdateSemantics(
    flutter::SemanticsNodeUpdates update,
    flutter::CustomAccessibilityActionUpdates actions) {
    FML_DLOG(INFO) << "PlatformViewOHOS::UpdateSemantics()";
    auto nativeAccessibilityChannel_ = std::make_shared<NativeAccessibilityChannel>();
    nativeAccessibilityChannel_->UpdateSemantics(std::move(update), std::move(actions));
}

// |PlatformView|
void PlatformViewOHOS::HandlePlatformMessage(
    std::unique_ptr<flutter::PlatformMessage> message) {
  FML_DLOG(INFO) << "HandlePlatformMessage";
  platform_message_handler_->HandlePlatformMessage(std::move(message));
}

// |PlatformView|
void PlatformViewOHOS::OnPreEngineRestart() const {
  FML_DLOG(INFO) << "OnPreEngineRestart";
  task_runners_.GetPlatformTaskRunner()->PostTask(
      fml::MakeCopyable([napi_facede = napi_facade_]() mutable {
        napi_facede->FlutterViewOnPreEngineRestart();
      }));
}

// |PlatformView|
std::unique_ptr<VsyncWaiter> PlatformViewOHOS::CreateVSyncWaiter() {
  FML_DLOG(INFO) << "CreateVSyncWaiter";
  return std::make_unique<VsyncWaiterOHOS>(task_runners_);
}

// |PlatformView|
std::unique_ptr<Surface> PlatformViewOHOS::CreateRenderingSurface() {
  FML_DLOG(INFO) << "CreateRenderingSurface";
  if (ohos_surface_ == nullptr) {
    FML_DLOG(ERROR) << "CreateRenderingSurface Failed.ohos_surface_ is null ";
    return nullptr;
  }

  LOGD("return CreateGPUSurface");
  return ohos_surface_->CreateGPUSurface(
      ohos_context_->GetMainSkiaContext().get());
}

// |PlatformView|
std::shared_ptr<ExternalViewEmbedder>
PlatformViewOHOS::CreateExternalViewEmbedder() {
  FML_DLOG(INFO) << "CreateExternalViewEmbedder";
  return nullptr;
}

// |PlatformView|
std::unique_ptr<SnapshotSurfaceProducer>
PlatformViewOHOS::CreateSnapshotSurfaceProducer() {
  FML_DLOG(INFO) << "CreateSnapshotSurfaceProducer";
  return std::make_unique<OHOSSnapshotSurfaceProducer>(*(ohos_surface_.get()));
}

// |PlatformView|
sk_sp<GrDirectContext> PlatformViewOHOS::CreateResourceContext() const {
  FML_DLOG(INFO) << "CreateResourceContext";
  if (!ohos_surface_) {
    return nullptr;
  }
  sk_sp<GrDirectContext> resource_context;
  if (ohos_surface_->ResourceContextMakeCurrent()) {
    // TODO(chinmaygarde): Currently, this code depends on the fact that only
    // the OpenGL surface will be able to make a resource context current. If
    // this changes, this assumption breaks. Handle the same.
    resource_context = ShellIOManager::CreateCompatibleResourceLoadingContext(
        GrBackend::kOpenGL_GrBackend,
        GPUSurfaceGLDelegate::GetDefaultPlatformGLInterface());
  } else {
    FML_DLOG(ERROR) << "Could not make the resource context current.";
  }

  return resource_context;
}

// |PlatformView|
void PlatformViewOHOS::ReleaseResourceContext() const {
  LOGI("ReleaseResourceContext");
  if (ohos_surface_) {
    ohos_surface_->ResourceContextClearCurrent();
  }
}

// |PlatformView|
std::shared_ptr<impeller::Context> PlatformViewOHOS::GetImpellerContext()
    const {
  FML_DLOG(INFO) << "GetImpellerContext";
  if (ohos_surface_) {
    return ohos_surface_->GetImpellerContext();
  }
  return nullptr;
}

// |PlatformView|
std::unique_ptr<std::vector<std::string>>
PlatformViewOHOS::ComputePlatformResolvedLocales(
    const std::vector<std::string>& supported_locale_data) {
  FML_DLOG(INFO) << "ComputePlatformResolvedLocales";
  return napi_facade_->FlutterViewComputePlatformResolvedLocales(
      supported_locale_data);
}

// |PlatformView|
void PlatformViewOHOS::RequestDartDeferredLibrary(intptr_t loading_unit_id) {
  FML_DLOG(INFO) << "RequestDartDeferredLibrary:" << loading_unit_id;
  return;
}

void PlatformViewOHOS::InstallFirstFrameCallback() {
  FML_DLOG(INFO) << "InstallFirstFrameCallback";
  SetNextFrameCallback(
      [platform_view = GetWeakPtr(),
       platform_task_runner = task_runners_.GetPlatformTaskRunner()]() {
        platform_task_runner->PostTask([platform_view]() {
          // Back on Platform Task Runner.
          FML_DLOG(INFO) << "install InstallFirstFrameCallback ";
          if (platform_view) {
            reinterpret_cast<PlatformViewOHOS*>(platform_view.get())
                ->FireFirstFrameCallback();
          }
        });
      });
}

void PlatformViewOHOS::FireFirstFrameCallback() {
  FML_DLOG(INFO) << "FlutterViewOnFirstFrame";
  napi_facade_->FlutterViewOnFirstFrame();
}

void PlatformViewOHOS::RegisterExternalTextureByImage(int64_t texture_id,
                                                      ImageNative* image) {
  if (ohos_context_->RenderingApi() == OHOSRenderingAPI::kOpenGLES) {
    auto iter = external_texture_gl_.find(texture_id);
    if (iter != external_texture_gl_.end()) {
      // iter->second->DispatchImage(image); //TODOTEST
    } else {
      auto extrenal_texture = CreateExternalTexture(texture_id);
      if (extrenal_texture != nullptr) {
        //extrenal_texture->SetPixelMapAsProducer(pixelMap, pixelMap_native_buffer); //TODOTEST
      }
      // ohos_external_gl->DispatchImage(image); //TODOTEST
    }
  }
}

PointerDataDispatcherMaker PlatformViewOHOS::GetDispatcherMaker() {
  return [](DefaultPointerDataDispatcher::Delegate& delegate) {
    return std::make_unique<SmoothPointerDataDispatcher>(delegate);
  };
}

void PlatformViewOHOS::OnNativeImageFrameAvailable(void* data) {
  uint64_t ptexture_id = (uint64_t)data;
  //std::lock_guard<std::mutex> lock(g_map_mutex); //TODOTEST
  if (g_texture_platformview_map.find(ptexture_id) ==
      g_texture_platformview_map.end()) {
    return;
  }
  PlatformViewOHOS* platform = g_texture_platformview_map[ptexture_id];

  if (platform == nullptr || platform->ohos_surface_ == nullptr) {
    FML_LOG(ERROR) << "OnNativeImageFrameAvailable NotifyDstroyed, will not "
                      "MarkTextureFrameAvailable";
    return;
  }

  // Note: RunNowOrPostTask may get dead lock when running in platform thread.
  platform->task_runners_.GetPlatformTaskRunner()->PostTask([ptexture_id]() {
    // std::lock_guard<std::mutex> lock(g_map_mutex); //TODOTEST
    if (g_texture_platformview_map.find(ptexture_id) ==
        g_texture_platformview_map.end()) {
      return;
    }
    PlatformViewOHOS* platform = g_texture_platformview_map[ptexture_id];
    uint64_t texture_id = ptexture_id - (uint64_t)platform;
    platform->MarkTextureFrameAvailable(texture_id);
  });
}

std::shared_ptr<OHOSExternalTexture> PlatformViewOHOS::CreateExternalTexture(
  int64_t texture_id) {
  uint64_t context_frame_data = (uint64_t)this + (uint64_t)texture_id;
  OH_OnFrameAvailableListener listener;
  listener.context = (void*)context_frame_data;
  listener.onFrameAvailable = OnNativeImageFrameAvailable;
  std::shared_ptr<OHOSExternalTextureGL> extrenal_texture = nullptr;
  FML_LOG(INFO) << " RegisterExternalTexture api type "
                << int(ohos_context_->RenderingApi()) << " texture_id "
                << texture_id;
  if (ohos_context_->RenderingApi() == OHOSRenderingAPI::kOpenGLES) {
    extrenal_texture =
        std::make_shared<OHOSExternalTextureGL>(texture_id, listener);
  }
  // else if (ohos_context_->RenderingApi() ==
  //           OHOSRenderingAPI::kImpellerVulkan) {
  //   extrenal_texture = std::make_shared<OHOSExternalTextureVulkan>(
  //       std::static_pointer_cast<impeller::ContextVK>(
  //           ohos_context_->GetImpellerContext()),
  //       texture_id, listener);
  // }
  if (extrenal_texture && extrenal_texture->GetProducerSurfaceId() != 0 &&
      extrenal_texture->GetProducerWindowId() != 0) {
    //std::lock_guard<std::mutex> lock(g_map_mutex); //TODOTEST
    g_texture_platformview_map[context_frame_data] = this;
    //all_external_texture_[texture_id] = extrenal_texture;
    external_texture_gl_[texture_id] = extrenal_texture;
    RegisterTexture(extrenal_texture);
  }
  return extrenal_texture;
}

uint64_t PlatformViewOHOS::RegisterExternalTexture(int64_t texture_id) {
  auto extrenal_texture = CreateExternalTexture(texture_id);
  if (extrenal_texture == nullptr) {
    return 0;
  } else {
    return extrenal_texture->GetProducerSurfaceId();
  }
  return 0;
}


void PlatformViewOHOS::SetTextureBufferSize(
    int64_t texture_id,
    int32_t width,
    int32_t height)
{
  if (ohos_context_->RenderingApi() == OHOSRenderingAPI::kOpenGLES) {
    auto iter = external_texture_gl_.find(texture_id);
    if (iter != external_texture_gl_.end()) {
      iter->second->SetProducerWindowSize(width, height);
    }
  }
}

void PlatformViewOHOS::NotifyTextureResizing(int64_t texture_id,
    int32_t width,
    int32_t height) {
  FML_LOG(INFO) << "PlatformViewOHOS::NotifyTextureResizing";
  if (external_texture_gl_.find(texture_id) != external_texture_gl_.end()) {
    auto external_texture = external_texture_gl_[texture_id];
    external_texture->NotifyResizing(width, height);
  }
}

void PlatformViewOHOS::UnRegisterExternalTexture(int64_t texture_id) {
  FML_DLOG(INFO) << "PlatformViewOHOS::UnRegisterExternalTexture, texture_id="
                 << texture_id;
  external_texture_gl_.erase(texture_id);
  UnregisterTexture(texture_id);
  g_texture_platformview_map.erase((uint64_t)this + (uint64_t)texture_id);
}

void PlatformViewOHOS::RegisterExternalTextureByPixelMap(
    int64_t texture_id,
    NativePixelMap* pixelMap) {
  auto extrenal_texture = CreateExternalTexture(texture_id);
  if (extrenal_texture != nullptr) {
    extrenal_texture->SetPixelMapAsProducer(pixelMap, nullptr); // pixelMap_native_buffer); //TODOTEST
  }
  // MarkTextureFrameAvailable(texture_id); //TODOTEST
}

void PlatformViewOHOS::SetExternalTextureBackGroundPixelMap(
    int64_t texture_id,
    NativePixelMap* pixelMap) {
  if (ohos_context_->RenderingApi() == OHOSRenderingAPI::kOpenGLES) {
    auto iter = external_texture_gl_.find(texture_id);
    if (iter != external_texture_gl_.end()) {
      // iter->second->DispatchBackGroundPixelMap(pixelMap); //TODOTEST
    }
  }
}

void PlatformViewOHOS::SetExternalTextureBackGroundColor(
    int64_t texture_id,
    uint32_t color) {
  if (ohos_context_->RenderingApi() == OHOSRenderingAPI::kOpenGLES) {
    auto iter = external_texture_gl_.find(texture_id);
    if (iter != external_texture_gl_.end()) {
      // iter->second->DispatchBackGroundColor(color); //TODOTEST
    }
  }
}

void PlatformViewOHOS::OnTouchEvent(
    const std::shared_ptr<std::string[]> touchPacketString,
    int size) {
  return napi_facade_->FlutterViewOnTouchEvent(touchPacketString, size);
}

void PlatformViewOHOS::RunTask(OHOS_THREAD_TYPE type, const fml::closure& task)
{
  fml::RefPtr<fml::TaskRunner> TaskRunnerPtr = nullptr;
  switch (type) {
    case OHOS_THREAD_TYPE::OHOS_THREAD_TYPE_PLATFORM:
      TaskRunnerPtr = task_runners_.GetPlatformTaskRunner();
      break;
    case OHOS_THREAD_TYPE::OHOS_THREAD_TYPE_UI:
      TaskRunnerPtr = task_runners_.GetUITaskRunner();
      break;
    case OHOS_THREAD_TYPE::OHOS_THREAD_TYPE_RASTER:
      TaskRunnerPtr = task_runners_.GetRasterTaskRunner();
      break;
    case OHOS_THREAD_TYPE::OHOS_THREAD_TYPE_IO:
      TaskRunnerPtr = task_runners_.GetIOTaskRunner();
      break;
    default:
      break;
  }

  if (!TaskRunnerPtr) {
    return;
  }

  fml::TaskRunner::RunNowOrPostTask(TaskRunnerPtr, task);
}

}  // namespace flutter
