#!/usr/bin/env python
import getopt
import sys
from pymongo import Connection
import json

# default values
host = 'localhost'
port = 27017
username = None
password = None

def usage():
	print sys.argv[0]

# getopt
try:
	opts, args = getopt.getopt(sys.argv[1:], 'h:p:u:w:')
except getopt.GetoptError, err:
	print str(err)
	usage()
	sys.exit(2)
for o, a in opts:
	if o == "-h":
		host = a
	elif o == "-p":
		port = int(a)
	elif o == "-u":
		username = a
	elif o == "-w":
		password = a

connection = Connection(host, port)
if not username is None:
	connection['admin'].authenticate(username, password)
	
db = connection['admin']
print connection.server_info()

dblocal = connection['local']
#dblocal.system.namespaces.find({'name': 'local.oplog.$main'})
a = dblocal['optlog.$main'].find()
for op in a:
	print a
