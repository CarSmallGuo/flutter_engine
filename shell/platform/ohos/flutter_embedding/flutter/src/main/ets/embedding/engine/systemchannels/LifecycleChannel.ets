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

import Log from '../../../util/Log';
import StringCodec from '../../../plugin/common/StringCodec';
import DartExecutor from '../dart/DartExecutor';
import BasicMessageChannel from '../../../plugin/common/BasicMessageChannel';

/**
 * 生命周期channel
 */
export default class LifecycleChannel {
    private static TAG = "LifecycleChannel";
    private static CHANNEL_NAME = "flutter/lifecycle";

    // These should stay in sync with the AppLifecycleState enum in the framework.
    private static RESUMED = "AppLifecycleState.resumed";
    private static INACTIVE = "AppLifecycleState.inactive";
    private static PAUSED = "AppLifecycleState.paused";
    private static DETACHED = "AppLifecycleState.detached";

    private lastOhosState = "";
    private lastFlutterState = "";
    private lastFocus = true;

    private channel: BasicMessageChannel<string>

    constructor(dartExecutor: DartExecutor) {
        this.channel = new BasicMessageChannel<string>(dartExecutor, LifecycleChannel.CHANNEL_NAME, StringCodec.INSTANCE)
    }

    // Called if at least one window in the app has focus.
    aWindowIsFocused(): void {
        this.sendState(this.lastOhosState, true);
    }

    // Called if no windows in the app have focus.
    noWindowsAreFocused(): void {
        this.sendState(this.lastOhosState, false);
    }

    appIsResumed(): void {
        this.sendState(LifecycleChannel.RESUMED, this.lastFocus);
    }

    appIsInactive(): void {
        this.sendState(LifecycleChannel.INACTIVE, this.lastFocus);
    }

    appIsPaused(): void {
        this.sendState(LifecycleChannel.PAUSED, this.lastFocus);
    }

    appIsDetached(): void {
        this.sendState(LifecycleChannel.DETACHED, this.lastFocus);
    }

    // Here's the state table this implements:
    //
    // | UIAbility State | Window focused | Flutter state |
    // |-----------------|----------------|---------------|
    // | onCreate        |     true       |    resumed    |
    // | onCreate        |     false      |    inactive   |
    // | onForeground    |     true       |    resumed    |
    // | onForeground    |     false      |    inactive   |
    // | onBackground    |     true       |    paused     |
    // | onBackground    |     false      |    paused     |
    // | onDestroy       |     true       |    detached   |
    // | onDestroy       |     false      |    detached   |

    private sendState(state: string, hasFocus: boolean): void {
        if (this.lastOhosState == state && hasFocus == this.lastFocus) {
            // No inputs changed, so Flutter state could not have changed.
            return;
        }
        let newState: string;
        if (state == LifecycleChannel.RESUMED) {
            newState = hasFocus ? LifecycleChannel.RESUMED : LifecycleChannel.INACTIVE;
        } else {
            newState = state;
        }
        // Keep the last reported values for future updates.
        this.lastOhosState = state;
        this.lastFocus = hasFocus;
        if (newState == this.lastFlutterState) {
            // No change in the resulting Flutter state, so don't report anything.
            return;
        }
        Log.i(LifecycleChannel.TAG, "Sending " + newState + " message.");
        this.channel.send(newState);
        this.lastFlutterState = newState;
    }
}