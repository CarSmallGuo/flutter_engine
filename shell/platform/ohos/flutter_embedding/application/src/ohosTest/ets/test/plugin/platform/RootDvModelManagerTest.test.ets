/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE_HW file.
 */
import { describe, it, expect } from '@ohos/hypium';
import { PlatformViewWrapper } from '@ohos/flutter_ohos';
import { RootDvModeManager } from '@ohos/flutter_ohos/src/main/ets/plugin/platform/RootDvModelManager';
import { DVModel, DVModelContainer } from '@ohos/flutter_ohos/src/main/ets/view/DynamicView/dynamicView';

export default function DVModelContainerTest() {
  describe('DVModelContainerTest', () => {
    it('TestDVModelContainerGetRootDvMode', 0, async (done: Function) => {
      const dvModel: DVModelContainer = RootDvModeManager.getRootDvMode();
      expect(dvModel).not().assertNull();
      done();
    })

    it('TestDVModelAddDvModel', 0, async (done: Function) => {
      const platformViewWrapper: PlatformViewWrapper = new PlatformViewWrapper();
      const dvModel: DVModel = platformViewWrapper.getDvModel();
      RootDvModeManager.addDvModel(dvModel);
      expect(dvModel).not().assertNull();
      done();
    })
  })
}