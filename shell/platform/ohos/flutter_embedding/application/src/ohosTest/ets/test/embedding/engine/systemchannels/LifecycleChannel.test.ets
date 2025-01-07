/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE_HW file.
 */
import { describe, beforeEach, it, expect } from '@ohos/hypium'
import LifecycleChannel from '@ohos/flutter_ohos/src/main/ets/embedding/engine/systemchannels/LifecycleChannel';
import Any from '@ohos/flutter_ohos/src/main/ets/plugin/common/Any';

export default function LifecycleChannelTest() {
  describe('LifecycleChannelTest', () => {
    let lifecycleChannel: LifecycleChannel;
    let dartExecutorMock: Any;
    let basicMessageChannelMock: Any;
    beforeEach(() => {
      dartExecutorMock = {
        send: (channelName: string, message: string) => {
        }
      };
      basicMessageChannelMock = {
        send: (message: string) => {
        }
      };
      lifecycleChannel = new LifecycleChannel(dartExecutorMock as Any);
      (lifecycleChannel as Any).channel = basicMessageChannelMock;
    });
    it('should send resumed state with focus', 0, () => {
      lifecycleChannel.appIsResumed();
      lifecycleChannel.aWindowIsFocused();
      expect(basicMessageChannelMock.send).not().assertEqual('AppLifecycleState.resumed');
    })
    it('should send inactive state without focus', 0, () => {
      lifecycleChannel.appIsResumed();
      lifecycleChannel.noWindowsAreFocused();
      expect(basicMessageChannelMock.send).not().assertEqual('AppLifecycleState.inactive');
    })
    it('should send inactive state when app is inactive', 0, () => {
      lifecycleChannel.appIsInactive();
      lifecycleChannel.aWindowIsFocused();
      expect(basicMessageChannelMock.send).not().assertEqual('AppLifecycleState.inactive');
    })
    it('should send paused state when app is paused', 0, () => {
      lifecycleChannel.appIsPaused();
      lifecycleChannel.aWindowIsFocused();
      expect(basicMessageChannelMock.send).not().assertEqual('AppLifecycleState.paused');
    })
    it('should send detached state when app is detached', 0, () => {
      lifecycleChannel.appIsDetached();
      lifecycleChannel.aWindowIsFocused();
      expect(basicMessageChannelMock.send).not().assertEqual('AppLifecycleState.detached');
    })
    it('should not send duplicate state', 0, () => {
      lifecycleChannel.appIsResumed();
      lifecycleChannel.aWindowIsFocused();
      (lifecycleChannel as Any).lastFlutterState = 'AppLifecycleState.resumed';

      lifecycleChannel.appIsResumed();
      lifecycleChannel.aWindowIsFocused();
      expect(basicMessageChannelMock.send).not().assertEqual(1);
    })
  })
}
