#!/usr/bin/env python
#
#
#
import sys
import os
import time
import traceback
import threading
import socket
import select
import paramiko
import threading
import getopt

server = 'localhost'
port = 22
username = 'username'
password = 'password'
enable = 'enable'

try:
	sock = socket.socket (socket.AF_INET, socket.SOCK_STREAM)
	sock.connect ((server, port))

	paramiko.util.log_to_file ('transport.log')

	transport = paramiko.Transport (sock)
	transport.start_client ()
	transport.auth_password (username, password)

	chan = transport.open_session ()
	chan.set_combine_stderr (True)
	chan.get_pty ()
	chan.invoke_shell ()
	
	def commands (chan):
		chan.send ('enable\n')
		chan.send (enable)
		chan.send ('terminal length 0\n')
		chan.send ('show version\n')
		chan.send ('quit\n')

	cmds = threading.Thread (target=commands, args=(chan,))
	cmds.start ()

	while True:
		r, w, e = select.select ([chan], [], [])
		if chan in r:
			try:
				x = chan.recv (1024)
				if len (x) == 0:
					print '***EOF\n\r',
					break
				sys.stdout.write (x)
				sys.stdout.flush ()
			except socket.timeout:
				print 'socket.timeout'
				pass
	print "%s" % (x)

#	chan.exec_command ('help')
#	stdin = paramiko.ChannelFile (chan, 'w')
#	stdout = paramiko.ChannelFile (chan, 'r')
#
#	print "%s" % (stdout.read ())
#
#	chan.exec_command ('help')
#	print "%s" % (stdout.read ())

	chan.close ()
	transport.close ()

except:
	traceback.print_exc ()
	sys.exit (1)


