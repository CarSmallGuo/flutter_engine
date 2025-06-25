import { describe, it, expect, beforeEach, afterEach } from '@ohos/hypium';
import {
  BasicMessageChannel,
  MessageCodec,
  BinaryMessenger,
  BinaryReply,
  Reply,
  TaskQueueOptions,
  TaskQueue,
  BinaryMessageHandler,
  MessageChannelUtils,
} from '@ohos/flutter_ohos';


class MockBinaryMessenger implements BinaryMessenger {
  public calls: Array<[string, ArrayBuffer | null, BinaryReply | null]> = [];
  public calls1: [string, BinaryMessageHandler | null][] = [];


  send(name: string, data: ArrayBuffer | null, replyHandler?: BinaryReply | null): void {
    this.calls.push([name, data, replyHandler ?? null]);
  }

  makeBackgroundTaskQueue(options?: TaskQueueOptions): TaskQueue{
    return {}
  };
  setMessageHandler(name: string, handler: BinaryMessageHandler | null): void {
    this.calls1.push([name, handler]);
  }
}
export default function BasicMessageChannelSend() {
  describe('BasicMessageChannel', () => {
    it('send no callback', 0, async (done: Function) => {
      const mockMessenger = new MockBinaryMessenger();
      const mockCodec: MessageCodec<string> = {
        encodeMessage: (message: string): ArrayBuffer => {
          return new ArrayBuffer(message.length)
        },
        decodeMessage: (message: ArrayBuffer | null): string => {
          throw new Error('Function not implemented.');
        }
      } as MessageCodecType;
      const channel = new BasicMessageChannel<string>(
        mockMessenger,
        'testChannel',
        mockCodec
      );

      channel.send('hello');

      expect(mockMessenger.calls.length).assertEqual(1);
      expect(mockMessenger.calls[0][0]).assertEqual('testChannel');
      expect(mockMessenger.calls[0][1]).not().assertNull();
      expect(mockMessenger.calls[0][2]).assertNull();

      done();
    });

    it('send callback', 0, async (done: Function) => {
      const mockMessenger = new MockBinaryMessenger();
      const mockCodec: MessageCodec<string> = {
        encodeMessage: (message: string): ArrayBuffer => {
          return new ArrayBuffer(message.length)
        },
        decodeMessage: (message: ArrayBuffer | null): string => {
          throw new Error('Function not implemented.');
        }
      } as MessageCodecType;
      const channel = new BasicMessageChannel<string>(
        mockMessenger,
        'testChannel',
        mockCodec
      );

      channel.send('world', () => {});

      expect(mockMessenger.calls.length).assertEqual(1);
      expect(mockMessenger.calls[0][0]).assertEqual('testChannel');
      expect(mockMessenger.calls[0][1]).not().assertNull();
      expect(mockMessenger.calls[0][2]).not().assertNull();

      done();
    });

    it('setMessageHandler', 0,async (done:Function) => {
      const mockMessenger = new MockBinaryMessenger();
      const mockCodec: MessageCodec<string> = {
        encodeMessage: (message: string): ArrayBuffer => {
          return new ArrayBuffer(message.length)
        },
        decodeMessage: (message: ArrayBuffer | null): string => {
          throw new Error('Function not implemented.');
        }
      } as MessageCodecType;
      const channel = new BasicMessageChannel<string>(
        mockMessenger,
        'testChannel',
        mockCodec
      );

      class MockHandler implements MessageHandler<string> {
        onMessage(message: string, reply: Reply<string>): void {}
      }

      const mockHandler = new MockHandler();

      channel.setMessageHandler(mockHandler);
      expect(mockMessenger.calls1.length).assertEqual(1);
      expect(mockMessenger.calls1[0][0]).assertEqual('testChannel');
      expect(mockMessenger.calls1[0][1]).not().assertNull();
      done();
    });

  });
}

interface MessageHandler<T> {
  onMessage(message: T, reply: Reply<T>): void;
}

interface MessageCodecType{
  encodeMessage,
  decodeMessage
}
