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

import { describe, it, expect } from '@ohos/hypium';
import { EditingStateWatcher, ListenableEditingState } from '@ohos/flutter_ohos/src/main/ets/plugin/editing/ListenableEditingState';

export default function ListenableEditingStateTest() {
  describe('ListenableEditingStateTest', () => {
    let editingState: ListenableEditingState = new ListenableEditingState(null, 0);
    it('should initialize with default values', 0, () => {
      expect(editingState.getSelectionStart()).assertEqual(0);
      expect(editingState.getSelectionEnd()).assertEqual(0);
      expect(editingState.getComposingStart()).assertEqual(-1);
      expect(editingState.getComposingEnd()).assertEqual(-1);
      expect(editingState.getStringCache()).assertEqual('');
    })
    it('should handle insert text event correctly', 0, ()  => {
      editingState.handleInsertTextEvent('hello');
      expect(editingState.getStringCache()).assertEqual('hello');
      expect(editingState.getSelectionStart()).assertEqual(5);
      expect(editingState.getSelectionEnd()).assertEqual(5);
    })

    it('should handle delete event correctly (right)', 0, ()  => {
      editingState.handleDeleteEvent(true, 1);
      expect(editingState.getStringCache()).assertEqual('hello');
      expect(editingState.getSelectionStart()).assertEqual(5);
      expect(editingState.getSelectionEnd()).assertEqual(5);
    })
    it('should handle newline event correctly', 0, ()  => {
      editingState.handleNewlineEvent();
      expect(editingState.getStringCache()).assertEqual('hello\n');
      expect(editingState.getSelectionStart()).assertEqual(6);
      expect(editingState.getSelectionEnd()).assertEqual(6);
    })
   })
}