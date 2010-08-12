#!/usr/bin/env python
#
#
from subprocess import Popen, PIPE

facterp = Popen("faacter --yaml", shell=True, stdout=PIPE, stderr=PIPE)
if facterp.wait():
	print "ERROR:", facterp.stderr.readlines()[0]

facterp.stdout.readline()
facts = dict([ fact.strip('\n').split(':', 1) for fact in facterp.stdout ])
