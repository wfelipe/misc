#!/usr/bin/env python

import httplib, urllib
import sys

conn = httplib.HTTPConnection ("localhost:8080")
conn.request ("POST", "/broadcast/pub?channel=pub", sys.argv[1])
resp = conn.getresponse ()
print resp.status, resp.reason
data = resp.read ()
conn.close ()
