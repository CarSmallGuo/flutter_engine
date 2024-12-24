// Copyright (C) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#ifndef OHOS_TYPES_H
#define OHOS_TYPES_H
namespace flutter {

constexpr int PIXEL_FMT_RGBA_8888 = 12;

enum Locales {
  LANGUAGE_INDEX = 0,
  REGION_INDEX,
  SCRIPT_INDEX,
};

using OHOS_SurfaceBufferUsage = enum {
  BUFFER_USAGE_CPU_READ = (1ULL << 0),  /**< CPU read buffer */
  BUFFER_USAGE_CPU_WRITE = (1ULL << 1), /**< CPU write memory */
  BUFFER_USAGE_MEM_MMZ = (1ULL << 2),   /**< Media memory zone (MMZ) */
  BUFFER_USAGE_MEM_DMA = (1ULL << 3), /**< Direct memory access (DMA) buffer */
  BUFFER_USAGE_MEM_SHARE = (1ULL << 4),     /**< Shared memory buffer*/
  BUFFER_USAGE_MEM_MMZ_CACHE = (1ULL << 5), /**< MMZ with cache*/
  BUFFER_USAGE_MEM_FB = (1ULL << 6),        /**< Framebuffer */
  BUFFER_USAGE_ASSIGN_SIZE = (1ULL << 7),   /**< Memory assigned */
  BUFFER_USAGE_HW_RENDER = (1ULL << 8),     /**< For GPU write case */
  BUFFER_USAGE_HW_TEXTURE = (1ULL << 9),    /**< For GPU read case */
  BUFFER_USAGE_HW_COMPOSER = (1ULL << 10),  /**< For hardware composer */
  BUFFER_USAGE_PROTECTED =
      (1ULL << 11), /**< For safe buffer case, such as DRM */
  BUFFER_USAGE_CAMERA_READ = (1ULL << 12),   /**< For camera read case */
  BUFFER_USAGE_CAMERA_WRITE = (1ULL << 13),  /**< For camera write case */
  BUFFER_USAGE_VIDEO_ENCODER = (1ULL << 14), /**< For encode case */
  BUFFER_USAGE_VIDEO_DECODER = (1ULL << 15), /**< For decode case */
  BUFFER_USAGE_VENDOR_PRI0 = (1ULL << 44),   /**< Reserverd for vendor */
  BUFFER_USAGE_VENDOR_PRI1 = (1ULL << 45),   /**< Reserverd for vendor */
  BUFFER_USAGE_VENDOR_PRI2 = (1ULL << 46),   /**< Reserverd for vendor */
  BUFFER_USAGE_VENDOR_PRI3 = (1ULL << 47),   /**< Reserverd for vendor */
  BUFFER_USAGE_VENDOR_PRI4 = (1ULL << 48),   /**< Reserverd for vendor */
  BUFFER_USAGE_VENDOR_PRI5 = (1ULL << 49),   /**< Reserverd for vendor */
  BUFFER_USAGE_VENDOR_PRI6 = (1ULL << 50),   /**< Reserverd for vendor */
  BUFFER_USAGE_VENDOR_PRI7 = (1ULL << 51),   /**< Reserverd for vendor */
  BUFFER_USAGE_VENDOR_PRI8 = (1ULL << 52),   /**< Reserverd for vendor */
  BUFFER_USAGE_VENDOR_PRI9 = (1ULL << 53),   /**< Reserverd for vendor */
  BUFFER_USAGE_VENDOR_PRI10 = (1ULL << 54),  /**< Reserverd for vendor */
  BUFFER_USAGE_VENDOR_PRI11 = (1ULL << 55),  /**< Reserverd for vendor */
  BUFFER_USAGE_VENDOR_PRI12 = (1ULL << 56),  /**< Reserverd for vendor */
  BUFFER_USAGE_VENDOR_PRI13 = (1ULL << 57),  /**< Reserverd for vendor */
  BUFFER_USAGE_VENDOR_PRI14 = (1ULL << 58),  /**< Reserverd for vendor */
  BUFFER_USAGE_VENDOR_PRI15 = (1ULL << 59),  /**< Reserverd for vendor */
  BUFFER_USAGE_VENDOR_PRI16 = (1ULL << 60),  /**< Reserverd for vendor */
  BUFFER_USAGE_VENDOR_PRI17 = (1ULL << 61),  /**< Reserverd for vendor */
  BUFFER_USAGE_VENDOR_PRI18 = (1ULL << 62),  /**< Reserverd for vendor */
  BUFFER_USAGE_VENDOR_PRI19 = (1ULL << 63),  /**< Reserverd for vendor */
};

}  // namespace flutter
#endif