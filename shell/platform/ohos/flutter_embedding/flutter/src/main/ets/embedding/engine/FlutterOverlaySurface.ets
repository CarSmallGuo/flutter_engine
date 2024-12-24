// Copyright (C) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

export class FlutterOverlaySurface {

  private id: number;

  constructor(id: number) {
    this.id = id
  }

  getId(): number {
    return this.id;
  }
}