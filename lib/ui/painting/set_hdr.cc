// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "flutter/lib/ui/painting/set_hdr.h"
#include "flutter/impeller/renderer/context.h"
#include "flutter/lib/ui/painting/image_filter.h"
#include "flutter/impeller/renderer/context.h"
#include "flutter/lib/ui/painting/matrix.h"
#include "flutter/lib/ui/ui_dart_state.h"
#include "third_party/tonic/converter/dart_converter.h"
#include "third_party/tonic/dart_args.h"
#include "third_party/tonic/dart_binding_macros.h"
#include "third_party/tonic/dart_library_natives.h"


namespace flutter {

IMPLEMENT_WRAPPERTYPEINFO(ui, SetHdr2);

void SetHdr2::Create(Dart_Handle wrapper) {
  //UIDartState::ThrowIfUIOperationsProhibited();
  auto res = fml::MakeRefCounted<SetHdr2>();
  res->AssociateWithDartWrapper(wrapper);
}

void SetHdr2::initSetHdr(int hdr, bool is_image) {
  impeller::Context::is_image_ = is_image;
  FML_DLOG(ERROR) << "is_image= " << impeller::Context::is_image_;
  if (is_image) {
    if (hdr >= 0) {
      FML_DLOG(ERROR) << "is_image:" << hdr;
      impeller::Context::hdr_ = hdr;
    }
  }
}

SetHdr2::SetHdr2() {}
SetHdr2::~SetHdr2() {}

}  // namespace flutter