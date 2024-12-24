// Copyright (C) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef OHOS_NATIVE_WINDOW_H
#define OHOS_NATIVE_WINDOW_H

#include "flutter/fml/macros.h"
#include "flutter/fml/memory/ref_counted.h"
#include "third_party/skia/include/core/SkSize.h"

#include <native_window/external_window.h>

namespace flutter {

/*
  class adapater for ThreadSafe
*/
class OHOSNativeWindow : public fml::RefCountedThreadSafe<OHOSNativeWindow> {
 public:
  using Handle = OHNativeWindow*;

  Handle Gethandle() const;

  bool IsValid() const;

  SkISize GetSize() const;

  Handle handle() const;

  /// Returns true when this AndroidNativeWindow is not backed by a real window
  /// (used for testing).
  bool IsFakeWindow() const { return is_fake_window_; }

  void SetWindowHeight(double height);

  void SetWindowWidth(double width);

  bool GetWindowWithAndHeight(double* height, double* width);

 private:
  Handle window_;
  double height_;
  double width_;
  const bool is_fake_window_;

  explicit OHOSNativeWindow(Handle window);

  ~OHOSNativeWindow();

  FML_FRIEND_MAKE_REF_COUNTED(OHOSNativeWindow);
  FML_FRIEND_REF_COUNTED_THREAD_SAFE(OHOSNativeWindow);
  FML_DISALLOW_COPY_AND_ASSIGN(OHOSNativeWindow);
};
}  // namespace flutter

#endif