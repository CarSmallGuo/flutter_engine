/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE_HW file.
 */
import { describe, it, expect } from '@ohos/hypium';
import {
  PlatformViewsController,
  FlutterOverlaySurface,
} from "@ohos/flutter_ohos";

const platformViewsController: PlatformViewsController = new PlatformViewsController();
const direction: Direction = 100;

export default function PlatformViewsControllerTest() {
  describe('PlatformViewsControllerTest', () => {

    it('TestPlatformViewRegistryImplGetRegistry', 0, async (done: Function) => {
      const result = platformViewsController.getRegistry();
      expect(result).not().assertNull();
      done();
    })

    it('TestCreateOverlaySurface', 0, async (done: Function) => {
      const result: FlutterOverlaySurface = platformViewsController.createOverlaySurface();
      expect(result).not().assertNull();
      done();
    })
    it('TestPlatformViewRegistryImplValidateDirection', 0, async (done: Function) => {
      const result = platformViewsController.validateDirection(direction);
      expect(result).assertFalse();
      done();
    })

    it('TestPlatformViewRegistryImplGetFlutterView', 0, async (done: Function) => {
      const result = platformViewsController.getFlutterView();
      expect(result).assertNull();
      done();
    })
  })
}