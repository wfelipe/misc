#!/bin/bash
#
#
# checks for httpd processes which consumes more than 50% of cpu slice
# and prints the stack using gdb
DATE=`date +"%s"`

[ -d "/export/logs/debug" ] || mkdir /export/logs/debug

for PID in `ps axuwww | grep httpd | grep -v grep | awk '{ if ($3 > 50) print $2; }'`
do
	cat << EOF | gdb /opt/apache/bin/httpd ${PID} >> /export/logs/debug/httpd.${PID}.${DATE}
where
detach
quit
EOF

done
