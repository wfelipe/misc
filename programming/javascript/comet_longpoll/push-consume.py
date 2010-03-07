#!/usr/bin/env python

import httplib, urllib
import sys

fd = open (sys.argv[1])
postdata = fd.read ()
fd.close ()
conn = httplib.HTTPConnection ("localhost:8080")
conn.request ("POST", "/broadcast/pub?id=3", postdata)
resp = conn.getresponse ()
print resp.status, resp.reason
data = resp.read ()
conn.close ()
