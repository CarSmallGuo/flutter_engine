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

import flutter from 'libflutter.so'

/**
 * 默认字符串工具
 */
export default class StringUtils {

  static stringToArrayBuffer(str: string): ArrayBuffer {
    if(str.length == 0){
      return new ArrayBuffer(0);
    }
    return flutter.nativeEncodeUtf8(str).buffer;
  }

  static arrayBufferToString(buffer: ArrayBuffer): string {
    if (buffer.byteLength <= 0) {
      return "";
    }
    return flutter.nativeDecodeUtf8(new Uint8Array(buffer));
  }

  static uint8ArrayToString(buffer: Uint8Array): string {
    if (buffer.length <= 0) {
      return "";
    }
    return flutter.nativeDecodeUtf8(buffer);
  }

  static isNotEmpty(str: string): boolean {
    return str != null && str.length > 0;
  }

  static isEmpty(str: string): boolean {
    return (!str) || str.length == 0;
  }
}