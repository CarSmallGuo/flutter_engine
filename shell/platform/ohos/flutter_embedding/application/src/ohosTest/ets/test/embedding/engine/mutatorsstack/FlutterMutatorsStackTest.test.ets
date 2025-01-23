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


import common from '@ohos.app.ability.common';
import {
  FlutterMutatorsStack,
  FlutterMutator
} from '@ohos/flutter_ohos/src/main/ets/embedding/engine/mutatorsstack/FlutterMutatorsStack'
import { describe,it, expect } from '@ohos/hypium';
import matrix4 from '@ohos.matrix4'
export default function FlutterMutatorsStackTest() {
  describe('FlutterMutatorsStackTest', () => {
    it('TestPushClipPath', 0, async (done: Function) => {
      let flutterMutatorStack = new FlutterMutatorsStack();
      const width = 100;
      const height = 200;
      const command = 'M 10 10 L 90 90';
      flutterMutatorStack.pushClipPath(width, height, command);
      expect(flutterMutatorStack.getMutators().length).assertEqual(1);
      expect(flutterMutatorStack.getFinalClippingPaths().length).assertEqual(1);
      done();
    })

    it('TestPushClipRect1', 0, async (done: Function) => {
      let flutterMutatorStack = new FlutterMutatorsStack();
      const width = 100;
      const height = 200;
      const radius = 10;
      flutterMutatorStack.pushClipRect(width, height, radius);
      expect(flutterMutatorStack.getMutators().length).assertEqual(1);
      done();
    })

    it('TestPushClipRect2', 0, async (done: Function) => {
      let flutterMutatorStack = new FlutterMutatorsStack();
      const width = 100;
      const height = 200;
      const radius = 10;
      flutterMutatorStack.pushClipRect(width, height, radius);
      expect(flutterMutatorStack.getFinalClippingRects().length).assertEqual(1);
      done();
    })

    it('TestPushTransform1', 0, async (done: Function) => {
      const values = [1, 0, 0, 0, 0, -1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1];
      let flutterMutatorStack = new FlutterMutatorsStack();
      flutterMutatorStack.pushTransform(values);
      expect(flutterMutatorStack.getMutators().length).assertEqual(1);
      done();
    })

    it('TestPushTransform2', 0, async (done: Function) => {
      const values = [1, 0, 0, 0, 0, -1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1];
      let flutterMutatorStack = new FlutterMutatorsStack();
      flutterMutatorStack.pushTransform(values);
      expect(flutterMutatorStack.getMutators().length).assertEqual(1);
      done();
    })

    it('TestFlutterMutatorsStack1', 0, async (done: Function) => {
      let flutterMutatorStack = new FlutterMutatorsStack();
      expect(flutterMutatorStack.getMutators().length).assertEqual(0);
      done();
    })

    it('TestFlutterMutatorsStack2', 0, async (done: Function) => {
      let flutterMutatorStack = new FlutterMutatorsStack();
      expect(flutterMutatorStack.getFinalClippingPaths().length).assertEqual(0);
      done();
    })

    it('TestFlutterMutatorsStack3', 0, async (done: Function) => {
      let flutterMutatorStack = new FlutterMutatorsStack();
      expect(flutterMutatorStack.getFinalClippingRects().length).assertEqual(0);
      done();
    })
  })
}
