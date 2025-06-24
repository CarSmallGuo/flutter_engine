
import { beforeAll, describe, it } from '@ohos/hypium';
import { FlutterPluginRegistry, FlutterView } from '@ohos/flutter_ohos';
import { common } from '@kit.AbilityKit';

export default function FlutterPluginRegistryTest() {
  let context: common.UIAbilityContext;
  let flutterView: FlutterView | null = null;
  let flutterPluginRegistry: FlutterPluginRegistry;

  describe('FlutterPluginRegistryTest', () => {
    beforeAll(() => {
      context = getContext() as common.UIAbilityContext;
      flutterPluginRegistry = new FlutterPluginRegistry();
    })

    it('attach', 0, () => {
      flutterView = new FlutterView('testId', context);
      flutterPluginRegistry.attach(flutterView, context);
    })

    it('detach', 0, () => {
      flutterPluginRegistry.detach();
    })

    it('destroy', 0, () => {
      flutterPluginRegistry.destroy();
    })

    it('onPreEngineRestart', 0, () => {
      flutterPluginRegistry.onPreEngineRestart();
    })
  })
}