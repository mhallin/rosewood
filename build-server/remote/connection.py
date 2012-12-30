#-*- coding: utf-8 -*-

import asyncore
import socket
import msgpack

from Queue import Queue

import objc
import Foundation

class DeviceClient(asyncore.dispatcher):
    def __init__(self, host, port):
        asyncore.dispatcher.__init__(self)
        self.create_socket(socket.AF_INET, socket.SOCK_STREAM)
        self.connect((host, port))

        self.buffer = ''

    def handle_connect(self):
        pass

    def handle_close(self):
        print 'closed'
        self.close()

    def handle_read(self):
        data = self.recv(8192)
        print 'received data', data

    def writable(self):
        return len(self.buffer) > 0

    def handle_write(self):
        if not self.buffer: return

        sent = self.send(self.buffer)
        self.buffer = self.buffer[sent:]

class DeviceConnectionObject(Foundation.NSObject):
    def initWithHost_port_(self, host, port):
        self = self.init()
        if self is None: return None
        
        self.client = DeviceClient(host, port)
        self.commandQueue = Queue()

        return self

    def queueSize(self):
        return self.commandQueue.qsize()

    def appendCommand_(self, command):
        self.willChangeValueForKey_('queueSize')
        self.commandQueue.put_nowait([str(item) for item in command])
        self.didChangeValueForKey_('queueSize')

    def tickLoop(self):
        asyncore.loop(timeout=0, count=1)
        self.fillSendBuffer()

    def disconnect(self):
        self.client.close()
        self.client = None

    def fillSendBuffer(self):
        if self.client.buffer or self.commandQueue.empty(): return

        self.willChangeValueForKey_('queueSize')
        command = self.commandQueue.get_nowait()
        self.client.buffer = msgpack.packb(command)
        self.didChangeValueForKey_('queueSize')

class DeviceConnectionFactory(Foundation.NSObject):
    def createConnectionToHost_port_(self, host, port):
        return DeviceConnectionObject.alloc().initWithHost_port_(host, port)
