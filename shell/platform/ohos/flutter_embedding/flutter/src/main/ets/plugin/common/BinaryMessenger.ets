/*
* Copyright (c) 2023 Hunan OpenValley Digital Industry Development Co., Ltd. All rights reserved.
* Use of this source code is governed by a BSD-style license that can be
* found in the LICENSE_KHZG file.
*
* Based on BinaryMessenger.java originally written by
* Copyright (C) 2013 The Flutter Authors.
*
*/


import SendableBinaryMessageHandler from './SendableBinaryMessageHandler'

export interface TaskQueue {}

/**
 * The priority of task execution
 *
 * This priority is guaranteed to be compatible with
 * https://developer.huawei.com/consumer/cn/doc/harmonyos-references-V5/js-apis-taskpool-V5#priority.
 *
 */
export enum TaskPriority {
  HIGH = 0,
  MEDIUM = 1,
  LOW = 2,
  IDLE = 3
}

/** Options that control how a TaskQueue should operate and be created. */
export class TaskQueueOptions {
  private isSerial: boolean = true;
  private isSingleThread: boolean = false;
  private priority: TaskPriority = TaskPriority.MEDIUM;

  getIsSerial():boolean {
    return this.isSerial;
  }

  setIsSerial(isSerial: boolean): TaskQueueOptions {
    this.isSerial = isSerial;
    return this;
  }

  getPriority(): TaskPriority {
    return this.priority;
  }

  setPriority(priority: TaskPriority): TaskQueueOptions {
    this.priority = priority;
    return this;
  }

  isSingleThreadMode(): boolean {
    return this.isSingleThread;
  }

  setSingleThreadMode(isSingleThread: boolean): TaskQueueOptions {
    this.isSingleThread = isSingleThread;
    return this;
  }
}

/**
 * Binary message reply callback. Used to submit a reply to an incoming message from Flutter. Also
 * used in the dual capacity to handle a reply received from Flutter after sending a message.
 */
export interface BinaryReply {
  /**
   * Handles the specified reply.
   *
   * @param reply the reply payload, a direct-allocated {@link ByteBuffer} or null. Senders of
   *     outgoing replies must place the reply bytes between position zero and current position.
   *     Reply receivers can read from the buffer directly.
   */
  reply: (reply: ArrayBuffer | null) => void;
}

/** Handler for incoming binary messages from Flutter. */
export interface BinaryMessageHandler {

  onMessage(message: ArrayBuffer, reply: BinaryReply): void;

}

export interface BinaryMessenger {
  makeBackgroundTaskQueue(options?: TaskQueueOptions): TaskQueue;

  /**
   * Sends a binary message to the Flutter application.
   *
   * @param channel the name {@link String} of the logical channel used for the message.
   * @param message the message payload, a direct-allocated {@link ByteBuffer} with the message
   *     bytes between position zero and current position, or null.
   */
  send(channel: String, message: ArrayBuffer | null): void;

  send(channel: String, message: ArrayBuffer, callback?: BinaryReply | null): void;

  setMessageHandler(channel: String, handler: BinaryMessageHandler | SendableBinaryMessageHandler | null, taskQueue?: TaskQueue, ...args: Object[]): void;
}
