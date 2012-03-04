#!/bin/bash
#
#
#
# wfelipe <wfelipe@gmail.com>
#

PATH=$PATH:/opt/mysql/meb-3.6/bin
MYBKP_COMMON_ARGS='-u root -p'

path=""
full=0
inc=0
apply=0
lastbkp_path=""
databases=""

usage()
{
    if [ "$1" ]; then
	echo "ERROR: $1"
    fi
    cat<<EOF>&2
usage:
  $0 -p path -f      full backup
  $0 -p path -i      incremental backup
  $0 -p path -a      apply incremental backups to full
EOF
    exit
}

logit()
{
    echo "mysql-backup: $1" >&2
}

runit()
{
    echo "runit"
}

mysql_full()
{
    logit 'performing a full backup'
    date="`date +full-%Y%m%d-%H%M`"
    logit "final path is $path/$date"
    # mysqlbackup doesn't work with --databases
    mysqlbackup \
	$MYBKP_COMMON_ARGS \
	--slave-info \
	--compress \
	--backup-dir="$path/$date/" \
	backup
}

get_last_backup()
{
    last_backup="`ls -td $path/inc-* $path/full-* 2> /dev/null | head -1`"

    echo "$last_backup" > /tmp/xx
    if [ ! -r "$last_backup/meta/backup_variables.txt" ]; then
	echo ""
	exit
    fi

    echo "$last_backup"
}

get_last_lsn()
{
    last_backup=$(get_last_backup)
    if [ -z "$last_backup" ]; then
	echo 0
    fi

    end_lsn=`grep end_lsn $last_backup/meta/backup_variables.txt 2> /dev/null | \
	awk -F\= '{ print $2 }'`

    echo $end_lsn
}

mysql_inc()
{
    logit "performing a incremental backup"
    date="`date +inc-%Y%m%d-%H%M`"
    logit "final path is $path/$date"

    lsn=$(get_last_lsn)
    if [ ! $lsn -gt 0 ]; then
	logit "invalid lsn"
    fi

    mysqlbackup \
	$MYBKP_COMMON_ARGS \
	--slave-info \
	--incremental --start-lsn=$lsn \
	--incremental-backup-dir="$path/$date/" \
	backup
}

mysql_apply()
{
    logit "apply"
    # get last full backup
    backups="`ls -td $path/inc-* $path/full-* 2> /dev/null`"
    inc_backups=""
    full_backup=""
    for bkp in $backups
    do
	if [[ $bkp == *full* ]]
	then
	    full_backup=$bkp
	    break
	fi
	inc_backups="$bkp $inc_backups"
    done

    if [ -z "$inc_backups" ]
    then
	logit "no incremental backups available"
	exit 1
    fi

    if [ -z "$full_backup" ]
    then
	logit "no full backup available"
	exit 2
    fi

    logit "full backup: $full_backup"
    logit "incremental backups: $inc_backups"

    # is full backup ready for apply-log?
    if [ `grep apply_log_done $full_backup/meta/backup_variables.txt | awk -F\= '{ print $2 }'` -eq 0 ]
    then
	if [ `grep is_compressed $full_backup/meta/backup_variables.txt | awk -F\= '{ print $2 }'` -eq 1 ]
	then
	    mysqlbackup \
		$MYBKP_COMMON_ARGS \
		--uncompress \
		--backup-dir=$full_backup \
		apply-log
	else
	    mysqlbackup \
		$MYBKP_COMMON_ARGS \
		--backup-dir=$full_backup \
		apply-log
	fi
    fi

    for inc in $inc_backups
    do
	logit "applying $inc into $full_backup"
	mysqlbackup \
	    $MYBKP_COMMON_ARGS \
	    --incremental-backup-dir=$inc \
	    --backup-dir=$full_backup \
	    apply-incremental-backup

	logit "removing inc directory $inc, no longer needed"
	rm -rf $inc
    done
}

mysql_restore()
{
    logit "restore"
}

#
# main
#
while getopts p:fihc:a o
do
    case "$o" in
	p)
	    path="$OPTARG"
	    ;;
	f)
	    full=1
	    ;;
	i)
	    inc=1
	    ;;
	a)
	    apply=1
	    ;;
	h)
	    usage
	    ;;
	[?])
	    usage
	    ;;
    esac
done

let S="$OPTIND-1"
shift $S

#
# arguments
#
# path mandatory
[ "$path" = "" ] && usage
# either one: full or inc
[ $full -eq 1 ] && [ $inc -eq 1 ] && [ $apply -eq 1 ] && usage "only one operation can be performed (full or inc)"
[ $full -eq 1 ] || [ $inc -eq 1 ] || [ $apply -eq 1 ] || usage "please choose one operation"

#
# sanity checks
#
[ -d $path ] && [ -w $path ] || usage "directory $path is not writable or doesn't exist"

#if [ -z "$@" ]; then
#    databases=""
#else
#    for d in $@; do
#	databases=" --include='$d\..*'"
#    done
#    databases="--databases=\"$@\""
#fi

[ $full -eq 1 ] && mysql_full
[ $inc -eq 1 ] && mysql_inc
[ $apply -eq 1 ] && mysql_apply
