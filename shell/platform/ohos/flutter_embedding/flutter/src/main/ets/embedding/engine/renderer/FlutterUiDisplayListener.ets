/*
* Copyright (c) 2023 Hunan OpenValley Digital Industry Development Co., Ltd. All rights reserved.
* Use of this source code is governed by a BSD-style license that can be
* found in the LICENSE_KHZG file.
*
* Based on FlutterUiDisplayListener.java originally written by
* Copyright (C) 2019  Matt Carroll <matthew-carroll@users.noreply.github.com>
*
*/

export interface FlutterUiDisplayListener {
  onFlutterUiDisplayed(): void;

  onFlutterUiNoLongerDisplayed(): void;
}