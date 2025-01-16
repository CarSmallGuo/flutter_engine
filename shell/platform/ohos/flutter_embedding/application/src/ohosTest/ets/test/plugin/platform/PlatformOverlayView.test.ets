/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

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