#!/usr/bin/env python
#
# wfelipe <wfelipe@gmail.com>
#

import sys, os
from optparse import OptionParser
import MySQLdb
import logging

def main():
    usage = "usage: %prog [options] lvm|nfs device"
    p = OptionParser(usage=usage)
    p.add_option("-u", "--username",
                 action="store", type="string", dest="username",
                 help="username to database")
    p.add_option("-p", "--password",
                 action="store", type="string", dest="password",
                 help="password to database")
    p.add_option("-s", "--size",
                 action="store", type="int", dest="size", default=1,
                 help="size of the snapshot, in GB")
    (options, args) = p.parse_args()

    if len(args) > 2 or len(args) < 2:
        p.error("two arguments are required\n")

    if args[0] != 'lvm' and args[0] != 'nfs':
        p.error("only lvm or nfs are allowed")

    do_backup(options, args)

def do_backup(options, args):
    args = dict()
    args['host'] = 'localhost'
    args['db'] = 'mysql'
    if options.username: args['user'] = options.username
    if options.password: args['passwd'] = options.password
    at = ('host', 'localhost')
    at = at + ('db', 'mysql')
    conn = MySQLdb.connect(*at)

    cursor = conn.cursor()
    cursor.execute('flush tables with read lock')
    cursor.execute('show slave status')
    row = cursor.fetchone()
    print row

    if args[0] == 'lvm':
        do_snapshot_lvm()
    elif args[1] == 'nfs':
        do_snapshot_nfs()

    cursor.execute ('unlock tables')

    if args[0] == 'lvm':
        do_backup_lvm()
    elif args[1] == 'nfs':
        do_backup_nfs()

    cursor.close()
    conn.close()

if __name__ == "__main__":
    main()

def do_snapshot_lvm():
    print 'do_snapshot_lvm'

def do_snapshop_nfs():
    print 'do_snapshot_nfs'

def do_backup_lvm():
    print 'do_backup_lvm'

def do_backup_nfs():
    print 'do_backup_nfs'

def connect_mysql(username=None, password=None):
    conn = MySQLdb.connect(host = 'localhost',
                           user,
                           passwd,
                           db)

    return conn

def flushlock(connection):
    cursor = connection.cursor()
    cursor.execute('flush tables with read lock')

    return cursor

def snapshot(lv):
    # lvcreate snapshot
    ret = os.system ('sudo lvcreate -s -L 5G -n ubuntu-snap /dev/vg/ubuntu')
    if (ret != 0):
        print "error"
        sys.exit (ret);

def unlock(cursor):
    cursor.execute ('unlock tables')

def remove_snap(snapshot):
    # copy the datafiles in the snapshot area
    ret = os.system ('sudo lvremove -f /dev/vg/ubuntu-snap')
    if (ret != 0):
        print "error removing the snapshot"
        sys.exit (ret);

def close_mysql():
    cursor.close ()
    conn.close ()

class LVM:
    def list_lv():
        pass
