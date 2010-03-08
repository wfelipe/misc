#!/usr/bin/env python
#

import pyinotify
import sys, os

origin_path = sys.argv[1]
dest_path = sys.argv[2]

wm = pyinotify.WatchManager ()
mask = pyinotify.IN_DELETE | pyinotify.IN_CREATE
mask |= pyinotify.IN_MODIFY | pyinotify.IN_ATTRIB
mask = pyinotify.ALL_EVENTS

class HandleEvents (pyinotify.ProcessEvent):
	def process_ALL_EVENTS (self, event):
		print event
	def process_IN_CREATE (self, event):
		print "Creating: ", event.pathname
		print "mask ", event.mask
	def process_IN_DELETE (self, event):
		print "Deleting: ", event.pathname
	def process_IN_CLOSE_WRITE (self, event):
		print "Close write: ", event.pathname
	def process_IN_ATTRIB (self, event):
		print "Attrib: ", event.pathname

p = HandleEvents ()
notifier = pyinotify.Notifier (wm, p)

wd = wm.add_watch (origin_path, mask, rec = True)
notifier.loop ()
