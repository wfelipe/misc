#!/usr/bin/env python
'''
'''
#
# wfelipe <wfelipe@gmail.com
import sys, os

#
# the headers that matter
headers = [
	'c-client-id',
	'date',
	'time',
	'cs-bytes',
	'sc-bytes',
	'x-duration',
	'c-referrer',
	'c-user-agent',
]

def process_file(file):
	fd = open(file, 'r')
	for line in fd:
		line = line.strip ()
		if (line.startswith('#Fields')):
			fields = line.split()[1:]
		if (line.startswith('#')): continue

		line = dict(zip(fields, line.split('\t')))
		if line['x-duration'] != '0' and line['x-event'] == 'disconnect' and line['x-category'] == 'session':
			print line
			lineout = ''
			for header in headers:
				lineout = lineout + str(line[header]) + ';'
			print lineout

headerout = ''
for header in headers:
	headerout = headerout + header + ';'
print headerout
for file in sys.argv[1:]:
	process_file(file)
