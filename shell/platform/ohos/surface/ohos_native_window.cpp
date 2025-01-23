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
