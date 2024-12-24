// Copyright (C) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

/**
 * 基础component，还未封装，看情况是否使用
 */
@Component
export default struct FlutterComponent {
  build() {
    Row() {
      Column() {
        Text("xxx")
          .fontSize(50)
          .fontWeight(FontWeight.Bold)
      }
      .width('100%')
    }
    .height('100%')
  }
}