
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
    it('should handle newline event correctly', 0, ()  => {
      editingState.handleNewlineEvent();
      expect(editingState.getStringCache()).assertEqual('hello\n');
      expect(editingState.getSelectionStart()).assertEqual(6);
      expect(editingState.getSelectionEnd()).assertEqual(6);
    })
   })
}