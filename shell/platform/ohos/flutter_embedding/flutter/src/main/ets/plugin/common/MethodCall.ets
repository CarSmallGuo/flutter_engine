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

import ToolUtils from '../../util/ToolUtils';
import TreeMap from '@ohos.util.TreeMap';
import HashMap from '@ohos.util.HashMap';
import LightWeightMap from '@ohos.util.LightWeightMap';
import Any from './Any';

/** Command object representing a method call on a {@link MethodChannel}. */
export default class MethodCall {
  /** The name of the called method. */
  method: string;

  /**
   * Arguments for the call.
   *
   * <p>Consider using {@link #arguments()} for cases where a particular run-time type is expected.
   * Consider using {@link #argument(String)} when that run-time type is {@link Map} or {@link
   * JSONObject}.
   */
  args: Any;

  constructor(method: string, args: Any) {
    this.method = method;
    this.args = args;
  }

  argument(key: string): Any {
    if (this.args == null) {
      return null;
    } else if (this.args instanceof Map) {
      return (this.args as Map<Any, Any>).get(key);
    } else if (ToolUtils.isObj(this.args)) {
      return this.args[key]
    } else {
      throw new Error("ClassCastException");
    }
  }

  hasArgument(key: string): boolean {
    if (this.args == null) {
      return false;
    } else if (this.args instanceof Map) {
      return (this.args as Map<Any, Any>).has(key);
    } else if (ToolUtils.isObj(this.args)) {
      return this.args.hasOwnProperty(key);
    } else {
      throw new Error("ClassCastException");
    }
  }
}