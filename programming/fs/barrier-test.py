#!/usr/bin/env python
#
# This is a test program meant to heavily exercise log and metadata
# writeback.  If you combine this with power failures, it should result
# in filesystem corruptions a significant percent of the time.
#
# The workload has 4 parts:
#
# 1) A directory tree full of empty files with very long names (240 chars)
# 2) A process hogging a significant percent of system ram.  This must
#    be enough to force constant metadata writeback due to memory pressure
# 3) A process constantly writing, fsyncing and truncating to zero a single 64k
#    file
# 4) A process constantly renaming the files with very long names from (1) 
#    between long-named-file.0 and long-named-file.1
#
# In order to consistently cause corruptions, the size of the directory from
# (1) needs to be at least as large as the ext3 log.  This is controlled with
# the -s command line option
#
# The amount of memory pinned down is controlled with -p
#
# Usage:
# The program first creates the files in a directory called barrier-test
# then it starts procs to pin ram and run the constant fsyncs.  After
# each phase has run long enough, they print out a statement about
# being ready, along with some other debugging output:
#
# Memory pin ready
# fsyncs ready
# Renames ready
#
# Once you see all three ready lines, turn off power.  Don't use the
# power button on the front of the machine, either pull the plug, use
# the power switch on the back of the machine, or use an external controller
#
# Written by Chris Mason <chris.mason@oracle.com>

import sys, os, sha, random, mmap
from optparse import OptionParser

total_name_bytes = 0
counter = 0
errors = 0
sha = sha.new()
salt = file("/dev/urandom").read(256)
sha.update(salt)
VERSION = "0.2"

def read_files(basedir, base_names):
    global total_name_bytes
    global counter
    global errors

    for x in os.listdir(basedir):
        total_name_bytes += len(x)
        counter += 1

        full = os.path.join(basedir, x)
        if not full.endswith(".0") and not full.endswith(".1"):
            continue
        num = int(x[-1])
        short = x[0:-2]
        if short in base_names:
            sys.stderr.write("warning: conflict on %s first %d dup %d\n" %
                            (short[0:10], base_names[short], num))
            errors += 1
        else:
            base_names[short] = num

def create_files(sha, basedir, base_names, size):
    global total_name_bytes
    global counter

    while total_name_bytes < size:
        s = str(counter)
        counter += 1

        sha.update(s)
        s = sha.hexdigest() * 6
        total_name_bytes += len(s)
        base_names[s] = 0
        fp = file(os.path.join(basedir, s + ".0"), 'w')
        fp.close()
        if counter % 10000 == 0:
            print "%d files %d MB total" % (counter,
                                            total_name_bytes / (1024 * 1024))

def run_fsyncs(basedir):
    pid = os.fork()
    if pid:
        os.waitpid(pid, os.WNOHANG)
        return
    fp = file(os.path.join(basedir, "bigfile"), 'w')
    buf = "a" * (64 * 1024)
    operations = 0
    while True:
        fp.write(buf)
        os.fsync(fp.fileno())
        fp.truncate(0)
        fp.seek(0)
        operations += 1
        if operations % 100 == 0:
            sys.stderr.write("f:%d " % operations)
            if operations == 300:
                sys.stderr.write("\nfsyncs ready\n")

def run_hog(hogmb):
    pid = os.fork()
    if pid:
        os.waitpid(pid, os.WNOHANG)
        return

    fp = file("/dev/zero", 'w+')
    hogmb *= 1024 * 1024
    mm = mmap.mmap(fp.fileno(), hogmb, mmap.MAP_PRIVATE,
                   mmap.PROT_READ | mmap.PROT_WRITE)
    operations = 0
    pos = 0
    didprint = 0
    buf = 'b' * 1024 * 1024
    while True:
        mm.write(buf)
        if mm.tell() >= hogmb:
            mm.seek(0)
            pos = 0
            operations += 1
            if not didprint:
                sys.stderr.write("\nMemory pin ready\n")
                didprint = 1

def run_renames(basedir, base_names):
    keys = base_names.keys()
    operations = 0
    while True:
        name = random.choice(keys)
        num = base_names[name]
        next = (num + 1) % 2
        os.rename(os.path.join(basedir, name + "." + str(num)),
                  os.path.join(basedir, name + "." + str(next)))
        base_names[name] = next
        operations += 1
        if operations % 1000 == 0:
            sys.stderr.write("r:%d " % operations)
            if operations == 10000:
                sys.stderr.write("\nRenames ready\n")

        
base_names = {}
usage = "usage: %prog [options]"
parser = OptionParser(usage=usage)
parser.add_option("-i", "--init", help="Init directory", default=False,
                  action="store_true")
parser.add_option("-d", "--dir", help="working dir", default="barrier-test")
parser.add_option("-s", "--size", help="Working set in MB",
                  type="int", default=32)
parser.add_option("-c", "--check", help="Check directory only",
                  default=False, action="store_true")
parser.add_option("-p", "--pin-mb", help="Amount of ram (MB) to pin",
                  default=512, type="int")
(options, args) = parser.parse_args()

sys.stderr.write("barrier-test version %s\n" % VERSION)
sys.stderr.write("Run init, don't cut the power yet\n")

if not os.path.exists(options.dir):
    os.makedirs(options.dir, 0700)
    options.init = True
else:
    read_files(options.dir, base_names)
    print "found %d files %d MB in %s" % (counter,
                                          total_name_bytes / (1024 * 1024),
                                          options.dir)
options.size *= 1024 * 1024
if options.check:
    print "Check complete found %d errors" % errors
    if errors:
        sys.exit(1)
    else:
        sys.exit(0)

if total_name_bytes < options.size:
    create_files(sha, options.dir, base_names, options.size)
    
sys.stderr.write("Starting metadata operations now\n")
run_fsyncs(options.dir)
run_hog(options.pin_mb)
run_renames(options.dir, base_names)
