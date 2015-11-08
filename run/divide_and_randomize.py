"""
divide_and_randomize.py: a script to divide and randomize input TTrees.

Shoutout to the godkunkle for originally writing this.
"""

import argparse
import glob
import sys
import os
import random
import math

parser = argparse.ArgumentParser(usage="Randomize and split root trees")

parser.add_argument("--input",    help="path to input files, comma-separated")
parser.add_argument("--output",   help="path to output dirctory")
parser.add_argument("--treename", help="name of tree to read and split")
parser.add_argument("--chunks",   help="number of output files to create")

options = parser.parse_args()

import ROOT

def main():

    if not options.input:  fatal("Must provide --input files")
    if not options.output: fatal("Must provide an --output directory")
    if not options.chunks: fatal("Must provide number of --chunks")
    treename = options.treename or "physics"
    chunks   = int(options.chunks)

    # configure input
    inputs = options.input.split(",")
    chain = ROOT.TChain(treename)
    for inp in inputs:
        if "*" in inp:
            _ = [chain.Add(inpglob) for inpglob in glob.glob(inp)]
        else:
            chain.Add(inp)
    entries = chain.GetEntries()
    if not entries:
        fatal("no entries in the input tree")
    else:
        print " split %s input files into %s output files" % (len(chain.GetListOfFiles()), chunks)
        print " input entries for %s :: %s"                % (chain.GetName(), entries)

    # configure output
    if os.path.isdir(options.output): 
        fatal("output directory already exists: %s" % (options.output))
    else:
        os.makedirs(options.output)

    outputs = []
    for chunk in xrange(chunks):

        filepath = os.path.join(options.output, "ntuple_%04d.root" % (chunk))
        outputs.append(ROOT.TFile.Open(filepath, "RECREATE"))

    # the randomizer
    random_entries  = random.sample(xrange(0, entries), entries)
    entries_per_job = int(math.ceil(float(entries)/chunks))
    split_entries   = [random_entries[ent : ent+entries_per_job] for ent in range(0, entries, entries_per_job)]
    
    if len(split_entries) != chunks:
        fatal("failed to configure: n(jobs) != n(chunks)")

    process(chain, split_entries, outputs)

    chain.Reset()
    for output in outputs:
        output.Write()
        output.Close()

    print
    print " Done! ^.^"
    print

def process(chain, list_of_entry_lists, outputs):

    if len(list_of_entry_lists) != len(outputs):
        fatal("number of input event lists does not match number of outputs")

    entries = chain.GetEntries()

    clones = []
    for output in outputs:
        clone = chain.CloneTree(0)
        clone.SetDirectory(output)
        print " output", output.GetName(), clone
        clones.append(clone)

    # this is strictly for performance
    # ROOT prefers looping over trees sequentially (0, 1, 2), not randomly
    entry_to_clone = {}
    for iclone, entry_list in enumerate(list_of_entry_lists):
        for entry in entry_list:
            entry_to_clone[entry] = iclone

    for entry in xrange(entries):
        if entry and entry % 10000 == 0: 
            print " %10s / %10s" % (entry, entries)
        chain.GetEntry(entry)
        clones[entry_to_clone[entry]].Fill()

    for output, clone in zip(outputs, clones):
        output.cd()
        clone.Write()
        print " output", output.GetName(), clone.GetEntries()

def fatal(message):
    sys.exit("Error in %s: %s" % (__file__, message))

if __name__ == "__main__":
    main()
