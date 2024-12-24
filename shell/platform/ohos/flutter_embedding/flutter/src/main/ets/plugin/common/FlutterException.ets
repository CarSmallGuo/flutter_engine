// Copyright (C) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
import Any from './Any';

export default class FlutterException implements Error {
  stack?: string;
  message: string;
  name: string = "";
  code: string;
  details: Any

  constructor(code: string, message: string, details: Any) {
    this.message = message;
    this.code = code;
    this.details =details;
  }
}