#!/bin/bash
#
# script to rotate based on dateformat, like logrotate does
#
# wfelipe <wfelipe@gmail.com>
#

#### config
ACCESSLOG=/export/logs/accesslog_80
OLDDIR=/export/logs_gz
GZIP=y
DATEFORMAT="%Y%m%d%H%M"
RENAMEFILE="`hostname`.accesslog."
KEEP_DAYS=1
LOCKFILE=/var/lock/subsys/varnishncsa_rotate
VARNISHNCSA_PID=/var/run/varnishncsa.pid
WAIT_FOR_ACCESSLOG=5

#### end of config - do not edit below

#
# main function
#
main () {
	lock
	wait=0
	check_vars

	# verify if file exists
	if [ ! -f ${ACCESSLOG} ]
	then
		echo "File does not exist ${ACCESSLOG}"
		unlock
		exit 1
	fi

	# if the file exists and has 0 byte, do nothing
	if [ ! -s ${ACCESSLOG} ]
	then
		echo "File has 0 byte and will not be rotated"
		unlock
		exit 0
	fi

	# checks if the file was already rotated
	logname="${RENAMEFILE}`date +${DATEFORMAT}`"
	currdir="`dirname ${ACCESSLOG}`"
	if [[ -f "${OLDDIR}/$logname" || -f "${OLDDIR}/$logname.gz" ]]
	then
		echo "Rotated file already exists: ${OLDDIR}/$logname*" >&2
		unlock
		exit 1
	fi

	# moves the file and send a signal to varnishncsa
	mv ${ACCESSLOG} $currdir/$logname
	/bin/kill -HUP `cat ${VARNISHNCSA_PID} 2>/dev/null` 2> /dev/null || true

	# awaits until varnishncsa creates the logfile and 
	# starts to write on it
	while [[ ! -f ${ACCESSLOG} && $wait -lt ${WAIT_FOR_ACCESSLOG} ]]
	do
		let "wait=$wait + 1"
		sleep 1
	done

	if [ $wait -eq ${WAIT_FOR_ACCESSLOG} ]
	then
		echo "Timeout exceeded waiting for accesslog ${ACCESSLOG}"
		unlock
		exit 3
	fi

	# move the rotated file to the OLDDIR
	if [ "$GZIP" == "y" ]
	then
		nice -20 gzip $currdir/$logname
		mv $currdir/$logname.gz ${OLDDIR}/$logname.gz
	else
		mv $currdir/$logname ${OLDDIR}/$logname
	fi
	purge_logs

	unlock
}

#
# auxiliar functions
#
lock () {
	if [ -f ${LOCKFILE} ]
	then
		echo "Another instance of $0 is running"
		exit 2
	fi
	touch ${LOCKFILE}
}
unlock () {
	rm -f ${LOCKFILE}
}

purge_logs () {
	date_i="`date +${DATEFORMAT} -d '1 days ago'`"

	files="`ls ${OLDDIR}/${RENAMEFILE}*`"
	for file in $files
	do
		file=`basename $file`
		if [[ "${RENAMEFILE}${date_i}" > "$file" ]]
		then
			echo "removendo o arquivo $file"
			rm -f ${OLDDIR}/$file
		fi
	done
}

# TODO - validades the variables
check_vars () {
	VARS="	ACCESSLOG \
		OLDDIR \
		GZIP \
		DATEFORMAT \
		RENAMEFILE \
		KEEP_DAYS \
		LOCKFILE \
		VARNISHNCSA_PID \
		WAIT_FOR_ACCESSLOG \
	"
}

main
