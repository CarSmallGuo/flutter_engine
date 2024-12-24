/*
Copyright (c) 2023 Hunan OpenValley Digital Industry Development Co., Ltd.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of pngout nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

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