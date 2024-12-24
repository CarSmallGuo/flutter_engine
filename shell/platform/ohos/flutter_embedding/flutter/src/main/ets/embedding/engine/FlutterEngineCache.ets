// Copyright (C) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import FlutterEngine from "./FlutterEngine"

export default class FlutterEngineCache {
  private static instance : FlutterEngineCache;
  private cachedEngines: Map<String, FlutterEngine> = new Map();

  static getInstance(): FlutterEngineCache {
    if (FlutterEngineCache.instance == null) {
      FlutterEngineCache.instance = new FlutterEngineCache();
    }
    return FlutterEngineCache.instance;
  }
  /**
   * 返回engineId对应的FlutterEngine是否存在
   */
  contains(engineId: String) : boolean {
    return this.cachedEngines.has(engineId);
  }

  /**
   * 返回engineId对应的FlutterEngine
   */
  get(engineId: String) : FlutterEngine | null {
    return this.cachedEngines.get(engineId) || null;
  }
  /**
   * 将传入的FlutterEngine与engineId放在缓存中
   */
  put(engineId :String, engine: FlutterEngine | null): void {
    if(engine != null) {
      this.cachedEngines.set(engineId, engine);
    } else {
      this.cachedEngines.delete(engineId);
    }
  }
  /**
   * 移除engineId对应的FlutterEngine
   */
  remove(engineId: String) : void {
    this.put(engineId, null);
  }

  /**
   * 移除cachedEngines所有中所有的FlutterEngine
   */
  clear():void {
    this.cachedEngines.clear();
  }
}