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

import { describe, it, expect, beforeAll } from '@ohos/hypium';
import { FlutterManager } from "@ohos/flutter_ohos";
import { window } from '@kit.ArkUI';
import { FullScreenListener } from '@ohos/flutter_ohos/src/main/ets/embedding/ohos/FlutterManager';

function sleep(time: number) {
  return new Promise<void>((resolve: Function) => setTimeout(resolve, time));
}

const flutterManager: FlutterManager = FlutterManager.getInstance();
const uiAbility = flutterManager.getUIAbility(getContext());

export default function FlutterManagerTest() {
  describe('FlutterManagerTest', () => {
    beforeAll(async () => {
      const windowStage = flutterManager.getWindowStage(uiAbility);
      await sleep(1000);
      flutterManager.pushWindowStage(uiAbility, windowStage);
      await sleep(1000);
      flutterManager.popWindowStage(uiAbility);
      await sleep(1000);
      flutterManager.pushUIAbility(uiAbility);
      await sleep(1000);
      flutterManager.popUIAbility(uiAbility);

    })

    it('TestGetFlutterViewList', 0, async (done: Function) => {
      const result = flutterManager.getFlutterViewList();
      expect(result).not().assertNull();
      done();
    })

    it('TestSetFullScreenListener', 0, async (done: Function) => {
      class MyFullScreenListener implements FullScreenListener {
        useFullScreen(): boolean {
          return true;
        }

        setUseFullScreen(useFullScreen: boolean): void {
          console.info("uitest: setUseFullScreen useFullScreen " + useFullScreen);
        }

        onScreenStateChanged(data: window.WindowStatusType): void {
          console.info("uitest: onScreenStateChanged");
        }
      }

      const listener: FullScreenListener = new MyFullScreenListener();
      flutterManager.setFullScreenListener(listener);
      await sleep(1000);
      const result = flutterManager.getFullScreenListener();
      expect(result).not().assertNull();
      done();
    })

    it('TestFullScreen', 0, async (done: Function) => {
      const result = flutterManager.useFullScreen();
      expect(result).assertTrue();
      flutterManager.setUseFullScreen(result);
      flutterManager.clear();
      done();
    })
  })
}