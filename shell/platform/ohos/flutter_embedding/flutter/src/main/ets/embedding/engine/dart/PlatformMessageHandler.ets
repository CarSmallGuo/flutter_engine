// Copyright (c) 2023 Hunan OpenValley Digital Industry Development Co., Ltd.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

export interface PlatformMessageHandler {

  handleMessageFromDart(channel: String,message: ArrayBuffer,replyId: number, messageData: number): void;

  handlePlatformMessageResponse(replyId: number, reply: ArrayBuffer): void;

}