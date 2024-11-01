// Copyright (c) 2023 Hunan OpenValley Digital Industry Development Co., Ltd.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

export default class FlutterRunArguments {
  public bundlePath: string;
  public entrypoint: string;
  public libraryPath: string;

  constructor(bundlePath: string, entrypoint: string, libraryPath: string) {
    this.bundlePath = bundlePath;
    this.entrypoint = entrypoint;
    this.libraryPath = libraryPath;
  }
}