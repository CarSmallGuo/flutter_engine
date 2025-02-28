

import { describe, expect, it } from "@ohos/hypium";
import { resourceManager } from "@kit.LocalizationKit";
import { DartExecutor, FlutterNapi, TextInputChannel } from "@ohos/flutter_ohos";

export default function TextInputChannelUnitTest() {
  describe('TextInputChannel', () => {
    it("TextInputChannel_createEditingStateJSON", 0, createEditingStateJSON);
  })
}

function createEditingStateJSON() {
  let textInputChannel = getTextInputChannel();
  let result = textInputChannel.createEditingStateJSON("test", 0, 1, 0, 2);
  expect(result).assertInstanceOf("Object");
}

function getTextInputChannel(): TextInputChannel {
  let flutterNapi = new FlutterNapi();
  let manager: resourceManager.ResourceManager = getContext().resourceManager;
  let dartExecutor = new DartExecutor(flutterNapi, manager);
  let channel = new TextInputChannel(dartExecutor);
  return channel;
}
