
import { describe, expect, it } from '@ohos/hypium';
import {
  BinaryMessageHandler,
  BinaryMessenger,
  BinaryReply,
  TaskQueue,
  TaskQueueOptions
} from '@ohos/flutter_ohos/src/main/ets/plugin/common/BinaryMessenger';

export default function BinaryMessengerTest() {
  let binaryMessenger: CurBinaryMessenger = new CurBinaryMessenger();

  describe('BinaryMessengerTest', () => {

    it('makeBackgroundTaskQueue', 0, async (done: Function) => {
      console.info("uitest: makeBackgroundTaskQueue begin");
      const options = new TaskQueueOptions();
      const taskQueue: TaskQueue = binaryMessenger.makeBackgroundTaskQueue(options);
      expect(taskQueue).not().assertNull();
      done();
    })
  })
}

class CurBinaryMessenger implements BinaryMessenger {
  makeBackgroundTaskQueue(options?: TaskQueueOptions | undefined): TaskQueue {
    return {};
  }

  send(channel: String, message: ArrayBuffer, callback?: BinaryReply | null | undefined) {

  }

  setMessageHandler(channel: String, handler: BinaryMessageHandler | BinaryMessageHandler | null, taskQueue?: TaskQueue,
    ...args: Object[]) {
  }
}