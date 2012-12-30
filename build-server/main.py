#-*- coding: utf-8 -*-

import objc
objc.setVerbose(1)

from AppKit import NSApplication

from build.worker import Worker
from remote.connection import DeviceConnectionFactory

app_delegate = NSApplication.sharedApplication().delegate()

worker = Worker.alloc().init()
connection_factory = DeviceConnectionFactory.alloc().init()

app_delegate.setWorker_(worker)
app_delegate.setConnectionFactory_(connection_factory);
