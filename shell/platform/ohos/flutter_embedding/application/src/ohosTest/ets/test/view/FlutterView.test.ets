import { beforeAll, describe, expect, it, MockKit, TestType, when } from '@ohos/hypium';
import {
  DisplayFeature,
  DisplayFeatureState,
  DisplayFeatureType,
  FlutterView,
  PlatformView,
  PlatformViewParas,
  PlatformViewsController,
  FlutterEngine,
  ViewportMetrics,
  FlutterInjector,
  FirstFrameListener
} from '@ohos/flutter_ohos';
import { display } from '@kit.ArkUI';
import { common } from '@kit.AbilityKit';
import { Params } from '@ohos/flutter_ohos/src/main/ets/plugin/platform/PlatformView';
import { IntentionCode, KeyCode } from '@kit.InputKit';
import { EventTarget } from '@kit.ArkTS';

const TAG = 'FlutterView'

export default function FlutterViewTest() {

  describe('ViewportMetricsTest', () => {

    it('clone', TestType.FUNCTION, () => {
      const viewportMetrics = new ViewportMetrics();
      viewportMetrics.devicePixelRatio = 2.0;
      const newVM = viewportMetrics.clone();
      expect(newVM).not().assertNull();
      expect(newVM.devicePixelRatio).assertEqual(2.0);
    })

    it('isEqual', TestType.FUNCTION, () => {
      const viewportMetrics = new ViewportMetrics();
      const newVM = viewportMetrics.clone();
      viewportMetrics.devicePixelRatio = 2.0;
      newVM.devicePixelRatio = 2.0;
      expect(viewportMetrics.isEqual(newVM)).assertTrue();
      newVM.devicePixelRatio = 3.0;
      expect(viewportMetrics.isEqual(newVM)).assertFalse();
    })

  })

  describe('DisplayFeature', () => {
    const LEFT = 10;
    const TOP = 10;
    const WIDTH = 10;
    const HEIGHT = 10;
    const TYPE = DisplayFeatureType.FOLD;
    const STATE = DisplayFeatureState.POSTURE_HALF_OPENED;
    const bound: display.Rect = {
      left: LEFT,
      top: TOP,
      width: WIDTH,
      height: HEIGHT
    }
    let displayFeature: DisplayFeature;

    beforeAll(() => {
      displayFeature = new DisplayFeature(bound, TYPE, STATE);
    })

    it('getBound', TestType.FUNCTION, () => {
      expect(displayFeature.bound.left).assertEqual(LEFT);
      expect(displayFeature.bound.top).assertEqual(TOP);
      expect(displayFeature.bound.width).assertEqual(WIDTH);
      expect(displayFeature.bound.height).assertEqual(HEIGHT);
    })

    it('getType', TestType.FUNCTION, () => {
      expect(displayFeature.type).assertEqual(TYPE);
    })

    it('getState', TestType.FUNCTION, () => {
      expect(displayFeature.state).assertEqual(STATE);
    })

    it('setBound', TestType.FUNCTION, () => {
      const bound: display.Rect = {
        left: 0,
        top: 0,
        width: 0,
        height: 0
      }
      displayFeature.setBound(bound);
      expect(displayFeature.bound.left).assertEqual(0);
      expect(displayFeature.bound.top).assertEqual(0);
      expect(displayFeature.bound.width).assertEqual(0);
      expect(displayFeature.bound.height).assertEqual(0);
    })

    it('setType', TestType.FUNCTION, () => {
      displayFeature.setType(DisplayFeatureType.UNKNOWN);
      expect(displayFeature.type).assertEqual(DisplayFeatureType.UNKNOWN);
    })

    it('setState', TestType.FUNCTION, () => {
      displayFeature.setState(DisplayFeatureState.UNKNOWN);
      expect(displayFeature.state).assertEqual(DisplayFeatureState.UNKNOWN);
    })

  })

  describe('PlatformViewParas', () => {
    let platformViewParas: PlatformViewParas;

    beforeAll(() => {
      platformViewParas = new PlatformViewParas();
    })

    it('setValue', TestType.FUNCTION, () => {
      platformViewParas.setValue(10, 20, 30, 40);
      expect(platformViewParas.width).assertEqual(10);
      expect(platformViewParas.height).assertEqual(20);
      expect(platformViewParas.top).assertEqual(30);
      expect(platformViewParas.left).assertEqual(40);
    })

    it('setOffset', TestType.FUNCTION, () => {
      platformViewParas.setOffset(10, 20);
      expect(platformViewParas.top).assertEqual(10);
      expect(platformViewParas.left).assertEqual(20);
    })
  })

  describe('FlutterView', () => {
    const VIEW_ID = 'VIEW_ID';
    let context: common.UIAbilityContext;
    let flutterView: FlutterView;
    let flutterEngine: FlutterEngine;
    const platformViewsController = new PlatformViewsController();

    const testArea: Area = {
      width: 10,
      height: 10,
      position: {
        x: 0,
        y: 0,
      },
      globalPosition: {
        x: 0,
        y: 0,
      }
    }

    const testKeyEvent: KeyEvent = {
      type: KeyType.Down,
      keyCode: KeyCode.KEYCODE_HOME,
      keyText: '',
      keySource: KeySource.Keyboard,
      deviceId: 0,
      metaKey: 0,
      timestamp: Date.now(),
      stopPropagation: () => {
      },
      intentionCode: IntentionCode.INTENTION_DOWN,
    };

    const testPanGestureEvent: PanGestureEvent = {
      offsetX: 0,
      offsetY: 0,
      velocityX: 0,
      velocityY: 0,
      velocity: 0,
      fingerList: [],
      target: {
        area: testArea
      },
      timestamp: Date.now(),
      source: SourceType.Unknown,
      pressure: 0,
      tiltX: 0,
      tiltY: 0,
      sourceTool: SourceTool.Unknown
    }

    beforeAll(() => {
      context = getContext() as common.UIAbilityContext;
      flutterView = new FlutterView(VIEW_ID, context);
      flutterEngine = new FlutterEngine(context, null, null, platformViewsController);
    })

    it('getId', TestType.FUNCTION, () => {
      expect(flutterView.getId()).assertEqual(VIEW_ID);
    })

    it('setSurfaceId', TestType.FUNCTION, () => {
      const SURFACE_ID = 'SURFACE_ID';
      flutterView.setSurfaceId(SURFACE_ID);
      expect(flutterView.getSurfaceId()).assertEqual(SURFACE_ID);
    })

    it('getSurfaceId', TestType.FUNCTION, () => {
      const DEFAULT_SURFACE_ID = '0';
      flutterView.setSurfaceId(DEFAULT_SURFACE_ID);
      expect(flutterView.getSurfaceId()).assertEqual(DEFAULT_SURFACE_ID);
    })

    it('getEmbeddingNodeController', TestType.FUNCTION, () => {
      expect(flutterView.getEmbeddingNodeController()).not().assertNull();
    })

    it('setWrappedBuilder', TestType.FUNCTION, () => {
      expect(flutterView.getWrappedBuilder()).assertUndefined();
      flutterView.setWrappedBuilder(new WrappedBuilder(() => {
      }));
      expect(flutterView.getWrappedBuilder()).not().assertNull();
    })

    it('getWrappedBuilder', TestType.FUNCTION, () => {
      flutterView.setWrappedBuilder(new WrappedBuilder(() => {
      }));
      expect(flutterView.getWrappedBuilder()).not().assertNull();
    })

    class TestPlatformView extends PlatformView {
      getView(): WrappedBuilder<[Params]> {
        throw new Error('Method not implemented.');
      }

      dispose(): void {
        throw new Error('Method not implemented.');
      }
    }

    it('setPlatformView', TestType.FUNCTION, () => {
      expect(flutterView.getPlatformView()).assertUndefined();
      flutterView.setPlatformView(new TestPlatformView());
      expect(flutterView.getPlatformView()).not().assertNull();
    })

    it('getPlatformView', TestType.FUNCTION, () => {
      flutterView.setPlatformView(new TestPlatformView());
      expect(flutterView.getPlatformView()).not().assertNull();
    })

    it('getPlatformViewSize', TestType.FUNCTION, () => {
      expect(flutterView.getPlatformViewSize()).not().assertNull();
    })

    it('getDVModel', TestType.FUNCTION, () => {
      expect(flutterView.getDVModel()).not().assertNull();
    })

    it('getKeyboardHeight', TestType.FUNCTION, () => {
      // flutterView.getKeyboardHeight();  error
    })

    it('onDestroy', TestType.FUNCTION, () => {
      flutterView.onDestroy();
    })

    it('attachToFlutterEngine', TestType.FUNCTION, () => {
      // let mocker: MockKit = new MockKit();
      // const injector = FlutterInjector.getInstance().getFlutterNapi();
      // let mockFunc = mocker.mockFunc(injector,injector.updateRefreshRate);
      // when(mockFunc)(60).afterReturn(undefined);
      // injector.updateRefreshRate(60);
      // console.log('t2233='+display.getDefaultDisplaySync().refreshRate);
      // flutterView.attachToFlutterEngine(flutterEngine);
      // expect(flutterView.isAttachedToFlutterEngine()).assertTrue();
    })

    it('detachFromFlutterEngine', TestType.FUNCTION, () => {
      flutterView.detachFromFlutterEngine();
    })

    it('onWindowCreated', TestType.FUNCTION, () => {
      flutterView.onWindowCreated();
    })

    it('sendSettings', TestType.FUNCTION, () => {
      flutterView.sendSettings();
    })

    it('onSurfaceCreated', TestType.FUNCTION, () => {
      flutterView.onSurfaceCreated();
    })

    it('onSurfaceDestroyed', TestType.FUNCTION, () => {
      flutterView.onSurfaceDestroyed();
    })

    it('onAreaChange', TestType.FUNCTION, () => {
      flutterView.onAreaChange(null, false);
      flutterView.onAreaChange(null, true);
      flutterView.onAreaChange(testArea, false);
      flutterView.onAreaChange(testArea, true);
    })

    it('onKeyPreIme', TestType.FUNCTION, () => {
      flutterView.onKeyPreIme(testKeyEvent);
    })
    it('onKeyEvent', TestType.FUNCTION, () => {
      flutterView.onKeyEvent(testKeyEvent);
    })

    it('onMouseWheel', TestType.FUNCTION, () => {
      flutterView.onMouseWheel('actionStart', testPanGestureEvent);
    })

    const listener = new MyListener();

    it('addFirstFrameListener', TestType.FUNCTION, () => {
      flutterView.addFirstFrameListener(listener);
    })

    it('removeFirstFrameListener', TestType.FUNCTION, () => {
      flutterView.removeFirstFrameListener(listener);
    })

    it('hasRenderedFirstFrame', TestType.FUNCTION, () => {
      const result = flutterView.hasRenderedFirstFrame();
      expect(result).not().assertNull();
    })

    it('onFirstFrame', TestType.FUNCTION, () => {
      flutterView.onFirstFrame();
    })

    it('setCheckFullScreen', TestType.FUNCTION, () => {
      flutterView.onFirstFrame();
    })

    it('setCheckKeyboard', TestType.FUNCTION, () => {
      flutterView.setCheckFullScreen(true);
    })

    it('setCheckGesture', TestType.FUNCTION, () => {
      flutterView.setCheckGesture(true);
    })

    it('setCheckAiBar', TestType.FUNCTION, () => {
      flutterView.setCheckAiBar(true);
    })

    it('setPaddingTop', TestType.FUNCTION, () => {
      flutterView.setPaddingTop(0);
    })

  })


}

class MyListener implements FirstFrameListener {
  onFirstFrame(): void {
    throw new Error('Method not implemented.');
  }
}
