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
/** Tracks the motion events received by the FlutterView. */
import PlainArray from '@ohos.util.PlainArray';
import { TouchEvent } from '@ohos.multimodalInput.touchEvent';
import Queue from '@ohos.util.Queue';

export class TouchEventTracker {
  private eventById : PlainArray<TouchEvent>;
  private unusedEvents : Queue<number>;
  private static INSTANCE:TouchEventTracker;

  public static getInstance(): TouchEventTracker {
    if (TouchEventTracker.INSTANCE == null) {
      TouchEventTracker.INSTANCE = new TouchEventTracker();
    }
    return TouchEventTracker.INSTANCE;
  }

  constructor() {
    this.eventById = new PlainArray();
    this.unusedEvents = new Queue();
  }

  /** Tracks the event and returns a unique MotionEventId identifying the event. */
  public track(event :TouchEvent) : TouchEventId {
    const eventId:TouchEventId = TouchEventId.createUnique();
    this.eventById.add(eventId.getId(), event);
    this.unusedEvents.add(eventId.getId());
    return eventId;
  }

  /**
   * Returns the MotionEvent corresponding to the eventId while discarding all the motion events
   * that occurred prior to the event represented by the eventId. Returns null if this event was
   * popped or discarded.
   */
  public pop(eventId : TouchEventId) : TouchEvent {
    // remove all the older events.
    while (this.unusedEvents.length != 0 && this.unusedEvents.getFirst() < eventId.getId()) {
      this.eventById.remove(this.unusedEvents.pop());
    }

    // remove the current event from the heap if it exists.
    if (this.unusedEvents.length != 0 && this.unusedEvents.getFirst() == eventId.getId()) {
      this.unusedEvents.pop();
    }

    const event : TouchEvent = this.eventById.get(eventId.getId());
    this.eventById.remove(eventId.getId());
    return event;
  }
}

/** Represents a unique identifier corresponding to a motion event. */
export class TouchEventId {
  private static ID_COUNTER : number = 0;
  private id : number;

  constructor(id : number) {
    this.id = id;
  }

  public static from(id : number) : TouchEventId {
    return new TouchEventId(id);
  }

  public static createUnique() : TouchEventId {
    return new TouchEventId(TouchEventId.ID_COUNTER++);
  }

  public getId() : number {
    return this.id;
  }
}