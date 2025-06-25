
import { describe, it, expect } from '@ohos/hypium'
import { Log } from '@ohos/flutter_ohos';
const TAG = 'LogTest'
export default function LogTest() {
  describe('LogTest', () => {
    it('Log', 0, () => {
      expect(Log.toString()).not().assertNull();
    });

    it('debug', 0, (done:Function) => {
      try {
        Log.d(TAG,'debug')
        done()
      }catch (e) {
      }
    });

    it('info', 0, (done:Function) => {
      try {
        Log.i(TAG,'info')
        done()
      }catch (e) {
      }
    });

    it('warn', 0, (done:Function) => {
      try {
        Log.w(TAG,'warn')
        done()
      }catch (e) {
      }
    });

    it('error', 0, (done:Function) => {
      try {
        Log.e(TAG,'error')
        done()
      }catch (e) {
      }
    });

    it('fatal', 0, (done:Function) => {
      try {
        Log.f(TAG,'fatal')
        done()
      }catch (e) {
      }
    });


  })
}