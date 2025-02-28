

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
