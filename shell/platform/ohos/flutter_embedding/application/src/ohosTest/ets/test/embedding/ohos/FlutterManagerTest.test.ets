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
import { describe, it, expect } from '@ohos/hypium';
import { FlutterView, FlutterManager } from "@ohos/flutter_ohos";
import { window } from '@kit.ArkUI';
import { FullScreenListener } from '@ohos/flutter_ohos/src/main/ets/embedding/ohos/FlutterManager';

function sleep(time: number) {
  return new Promise<void>((resolve: Function) => setTimeout(resolve, time));
}

const flutterManager: FlutterManager = FlutterManager.getInstance();
const uiAbility = flutterManager.getUIAbility(getContext());

export default function FlutterManagerTest() {
  describe('FlutterManagerTest', () => {
    it('TestUIAbility', 0, async (done: Function) => {
      flutterManager.pushUIAbility(uiAbility);
      done();
    })

    it('TestPopUIAbility', 0, async (done: Function) => {
      console.info("uitest: TestPopUIAbility begin");
      flutterManager.popUIAbility(uiAbility);
      done();
    })

    it('TestWindowStage', 0, async (done: Function) => {
      const windowStage = flutterManager.getWindowStage(uiAbility);
      await sleep(1000);
      flutterManager.pushWindowStage(uiAbility, windowStage);
      await sleep(1000);
      flutterManager.popWindowStage(uiAbility);
      done();
    })

    it('TestCreateFlutterView', 0, async (done: Function) => {
      await sleep(1000);
      let flutterViewA: FlutterView = flutterManager.createFlutterView(getContext());
      expect(flutterViewA).not().assertNull();
      const viewId = flutterViewA.getId();
      expect(viewId).not().assertNull();
      await sleep(1000);
      let flutterViewB = flutterManager.getFlutterView(viewId);
      expect(flutterViewB).not().assertNull();
      done();
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
      done();
    })

    it('TestClear', 0, async (done: Function) => {
      flutterManager.clear();
      done();
    })
  })

}
