// Copyright (C) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
import { describe, expect, it } from '@ohos/hypium';
import FlutterEngineGroupCache from '@ohos/flutter_ohos/src/main/ets/embedding/engine/FlutterEngineGroupCache';
import { FlutterEngineGroup } from '@ohos/flutter_ohos';

export default function FlutterEngineGroupCacheTest() {
  const flutterEngineGroupCache = new FlutterEngineGroupCache();
  const engineGroupId: string = '';

  describe('FlutterEngineGroupCacheTest', () => {

    it('instance', 0, async (done: Function) => {
      console.info("uitest: instance begin");
      const instance = FlutterEngineGroupCache.instance;
      expect(instance).not().assertNull();
      done();
    })

    it('contains', 0, async (done: Function) => {
      console.info("uitest: contains begin");
      const flag = flutterEngineGroupCache.contains(engineGroupId);
      expect(flag).not().assertNull();
      done();
    })

    it('get', 0, async (done: Function) => {
      console.info("uitest: get begin");
      const engGroupId: FlutterEngineGroup | null = flutterEngineGroupCache.get(engineGroupId);
      if (engGroupId) {
        expect(engGroupId).not().assertNull();
      }
      done();
    })
  })
}