"""
grl_cleaning.py: a script to kill files not in the GRL

python grl_cleaning.py --grl=${grl} --datadir=${datadir}
"""

import argparse
import glob
import os
import sys

try:
    from goodruns import GRL
except:
    print
    print "Please install the goodruns package for pythonic GRLs."
    print "https://pypi.python.org/pypi/goodruns/2.8.1"
    print
    sys.exit()

def options():
    parser = argparse.ArgumentParser()
    parser.add_argument("--grl",     help="Path to input GRL.")
    parser.add_argument("--datadir", help="Path to directory of data downloads.")
    return parser.parse_args()

def main():

    ops = options()

    if not ops.grl:     fatal("Please provide --grl")
    if not ops.datadir: fatal("Please provide --datadir")
    
    ops.grl     = os.path.abspath(ops.grl)
    ops.datadir = os.path.abspath(ops.datadir)
    print
    print "# GRL  = %s" % (os.path.basename(ops.grl))
    print "# data = %s" % (ops.datadir)
    print

    grl     = GRL(ops.grl)
    rundirs = glob.glob(os.path.join(ops.datadir, "*"))
    summary = []

    # walk the data directory for runs
    for rundir in sorted(rundirs):

        if not os.path.isdir(rundir):
            continue

        all_files = 0
        bad_files = 0

        basename = os.path.basename(rundir)

        if not basename.startswith("data15_13TeV"):
            continue

        run = int(basename.lstrip("data15_13TeV.").split(".")[0])

        # walk the run directory for files
        files = sorted(glob.glob(os.path.join(rundir, "*")))
        for fi in files:

            basename = os.path.basename(fi)
            lb = int(basename.split("_lb")[1].split(".")[0]) 

            all_files += 1
            if not (run, lb) in grl: 
                print "rm -f %s" % (fi)
                bad_files += 1

        summary.append("# Run %i :: %7i / %7i files are bad" % (run, bad_files, all_files))

    print
    for line in summary:
        print line
    print

def fatal(message):
    sys.exit("Error in %s: %s" % (__file__, message))

if __name__ == "__main__":
    main()
