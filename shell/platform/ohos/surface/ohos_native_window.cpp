/*
 * Copyright 2013 The Flutter Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include "flutter/shell/platform/ohos/surface/ohos_native_window.h"
#include "flutter/fml/logging.h"

namespace flutter {

OHOSNativeWindow::OHOSNativeWindow(Handle window)
    : window_(window), is_fake_window_(false) {
  FML_LOG(INFO) << " native_window:" << (int64_t)window_;
}

OHOSNativeWindow::~OHOSNativeWindow() {
  if (window_ != nullptr) {
    window_ = nullptr;
  }
}

bool OHOSNativeWindow::IsValid() const {
  return window_ != nullptr;
}

SkISize OHOSNativeWindow::GetSize() const {
  if (window_ != nullptr) {
    return SkISize::Make(width_, height_);
  }

  return SkISize::Make(0, 0);
}

void OHOSNativeWindow::SetWindowHeight(double height) {
  height_ = height;
}

void OHOSNativeWindow::SetWindowWidth(double width) {
  width_ = width;
}

bool OHOSNativeWindow::GetWindowWithAndHeight(double* height, double* width) {
  if (window_ == nullptr) {
    return false;
  }

  *height = height_;
  *width = width_;

  return true;
}

OHOSNativeWindow::Handle OHOSNativeWindow::Gethandle() const {
  return window_;
}

OHOSNativeWindow::Handle OHOSNativeWindow::handle() const {
  return window_;
}

}  // namespace flutter
