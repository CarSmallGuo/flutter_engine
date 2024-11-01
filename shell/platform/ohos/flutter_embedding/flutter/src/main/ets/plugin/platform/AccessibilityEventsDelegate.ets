// Copyright (c) 2023 Hunan OpenValley Digital Industry Development Co., Ltd.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import AccessibilityBridge from '../../view/AccessibilityBridge';

export class AccessibilityEventsDelegate {
  private accessibilityBridge: AccessibilityBridge | null = null;

  requestSendAccessibilityEvent(accessibilityBridge: AccessibilityBridge): boolean {
    if (accessibilityBridge == null) {
      return false;
    }
    return true;
  }

  onAccessibilityHoverEvent(accessibilityBridge: AccessibilityBridge): boolean {
    if (accessibilityBridge == null) {
      return false;
    }
    return true;
  }

  setAccessibilityBridge (accessibilityBridge: AccessibilityBridge): void {
    this.accessibilityBridge = accessibilityBridge;
  }
}