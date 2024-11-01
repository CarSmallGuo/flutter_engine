// Copyright (c) 2023 Hunan OpenValley Digital Industry Development Co., Ltd.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import FlutterEngine from '../engine/FlutterEngine';

export default interface FlutterEngineConfigurator {

  configureFlutterEngine: (flutterEngine: FlutterEngine) => void;

  cleanUpFlutterEngine: (flutterEngine: FlutterEngine) => void;
}