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

import { describe, it, expect } from '@ohos/hypium';
import { FlutterEngine, FlutterPlugin, FlutterEntry } from "@ohos/flutter_ohos";
import { FlutterPluginBinding } from '@ohos/flutter_ohos/src/main/ets/embedding/engine/plugins/FlutterPlugin';

function sleep(time: number) {
  return new Promise<void>((resolve: Function) => setTimeout(resolve, time));
}

export default function FlutterEntryTest() {
  describe('FlutterEntryTest', () => {

    it('TestGetFlutterView', 0, async (done: Function) => {
      const flutterEntry: FlutterEntry = new FlutterEntry(getContext());
      const result = flutterEntry.getFlutterView();
      expect(result).assertNull();
      done();
    })

    it('TestGetFlutterEngine', 0, async (done: Function) => {
      const flutterEntry: FlutterEntry = new FlutterEntry(getContext());
      const result = flutterEntry.getFlutterEngine();
      expect(result).not().assertNull();
      done();
    })

    it('TestShouldDispatchAppLifecycleState', 0, async (done: Function) => {
      const flutterEntry: FlutterEntry = new FlutterEntry(getContext());
      const result = flutterEntry.shouldDispatchAppLifecycleState();
      expect(result).assertTrue();
      done();
    })

    it('TestProvideFlutterEngine', 0, async (done: Function) => {
      const flutterEntry: FlutterEntry = new FlutterEntry(getContext());
      const result = flutterEntry.provideFlutterEngine(getContext());
      expect(result).assertNull();
      done();
    })

    it('TestProvidePlatformPlugin', 0, async (done: Function) => {
      const flutterEntry: FlutterEntry = new FlutterEntry(getContext());
      const flutterEngine = new FlutterEngine(getContext(), null, null, null);
      const result = flutterEntry.providePlatformPlugin(flutterEngine);
      expect(result).not().assertNull();
      done();
    })

    it('TestGetFlutterShellArgs', 0, async (done: Function) => {
      const flutterEntry: FlutterEntry = new FlutterEntry(getContext());
      const result = flutterEntry.getFlutterShellArgs();
      expect(result).not().assertNull();
      done();
    })

    it('TestGetDartEntrypointArgs', 0, async (done: Function) => {
      const flutterEntry: FlutterEntry = new FlutterEntry(getContext());
      const result = flutterEntry.getDartEntrypointArgs();
      expect(result).not().assertNull();
      done();
    })

    it('TestPopSystemNavigator', 0, async (done: Function) => {
      const flutterEntry: FlutterEntry = new FlutterEntry(getContext());
      const result = flutterEntry.popSystemNavigator();
      expect(result).assertFalse();
      done();
    })

    it('TestShouldAttachEngineToAbility', 0, async (done: Function) => {
      const flutterEntry: FlutterEntry = new FlutterEntry(getContext());
      const result = flutterEntry.shouldAttachEngineToAbility();
      expect(result).assertUndefined();
      done();
    })

    it('TestGetDartEntrypointLibraryUri', 0, async (done: Function) => {
      const flutterEntry: FlutterEntry = new FlutterEntry(getContext());
      const result = flutterEntry.getDartEntrypointLibraryUri();
      expect(result).assertEqual("");
      done();
    })

    it('TestGetAppBundlePath', 0, async (done: Function) => {
      const flutterEntry: FlutterEntry = new FlutterEntry(getContext());
      const result = flutterEntry.getAppBundlePath();
      expect(result).assertEqual("");
      done();
    })

    it('TestGetDartEntrypointFunctionName', 0, async (done: Function) => {
      const flutterEntry: FlutterEntry = new FlutterEntry(getContext());
      const result = flutterEntry.getDartEntrypointFunctionName();
      expect(result).not().assertNull();
      done();
    })

    it('TestGetInitialRoute', 0, async (done: Function) => {
      const flutterEntry: FlutterEntry = new FlutterEntry(getContext());
      const result = flutterEntry.getInitialRoute();
      expect(result).not().assertNull();
      done();
    })

    it('TestAttachToEngineAutomatically', 0, async (done: Function) => {
      const flutterEntry: FlutterEntry = new FlutterEntry(getContext());
      const result = flutterEntry.attachToEngineAutomatically();
      expect(result).assertTrue();
      done();
    })

    it('TestShouldRestoreAndSaveState', 0, async (done: Function) => {
      const flutterEntry: FlutterEntry = new FlutterEntry(getContext());
      const result = flutterEntry.shouldRestoreAndSaveState();
      expect(result).not().assertNull();
      done();
    })

    it('TestGetExclusiveAppComponent', 0, async (done: Function) => {
      const flutterEntry: FlutterEntry = new FlutterEntry(getContext());
      const result = flutterEntry.getExclusiveAppComponent();
      expect(result).assertNull();
      done();
    })

    it('TestGetCachedEngineId', 0, async (done: Function) => {
      const flutterEntry: FlutterEntry = new FlutterEntry(getContext());
      const result = flutterEntry.getCachedEngineId();
      expect(result).assertUndefined();
      done();
    })

    it('TestGetCachedEngineGroupId', 0, async (done: Function) => {
      const flutterEntry: FlutterEntry = new FlutterEntry(getContext());
      const result = flutterEntry.getCachedEngineGroupId();
      expect(result).assertUndefined();
      done();
    })

    it('TestAddPlugin', 0, async (done: Function) => {
      const flutterEntry: FlutterEntry = new FlutterEntry(getContext());
      flutterEntry.addPlugin(plugin);
      done();
    })

    it('TestRemovePlugin', 0, async (done: Function) => {
      const flutterEntry: FlutterEntry = new FlutterEntry(getContext());
      flutterEntry.removePlugin(plugin);
      done();
    })
  })
}

class TestFlutterPlugin implements FlutterPlugin {
  getUniqueClassName(): string {
    return "engine test";
  }

  onAttachedToEngine(binding: FlutterPluginBinding): void {
    console.info("uitest: onAttachedToEngine");
  }

  onDetachedFromEngine(binding: FlutterPluginBinding): void {
    console.info("uitest: onAttachedToEngine");
  }
}

let plugin: FlutterPlugin = new TestFlutterPlugin();
