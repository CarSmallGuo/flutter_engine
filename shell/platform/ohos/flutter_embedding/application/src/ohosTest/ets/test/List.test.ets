/*
Copyright (C) 2024 Huawei Device Co., Ltd.

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

import FlutterAbilityAndEntryDelegateTest from './embedding/ohos/FlutterAbilityAndEntryDelegateTest.test';
import FlutterAbilityTest from './embedding/ohos/FlutterAbilityTest.test';
import FlutterEntryTest from './embedding/ohos/FlutterEntryTest.test';
import FlutterManagerTest from './embedding/ohos/FlutterManagerTest.test';
import SettingTest from './embedding/ohos/SettingTest.test';
import BinaryCodecTest from './plugin/common/BinaryCodecTest.test';
import PlatformViewRegistryImplTest from './plugin/platform/PlatformViewRegistryImplTest.test';
import PlatformViewWrapperTest from './plugin/platform/PlatformViewWrapperTest.test';
import PlatformViewsControllerTest from './plugin/platform/PlatformViewsControllerTest.test';
import RawPointerCoordTest from './plugin/platform/RawPointerCoordTest.test';
import RootDvModelManagerTest from './plugin/platform/RootDvModelManagerTest.test';
import ByteBufferTest from './plugin/util/ByteBufferTest.test';
import PlatformChannelUnitTest from './embedding/engine/systemchannels/PlatformChannel.test';
import RestorationChannelUnitTest from './embedding/engine/systemchannels/RestorationChannel.test';
import SettingsChannelUnitTest from './embedding/engine/systemchannels/SettingsChannel.test';
import TextInputChannelUnitTest from './embedding/engine/systemchannels/TextInputChannel.test';
import JSONMethodCodecUnitTest from './plugin/common/JSONMethodCodec.test';
import MethodCallUnitTest from './plugin/common/MethodCall.test';
import AccessibilityEventsDelegateUnitTest from './plugin/platform/AccessibilityEventsDelegate.test';

import DartMessengerTest from './embedding/engine/dart/DartMessengerTest.test'
import ApplicationInfoTest from './embedding/engine/loader/ApplicationInfoTest.test'
import FlutterApplicationInfoTest from './embedding/engine/loader/FlutterApplicationInfoTest.test'
import FlutterLoaderTest from './embedding/engine/loader/FlutterLoaderTest.test'
import FlutterMutatorViewTest from './embedding/engine/mutatorsstack/FlutterMutatorViewTest.test'
import FlutterMutatorsStackTest from './embedding/engine/mutatorsstack/FlutterMutatorsStackTest.test'
import FlutterRendererTest  from './embedding/engine/renderer/FlutterRendererTest.test'
import TouchEventProcessorTest from './embedding/ohos/TouchEventProcessorTest.test';
import TouchEventTrackerTest from './embedding/ohos/TouchEventTrackerTest.test';
import WindowInfoRepositoryCallbackAdapterWrapperTest from './embedding/ohos/WindowInfoRepositoryCallbackAdapterWrapperTest.test';
import FlutterEngineTest from './embedding/engine/FlutterEngineTest.test';
import FlutterEngineCacheTest from './embedding/engine/FlutterEngineCacheTest.test';
import FlutterEngineConnectionRegistryTest from './embedding/engine/FlutterEngineConnectionRegistryTest.test';
import FlutterEngineGroupCacheTest from './embedding/engine/FlutterEngineGroupCacheTest.test';
import FlutterEngineGroupTest from './embedding/engine/FlutterEngineGroupTest.test';
import FlutterNapiTest from './embedding/engine/FlutterNapiTest.test';
import FlutterOverlaySurfaceTest from './embedding/engine/FlutterOverlaySurfaceTest.test';
import FlutterShellArgsTest from './embedding/engine/FlutterShellArgsTest.test';
import BinaryMessengerTest from './plugin/common/BinaryMessengerTest.test';
import DartExecutorTest from './embedding/engine/dart/DartExecutorTest.test';

import PathUtilsTest from './plugin/util/PathUtils.test';
import StringUtilsTest from './plugin/util/StringUtils.test';
import ToolUtilsTest from './plugin/util/ToolUtils.test';
import MessageChannelUtilsTest from './plugin/util/MessageChannelUtils.test';
import LogTest from './plugin/util/Log.test';
import TraceSectionTest from './plugin/util/TraceSection.test';
import FlutterInjectorTest from './FlutterInjector.test';
import SendableBinaryCodecTest from './plugin/common/SendableBinaryCodec.test';
import dynamicViewJsonTest from './view/DynamicView/dynamicViewJson.test';
import PlatformOverlayViewTest from './plugin/platform/PlatformOverlayView.test';
import PlatformViewTest from './plugin/platform/PlatformView.test';
import FlutterRunArgumentsTest from './view/FlutterRunArguments.test';
import LifecycleChannelTest from './embedding/engine/systemchannels/LifecycleChannel.test';
import FlutterExceptionTest from './plugin/common/FlutterException.test';
import JSONMessageCodecTest from './plugin/common/JSONMessageCodec.test';
import SendableJSONMessageCodecTest from './plugin/common/SendableJSONMessageCodec.test';
import SendableJSONMethodCodecTest from './plugin/common/SendableJSONMethodCodec.test';
import SendableStringCodecTest from './plugin/common/SendableStringCodec.test';
import StandardMessageCodecTest from './plugin/common/StandardMessageCodec.test';
import StandardMethodCodecTest from './plugin/common/StandardMethodCodec.test';
import StringCodecTest from './plugin/common/StringCodec.test';
import ListenableEditingStateTest from './plugin/editing/ListenableEditingState.test';
import TextEditingDeltaTest from './plugin/editing/TextEditingDelta.test';
import TextInputPluginTest from './plugin/editing/TextInputPlugin.test';
import LocalizationPluginTest from './plugin/localization/LocalizationPlugin.test';
import SendableStandardMessageCodecTest from './plugin/common/SendableStandardMessageCodec.test';
import SendableStandardMethodCodecTest from './plugin/common/SendableStandardMethodCodec.test';

export default function testsuite() {
  FlutterAbilityAndEntryDelegateTest();
  FlutterAbilityTest();
  FlutterEntryTest();
  FlutterManagerTest();
  SettingTest();
  BinaryCodecTest();
  PlatformViewRegistryImplTest();
  PlatformViewWrapperTest();
  PlatformViewsControllerTest();
  RawPointerCoordTest();
  RootDvModelManagerTest();
  ByteBufferTest();
  PlatformChannelUnitTest();
  RestorationChannelUnitTest();
  SettingsChannelUnitTest();
  TextInputChannelUnitTest();
  JSONMethodCodecUnitTest();
  MethodCallUnitTest();
  AccessibilityEventsDelegateUnitTest();
  DartMessengerTest();
  ApplicationInfoTest();
  FlutterApplicationInfoTest();
  FlutterLoaderTest()
  FlutterMutatorViewTest();
  FlutterMutatorsStackTest();
  FlutterRendererTest();
  TouchEventProcessorTest();
  TouchEventTrackerTest();
  WindowInfoRepositoryCallbackAdapterWrapperTest();
  FlutterEngineTest();
  FlutterEngineCacheTest();
  FlutterEngineConnectionRegistryTest();
  FlutterEngineGroupCacheTest();
  FlutterEngineGroupTest();
  FlutterNapiTest();
  FlutterOverlaySurfaceTest();
  FlutterShellArgsTest();
  BinaryMessengerTest();
  DartExecutorTest();
  PathUtilsTest();
  StringUtilsTest();
  ToolUtilsTest();
  MessageChannelUtilsTest();
  LogTest();
  TraceSectionTest();
  FlutterInjectorTest();
  SendableBinaryCodecTest();
  dynamicViewJsonTest();
  PlatformOverlayViewTest();
  PlatformViewTest();
  PlatformViewTest();
  FlutterRunArgumentsTest();
  LifecycleChannelTest();
  FlutterExceptionTest();
  JSONMessageCodecTest();
  SendableJSONMessageCodecTest();
  SendableJSONMethodCodecTest();
  SendableStandardMessageCodecTest();
  SendableStandardMethodCodecTest();
  SendableStringCodecTest();
  StandardMessageCodecTest();
  StandardMethodCodecTest();
  StringCodecTest();
  ListenableEditingStateTest();
  TextEditingDeltaTest();
  TextInputPluginTest();
  LocalizationPluginTest();
}