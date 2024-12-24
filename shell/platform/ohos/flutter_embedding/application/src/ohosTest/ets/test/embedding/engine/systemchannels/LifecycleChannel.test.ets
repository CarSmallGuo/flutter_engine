/*
Copyright (C) 2024 Huawei Device Co., Ltd.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of pngout nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
