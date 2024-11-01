// Copyright (c) 2023 Hunan OpenValley Digital Industry Development Co., Ltd.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import { AccessibilityEventsDelegate } from './AccessibilityEventsDelegate';

export class PlatformOverlayView {
  private accessibilityEventsDelegate: AccessibilityEventsDelegate;

  constructor(context: Context, width: Number, height: Number, accessibilityEventsDelegate: AccessibilityEventsDelegate) {
    this.accessibilityEventsDelegate= accessibilityEventsDelegate;
  }

  public onHoverEvent(): boolean {
    return false;
  }
}