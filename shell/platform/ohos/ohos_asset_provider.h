/*
 * Copyright 2013 The Flutter Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef OHOS_ASSET_PROVIDER_H
#define OHOS_ASSET_PROVIDER_H

#include "flutter/assets/asset_resolver.h"
#include "flutter/fml/memory/ref_counted.h"

namespace flutter {

// ohos平台的文件管理 ，必须通过NativeResourceManager* 指针对它进行初始化
class OHOSAssetProvider final : public AssetResolver {
 public:
  explicit OHOSAssetProvider(void* assetHandle,
                             const std::string& dir = "flutter_assets");
  ~OHOSAssetProvider() = default;

  std::unique_ptr<OHOSAssetProvider> Clone() const;

 private:
  void* assetHandle_;
  std::string dir_;

  bool IsValid() const override;

  bool IsValidAfterAssetManagerChange() const override;

  AssetResolver::AssetResolverType GetType() const override;

  std::unique_ptr<fml::Mapping> GetAsMapping(
      const std::string& asset_name) const override;
};
}  // namespace flutter
#endif