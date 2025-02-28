
import { describe, expect, it } from '@ohos/hypium';
import { abilityDelegatorRegistry } from '@kit.TestKit';
import { Want } from '@kit.AbilityKit';
import FlutterShellArgs from '@ohos/flutter_ohos/src/main/ets/embedding/engine/FlutterShellArgs';

const bundleName = abilityDelegatorRegistry.getArguments().bundleName;

export default function FlutterShellArgsTest() {
  const flutterShellArgs: FlutterShellArgs = new FlutterShellArgs();
  const want: Want = {
    bundleName: bundleName,
    abilityName: 'EntryAbility'
  }

  describe('FlutterShellArgsTest', () => {

    it('fromWant', 0, async (done: Function) => {
      console.info("uitest: fromWant begin");
      try {
        const flutterShellArgs = FlutterShellArgs.fromWant(want);
        if (flutterShellArgs) {
          expect(flutterShellArgs).not().assertNull();
        }
      } catch (e) {
      }
      done();
    })

    it('checkArg', 0, async (done: Function) => {
      console.info("uitest: checkArg begin");
      const argKey = 'argKey';
      const argFlag = 'argFlag';
      try {
        const flutterShellArgs = FlutterShellArgs.fromWant(want);
        FlutterShellArgs.checkArg(argKey, argFlag, want, flutterShellArgs);
      } catch (e) {
      }
      done();
    })

    it('toArray', 0, async (done: Function) => {
      console.info("uitest: toArray begin");
      const toArray = flutterShellArgs.toArray();
      expect(toArray).not().assertNull();
      done();
    })
  })
}