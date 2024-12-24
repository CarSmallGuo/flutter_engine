// Copyright (C) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import { describe, it, expect } from '@ohos/hypium'
import { AccessibilityEventsDelegate, PlatformOverlayView } from '@ohos/flutter_ohos';

const platformOverlayView: PlatformOverlayView =
  new PlatformOverlayView(getContext(), 800, 800, new AccessibilityEventsDelegate())

export default function PlatformOverlayViewTest() {
  describe('PlatformOverlayViewTest', () => {
    it('PlatformOverlayView', 0, () => {
      let overlayLayerViews = new Map<number, PlatformOverlayView>();
      expect(overlayLayerViews).not().assertNull();
    });
    it('onHoverEvent', 0, () => {
      expect(platformOverlayView.onHoverEvent()).assertFalse();
    })
  })
}