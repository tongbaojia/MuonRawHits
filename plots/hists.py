"""
hists.py: script to make histograms from MuonRawHits ntuples. 

Run outside athena.

> python hists.py --input=input.root --output=output.root
"""

import argparse
import glob
import multiprocessing as mp
import os
import sys
import time
import warnings
warnings.filterwarnings(action="ignore", category=RuntimeWarning)

import ROOT
import rootlogon
ROOT.gROOT.SetBatch(True)

def options():
    parser = argparse.ArgumentParser()
    parser.add_argument("--input",       help="comma-separated, glob-able input root files")
    parser.add_argument("--output",      help="output root file")
    parser.add_argument("--cpus",        help="number of cpus")
    parser.add_argument("--events",      help="max number of events")
    parser.add_argument("--filesperjob", help="files per job (if applicable)")
    return parser.parse_args()

def main():

    ops = options()
    if not ops.input:
        fatal("Please give a comma-separated list of --input files (glob-capable)")
    if not ops.output:
        fatal("Please give an --output file")

    inputs = []
    for inp in ops.input.split(","):
        if "*" in inp:
            inputs.extend(glob.glob(inp))
        else:
            inputs.append(inp)

    hists = parallelize_histograms(inputs)

    output = ROOT.TFile.Open(ops.output, "recreate")
    for hist in hists:
        output.cd()
        hist.Write()
    output.Close()
        
def parallelize_histograms(files):

    ops = options()

    maxevents   = int(ops.events)      if ops.events      else None
    filesperjob = int(ops.filesperjob) if ops.filesperjob else 1
    cpus        = int(ops.cpus)        if ops.cpus        else 1
    configs     = []

    while files:
        iconfig = len(configs)
        configs.append(dict())
        configs[iconfig]["tree"]  = "physics"
        configs[iconfig]["job"]   = iconfig
        configs[iconfig]["max"]   = maxevents
        configs[iconfig]["files"] = []
        for ifpj in xrange(filesperjob):
            if files:
                configs[iconfig]["files"].append(files.pop(0))

    for iconfig, config in enumerate(configs):
        print " job", iconfig
        for fi in config["files"]:
            print " -", fi

    # map
    npool = min(len(configs), cpus, mp.cpu_count()-1)
    pool = mp.Pool(npool)
    results = pool.map(ntuple_to_histogram, configs)

    # reduce
    histograms = [rootsum(hists) for hists in zip(*results)]
    for hist in histograms:
        hist.SetName(hist.GetName().split("_job")[0])
    print

    return histograms

def ntuple_to_histogram(config):

    job   = config["job"]
    tree  = ROOT.TChain(config["tree"])
    for fi in config["files"]:
        tree.Add(fi)

    run     = run_number(tree)
    bunches = colliding_bunches(tree)
    hists   = initialize_histograms(run, job)
    hits    = {}
    entries = min(config["max"], tree.GetEntries()) or tree.GetEntries()

    # hits
    start_time = time.time()
    for entry in xrange(entries):

        if entry % 100 == 0 and entry > 0:
            progress(time.time() - start_time, entry, entries)

        tree.GetEntry(entry)
        hists["entries"].Fill(1)

        for region in hists:
            hits[region] = 0

        # mdt hits
        for ich in xrange(tree.mdt_chamber_n):

            nhits        = tree.mdt_chamber_tube_n[ich]
            chamber_type = tree.mdt_chamber_type[ich]
            eta_station  = tree.mdt_chamber_eta_station[ich]

            hits["mdt_all_orbit"] += nhits
            if chamber_type == "EIL":
                if   abs(eta_station) == 1: hits["mdt_EIL1_orbit"] += nhits
                elif abs(eta_station) == 2: hits["mdt_EIL2_orbit"] += nhits
            elif chamber_type == "EIS":
                if   abs(eta_station) == 1: hits["mdt_EIS1_orbit"] += nhits
                elif abs(eta_station) == 2: hits["mdt_EIS2_orbit"] += nhits

            if "EI" in chamber_type and abs(eta_station) in [1, 2]:

                if   chamber_type == "EIL": region = "endcap_L_hits"
                elif chamber_type == "EIS": region = "endcap_S_hits"
                else: 
                    continue

                for tube in xrange(nhits):
                    hists[region].Fill(tree.mdt_chamber_tube_r[ich][tube], 1)
                    
        for ich in xrange(tree.csc_chamber_n):

            nhits        = tree.csc_chamber_cluster_n[ich]
            chamber_type = tree.csc_chamber_type[ich]

            hits["csc_all_orbit"] += nhits
            if   chamber_type == "CSL": hits["csc_L_orbit"] += nhits
            elif chamber_type == "CSS": hits["csc_S_orbit"] += nhits
                
            if   chamber_type == "CSL": region = "endcap_L_hits"
            elif chamber_type == "CSS": region = "endcap_S_hits"

            for clus in xrange(nhits):
                hists[region].Fill(tree.csc_chamber_cluster_r[ich][clus], 1)
            
        lumi     = tree.lbAverageLuminosity/1000
        prescale = tree.prescale_HLT

        for region in hists:

            if "bunch" in region or "endcap" in region or "entries" in region:
                continue
            
            hists[region].Fill(lumi, hits[region], prescale)
            hists[region.replace("orbit", "bunch")].Fill(1000*lumi/bunches, hits[region], prescale)

    # alphabetize output
    return [hists[key] for key in sorted(hists.keys())]

def initialize_histograms(run, job):

    hists = {}

    hists["entries"] = ROOT.TH1F("entries_%s_job%s" % (run, job), "entries", 1, 0, 2)

    name = "mdt_all_vs_lumi_orbit_%s_job%s" % (run, job)
    xaxis = "< inst. lumi. per fill > [e^{33}_cm^{-2}_s^{-1}_]".replace("_", "#scale[0.5]{ }")
    yaxis = "hits in MDT"
    zaxis = "arbitrary units"

    hists["mdt_all_orbit"]  = ROOT.TH2F(name,                        ";%s;%s;%s" % (xaxis, yaxis, zaxis), 200, 0.0, 6.0, 200, 0, 5000)
    hists["mdt_EIL1_orbit"] = ROOT.TH2F(name.replace("all", "EIL1"), ";%s;%s;%s" % (xaxis, yaxis, zaxis), 200, 0.0, 6.0, 200, 0,  500)
    hists["mdt_EIL2_orbit"] = ROOT.TH2F(name.replace("all", "EIL2"), ";%s;%s;%s" % (xaxis, yaxis, zaxis), 200, 0.0, 6.0, 200, 0,  500)
    hists["mdt_EIS1_orbit"] = ROOT.TH2F(name.replace("all", "EIS1"), ";%s;%s;%s" % (xaxis, yaxis, zaxis), 200, 0.0, 6.0, 200, 0,  500)
    hists["mdt_EIS2_orbit"] = ROOT.TH2F(name.replace("all", "EIS2"), ";%s;%s;%s" % (xaxis, yaxis, zaxis), 200, 0.0, 6.0, 200, 0,  500)

    name_csc = name.replace("mdt", "csc")
    hists["csc_all_orbit"] = ROOT.TH2F(name_csc,                     ";%s;%s;%s" % (xaxis, yaxis.replace("MDT", "CSC"), zaxis), 200, 0.0, 6.0, 200,  0, 200)
    hists["csc_L_orbit"]   = ROOT.TH2F(name_csc.replace("all", "L"), ";%s;%s;%s" % (xaxis, yaxis.replace("MDT", "CSC"), zaxis), 200, 0.0, 6.0, 200,  0, 100)
    hists["csc_S_orbit"]   = ROOT.TH2F(name_csc.replace("all", "S"), ";%s;%s;%s" % (xaxis, yaxis.replace("MDT", "CSC"), zaxis), 200, 0.0, 6.0, 200,  0, 100)

    name  = name.replace("_lumi_orbit_", "_lumi_bunch_")
    xaxis = xaxis.replace("lumi. per fill", "lumi. per BC").replace("e^{33}", "e^{30}")
    hists["mdt_all_bunch"]  = ROOT.TH2F(name,                        ";%s;%s;%s" % (xaxis, yaxis, zaxis), 180, 1.0, 4.0, 200, 0, 5000)
    hists["mdt_EIL1_bunch"] = ROOT.TH2F(name.replace("all", "EIL1"), ";%s;%s;%s" % (xaxis, yaxis, zaxis), 180, 1.0, 4.0, 200, 0,  500)
    hists["mdt_EIL2_bunch"] = ROOT.TH2F(name.replace("all", "EIL2"), ";%s;%s;%s" % (xaxis, yaxis, zaxis), 180, 1.0, 4.0, 200, 0,  500)
    hists["mdt_EIS1_bunch"] = ROOT.TH2F(name.replace("all", "EIS1"), ";%s;%s;%s" % (xaxis, yaxis, zaxis), 180, 1.0, 4.0, 200, 0,  500)
    hists["mdt_EIS2_bunch"] = ROOT.TH2F(name.replace("all", "EIS2"), ";%s;%s;%s" % (xaxis, yaxis, zaxis), 180, 1.0, 4.0, 200, 0,  500)

    name_csc = name.replace("mdt", "csc")
    hists["csc_all_bunch"] = ROOT.TH2F(name_csc,                     ";%s;%s;%s" % (xaxis, yaxis.replace("MDT", "CSC"), zaxis), 180, 1.0, 4.0, 200, 0, 200)
    hists["csc_L_bunch"]   = ROOT.TH2F(name_csc.replace("all", "L"), ";%s;%s;%s" % (xaxis, yaxis.replace("MDT", "CSC"), zaxis), 180, 1.0, 4.0, 200, 0, 100)
    hists["csc_S_bunch"]   = ROOT.TH2F(name_csc.replace("all", "S"), ";%s;%s;%s" % (xaxis, yaxis.replace("MDT", "CSC"), zaxis), 180, 1.0, 4.0, 200, 0, 100)

    for quantity in ["hits"]:
        for sector in ["S", "L"]:

            name = "endcap_%s_%s_%s_job%s" % (sector, quantity, run, job)
            xaxis = "radius [mm]"
            yaxis = "%s sectors: %s %s" % (sector, quantity, unit(quantity))
            hists["endcap_%s_%s" % (sector, quantity)] = ROOT.TH1F(name, ";%s;%s;" % (xaxis, yaxis), 500, 0, endcap_xaxis_max(name))

    for hist in hists:
        hists[hist].Sumw2()
        ROOT.SetOwnership(hists[hist], False)

    return hists

def endcap_xaxis_max(histname):
    if "_L_" in histname: return 5200
    if "_S_" in histname: return 5440
    fatal("Bad histname for endcap_xaxis_max: %s" % (histname))

def hadd(output, inputs, delete=False):
    command = ["hadd", output] + inputs
    print command
    subprocess.call(command)
    print "rm -f %s" % (" ".join(inputs))

def progress(time_diff, nprocessed, ntotal):
    nprocessed = float(nprocessed)
    ntotal     = float(ntotal)
    rate = (nprocessed+1)/time_diff
    sys.stdout.write("\r > %6i / %6i | %2i%% | %8.1fHz | %6.1fm elapsed | %6.1fm remaining" % 
                     (nprocessed, ntotal, 100*nprocessed/ntotal, rate, time_diff/60, (ntotal-nprocessed)/(rate*60)))
    sys.stdout.flush()

def unit(quantity):
    if quantity == "area": return "[ m^{2} ]"
    if quantity == "time": return "[ s ]"
    return ""

def fatal(message):
    sys.exit("Error in %s: %s" % (__file__, message))

def run_number(tree):
    tree.GetEntry(1)
    return tree.RunNumber

def colliding_bunches(tree):
    tree.GetEntry(1)
    return tree.colliding_bunches

def rootsum(objects):
    if not objects: 
        fatal("cannot sum empty list")
    total = objects[0]
    for obj in objects[1:]:
        total.Add(obj)
    return total

if __name__ == "__main__":
    main()
