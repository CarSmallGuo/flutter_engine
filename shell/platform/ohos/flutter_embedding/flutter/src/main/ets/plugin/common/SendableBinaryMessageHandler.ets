// Copyright (C) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
import { lang } from '@kit.ArkTS';
import { BinaryReply } from './BinaryMessenger';

type ISendable = lang.ISendable;
export default interface SendableBinaryMessageHandler extends ISendable {
  onMessage(message: ArrayBuffer, reply: BinaryReply, ...args: Object[]): void;
}
