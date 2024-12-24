// Copyright (C) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import FlutterEngine from '../engine/FlutterEngine';
import common from '@ohos.app.ability.common';

export default interface FlutterEngineProvider {
  provideFlutterEngine(context: common.Context): FlutterEngine | null;
}