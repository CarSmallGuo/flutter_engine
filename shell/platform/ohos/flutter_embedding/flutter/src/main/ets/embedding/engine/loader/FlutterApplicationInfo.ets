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

import BuildProfile from "../../../../../../BuildProfile";

const DEFAULT_AOT_SHARED_LIBRARY_NAME = "libapp.so";
const DEFAULT_VM_SNAPSHOT_DATA = "vm_snapshot_data";
const DEFAULT_ISOLATE_SNAPSHOT_DATA = "isolate_snapshot_data";
const DEFAULT_FLUTTER_ASSETS_DIR = "flutter_assets";


/**
 * application 信息，后期看如何设置
 */
export default class FlutterApplicationInfo {
  aotSharedLibraryName: string;
  vmSnapshotData: string;
  isolateSnapshotData: string;
  flutterAssetsDir: string;
  domainNetworkPolicy: string;
  nativeLibraryDir: string;
  automaticallyRegisterPlugins: boolean;
  //是否是开发模式，先放在这里，后续应该从context获取
  isDebugMode: boolean;
  //是否是profile模式
  isProfile: boolean;

  constructor(aotSharedLibraryName: string | null,
              vmSnapshotData: string | null,
              isolateSnapshotData: string | null,
              flutterAssetsDir: string | null,
              domainNetworkPolicy: string | null,
              nativeLibraryDir: string,
              automaticallyRegisterPlugins: boolean) {
    this.aotSharedLibraryName = aotSharedLibraryName == null ? DEFAULT_AOT_SHARED_LIBRARY_NAME : aotSharedLibraryName;
    this.vmSnapshotData = vmSnapshotData == null ? DEFAULT_VM_SNAPSHOT_DATA : vmSnapshotData;
    this.isolateSnapshotData = isolateSnapshotData == null ? DEFAULT_ISOLATE_SNAPSHOT_DATA : isolateSnapshotData;
    this.flutterAssetsDir = flutterAssetsDir == null ? DEFAULT_FLUTTER_ASSETS_DIR : flutterAssetsDir;
    this.domainNetworkPolicy = domainNetworkPolicy == null ? "" : domainNetworkPolicy;
    this.nativeLibraryDir = nativeLibraryDir;
    this.automaticallyRegisterPlugins = automaticallyRegisterPlugins;
    this.isDebugMode = "debug" == String(BuildProfile.BUILD_MODE_NAME);
    this.isProfile = "profile" == String(BuildProfile.BUILD_MODE_NAME);
  }
}