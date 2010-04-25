#!/usr/bin/env python
#
#Author: Wilson Felipe <wfelipe@gmail.com>
#
"""
Flash Media Server log processor

Following the access log from the FMS:
http://help.adobe.com/en_US/FlashMediaServer/3.5_AdminGuide/WS5b3ccc516d4fbf351e63e3d119f2926bcf-79d1.html

it reads the files passed by argument, one by one,
filter the important fields and summarizes the output
in a csv file
"""

import sys, os
from datetime import datetime
import socket # to use gethostbyaddr

#
# the headers that matter
headers = [
	'c-client-id',
	'start',
	'end',
	'tz',
	'x-duration',
	'cs-bytes',
	'sc-bytes',
	'c-referrer',
	'c-user-agent',
	'c-ip',
]

session = dict ()

def process_file(file):
	fd = open(file, 'r')
	fields = []
	for line in fd:
		line = line.strip ()
		if (line.startswith('#Fields')):
			fields = line.split()[1:]
		if (line.startswith('#')): continue

		line = dict(zip(fields, line.split('\t')))

		sid = line['c-client-id']
		if sid not in session: session[sid] = {}
		if line['x-event'] == 'connect-pending' and line['x-category'] == 'session':
			#session[sid]['start'] = datetime.strptime(line['date'] + line['time'], '%Y-%m-%d%H:%M:%S')
			session[sid]['start'] = line['date'] + " " + line['time']
			session[sid]['tz'] = line['tz']
			session[sid]['cs-bytes'] = line['cs-bytes']
			session[sid]['sc-bytes'] = line['sc-bytes']
			session[sid]['c-user-agent'] = line['c-user-agent']
			session[sid]['c-referrer'] = line['c-referrer']
			session[sid]['c-ip'] = line['c-ip']

			session[sid]['x-duration'] = '-1'
			session[sid]['end'] = '-1'
		if line['x-event'] == 'disconnect' and line['x-category'] == 'session':
			session[sid]['x-duration'] = line['x-duration']
			#session[sid]['end'] = datetime.strptime(line['date'] + line['time'], '%Y-%m-%d%H:%M:%S')
			session[sid]['end'] = line['date'] + " " + line['time']
			session[sid]['cs-bytes'] = line['cs-bytes']
			session[sid]['sc-bytes'] = line['sc-bytes']

headerout = ''
for header in headers:
	headerout = headerout + header + ';'
print headerout
for file in sys.argv[1:]:
	process_file(file)

for sid in session:
	if 'start' in session[sid]:
		print "%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;" % (
			sid,
			session[sid]['start'],
			session[sid]['end'],
			session[sid]['tz'],
			session[sid]['x-duration'],
			session[sid]['cs-bytes'],
			session[sid]['sc-bytes'],
			session[sid]['c-referrer'],
			session[sid]['c-user-agent'],
			session[sid]['c-ip'],
	)
