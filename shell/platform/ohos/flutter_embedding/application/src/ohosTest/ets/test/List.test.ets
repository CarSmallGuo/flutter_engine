/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the 'License');
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an 'AS IS' BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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

import DartMessengerTest from './embedding/engine/DartMessengerTest.test'
import ApplicationInfoTest from './embedding/engine/ApplicationInfoTest.test'
import FlutterApplicationInfoTest from './embedding/engine/FlutterApplicationInfoTest.test'
import FlutterLoaderTest from './embedding/engine/FlutterLoaderTest.test'
import FlutterMutatorViewTest from './embedding/engine/FlutterMutatorViewTest.test'
import FlutterMutatorsStackTest from './embedding/engine/FlutterMutatorsStackTest.test'
import FlutterRendererTest  from './embedding/engine/FlutterRendererTest.test'
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
}