
import { AccessibilityBridge, AccessibilityEventsDelegate } from "@ohos/flutter_ohos";
import { describe, expect, it } from "@ohos/hypium";

export default function AccessibilityEventsDelegateUnitTest() {
  describe('AccessibilityEventsDelegate', () => {
    it("AccessibilityEventsDelegate_requestSendAccessibilityEvent", 0, requestSendAccessibilityEvent);

    it("AccessibilityEventsDelegate_onAccessibilityHoverEvent", 0, onAccessibilityHoverEvent);
  })
}

function requestSendAccessibilityEvent() {
  let delegate = new AccessibilityEventsDelegate();
  let result = delegate.requestSendAccessibilityEvent(new AccessibilityBridge());
  expect(result).assertTrue();
}

function onAccessibilityHoverEvent() {
  let delegate = new AccessibilityEventsDelegate();
  let result = delegate.onAccessibilityHoverEvent(new AccessibilityBridge());
  expect(result).assertTrue();
}