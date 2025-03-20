// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FLUTTER_SHELL_PLATFORM_OHOS_IMAGE_LRU_H_
#define FLUTTER_SHELL_PLATFORM_OHOS_IMAGE_LRU_H_

#include <array>
#include <cstddef>

#include "third_party/skia/include/core/SkImage.h"

namespace flutter {

// This value needs to be larger than the number of swapchain images
// that a typical image reader will produce to ensure that we effectively
// cache. If the value is too small, we will unnecessarily churn through
// images, while if it is too large we may retain images longer than
// necessary.
// OHOS' camera queue size is 8
// OHOS' video queue size is 9
#define K_MAX_QUEUE_SIZE 9

using NativeBufferKey = uint64_t;

class ImageLRU {
 public:
  ImageLRU() = default;

  ~ImageLRU() = default;

  /// @brief Retrieve the image associated with the given [key], or nullptr.
  sk_sp<SkImage> FindImage(NativeBufferKey key);

  /// @brief Add a new image to the cache with a key, returning the key of the
  ///        LRU entry that was removed.
  ///
  /// The value may be `0`, in which case nothing was removed.
  NativeBufferKey AddImage(const sk_sp<SkImage>& image,
                           NativeBufferKey key);

  /// @brief Remove all entires from the image cache.
  void Clear();

 private:
  /// @brief Marks [key] as the most recently used.
  void UpdateKey(const sk_sp<SkImage>& image, NativeBufferKey key);

  struct Data {
    NativeBufferKey key = 0u;
    sk_sp<SkImage> value;
  };

  std::array<Data, K_MAX_QUEUE_SIZE> images_;
};

}  // namespace flutter

#endif  // FLUTTER_SHELL_PLATFORM_OHOS_IMAGE_LRU_H_
