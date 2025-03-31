// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FLUTTER_LIB_UI_PAINTING_SET_HDR_H_
#define FLUTTER_LIB_UI_PAINTING_SET_HDR_H_

#include "flutter/lib/ui/dart_wrapper.h"
#include "third_party/tonic/typed_data/typed_list.h"

namespace tonic {
class DartLibraryNatives;
}  // namespace tonic

namespace flutter {

class SetHdr2 : public RefCountedDartWrappable<SetHdr2> {
  DEFINE_WRAPPERTYPEINFO();
  FML_FRIEND_MAKE_REF_COUNTED(SetHdr2);

 public:
  
  ~SetHdr2();
  static void initSetHdr(int hdr, bool is_image);
  static void Create(Dart_Handle wrapper);

  static void RegisterNatives(tonic::DartLibraryNatives* natives);

 private:
  SetHdr2();
};

}  // namespace flutter

#endif  // FLUTTER_LIB_UI_PAINTING_SET_HDR_H_
