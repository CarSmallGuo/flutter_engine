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