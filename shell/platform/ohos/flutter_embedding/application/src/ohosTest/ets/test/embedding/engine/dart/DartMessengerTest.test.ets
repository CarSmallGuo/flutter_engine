// Copyright (C) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import { DartMessenger} from '@ohos/flutter_ohos/src/main/ets/embedding/engine/dart/DartMessenger'
import { describe,it, expect } from '@ohos/hypium';
import FlutterNapi from '@ohos/flutter_ohos/src/main/ets/embedding/engine/FlutterNapi'
import {
  TaskQueue,
} from '@ohos/flutter_ohos/src/main/ets/plugin/common/BinaryMessenger'
export default function DartMessengerTest() {
  describe('FlutterDartMessengerTest', () => {
    it('TestMakeBackgroundTaskQueue', 0, async (done: Function) => {
      const flutterNapi: FlutterNapi = new FlutterNapi();
      const dartMessage = new DartMessenger(flutterNapi);
      let taskQueue: TaskQueue = dartMessage.makeBackgroundTaskQueue();
      expect(taskQueue).not().assertNull();
      done();
    })

    it('TestGetPendingChannelResponseCount', 0, async (done: Function) => {
      const flutterNapi: FlutterNapi = new FlutterNapi();
      const messenger = new DartMessenger(flutterNapi);
      let count: number = messenger.getPendingChannelResponseCount();
      expect(count).assertEqual(0);
      done();
    })

    it('TestAskQueueFactory', 0, async (done: Function) => {
      const flutterNapi: FlutterNapi = new FlutterNapi();
      const messenger = new DartMessenger(flutterNapi);
      expect(messenger.taskQueueFactory).not().assertNull();
      done();
    })
  })
}