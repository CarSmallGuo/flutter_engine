// Copyright (C) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
import { describe, it, expect } from '@ohos/hypium';
import  WindowInfoRepositoryCallbackAdapterWrapper    from '@ohos/flutter_ohos/src/main/ets/embedding/ohos/WindowInfoRepositoryCallbackAdapterWrapper';

export default function WindowInfoRepositoryCallbackAdapterWrapperTest() {

  describe('WindowInfoRepositoryCallbackAdapterWrapper', () => {

    it('WindowInfoRepositoryCallbackAdapterWrapper', 0, async (done: Function) => {
      console.info("uitest: WindowInfoRepositoryCallbackAdapterWrapper begin");
      const windowInfoRepositoryCallbackAdapterWrapper: WindowInfoRepositoryCallbackAdapterWrapper = new WindowInfoRepositoryCallbackAdapterWrapper();
      expect(windowInfoRepositoryCallbackAdapterWrapper).not().assertNull();
      done();
    })
  })
}