import { describe, expect, it } from '@ohos/hypium';
import { FlutterCallbackInformation } from '@ohos/flutter_ohos';

const TAG = 'FlutterCallbackInformationTest'

export default function FlutterCallbackInformationTest() {

  describe('FlutterCallbackInformationTest', () => {

    it('constructor', 0, () => {
      const flutterCallbackInformation = new FlutterCallbackInformation(TAG, TAG, TAG);
      expect(flutterCallbackInformation.callbackName).assertEqual(TAG);
      expect(flutterCallbackInformation.callbackClassName).assertEqual(TAG);
      expect(flutterCallbackInformation.callbackLibraryPath).assertEqual(TAG);
    })

    it('init', 0, () => {
      const flutterCallbackInformation = new FlutterCallbackInformation();
      flutterCallbackInformation.init(TAG, TAG, TAG);
      expect(flutterCallbackInformation.callbackName).assertEqual(TAG);
      expect(flutterCallbackInformation.callbackClassName).assertEqual(TAG);
      expect(flutterCallbackInformation.callbackLibraryPath).assertEqual(TAG);
    })

  })
}