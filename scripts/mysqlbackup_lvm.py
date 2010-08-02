import sys, os
import MySQLdb

conn = MySQLdb.connect (host = 'localhost',
			user = 'wfelipe',
			passwd = '',
			db = 'tmp')
cursor = conn.cursor ()
cursor.execute ('flush tables with read lock')

# lvcreate snapshot
ret = os.system ('sudo lvcreate -s -L 5G -n ubuntu-snap /dev/vg/ubuntu')
if (ret != 0):
	print "error"
	sys.exit (ret);

cursor.execute ('unlock tables')

# copy the datafiles in the snapshot area
ret = os.system ('sudo lvremove -f /dev/vg/ubuntu-snap')
if (ret != 0):
	print "error removing the snapshot"
	sys.exit (ret);
cursor.close ()
conn.close ()
