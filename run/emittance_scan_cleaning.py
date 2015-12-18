import ROOT
ROOT.gROOT.SetBatch(True)

ROOT.gStyle.SetOptStat(0)
ROOT.gStyle.SetPadLeftMargin(0.18)
ROOT.gStyle.SetPadRightMargin(0.18)
ROOT.gStyle.SetPadTopMargin(0.06)
ROOT.gStyle.SetPadBottomMargin(0.14)

import os
import glob

esddir  = "/n/atlasfs/atlasdata/tuna/"
topdir  = "/n/atlasfs/atlasdata/tuna/MuonRawHits/batch-2015-12-10-12h33m42s"
summary = []

for rundir in sorted(glob.glob(os.path.join(topdir, "*"))):

    run = os.path.basename(rundir)
    if "280231" in run:
        continue

    tree = ROOT.TChain("physics")
    for rootfile in glob.glob(os.path.join(rundir, "*", "ntuple*.root")):
        tree.Add(rootfile)

    lbn_min  = tree.GetMinimum("lbn")
    lbn_max  = tree.GetMaximum("lbn")
    lumi_min = tree.GetMinimum("lbAverageLuminosity")
    lumi_max = tree.GetMaximum("lbAverageLuminosity")
    bad_lbs  = 0
    
    if not int(lbn_min) == lbn_min: print "FUCK FUCK"
    if not int(lbn_max) == lbn_max: print "FUCK FUCK"

    name = "lumi_vs_lb_%s" % run
    xaxis = "eventInfo->lumiBlock()"
    yaxis = "m_lumiTool->lbAverageLuminosity()"
    zaxis = "events"

    canvas = ROOT.TCanvas(name, name, 800, 800)
    lumi_vs_lb = ROOT.TH2F(name, ";%s;%s;%s" % (xaxis, yaxis, zaxis), 
                           int(lbn_max - lbn_min + 1), lbn_min-0.5, lbn_max+0.5, 50, lumi_min, lumi_max)
    tree.Draw("lbAverageLuminosity:lbn >> %s" % (name), "", "colz")

    lumis_end_of_run = []
    for xbin in xrange(lumi_vs_lb.GetNbinsX()-5, lumi_vs_lb.GetNbinsX()):
        for ybin in xrange(1, lumi_vs_lb.GetNbinsY()+1):
            if lumi_vs_lb.GetBinContent(lumi_vs_lb.GetBin(xbin, ybin)) > 0:
                lumis_end_of_run.append(lumi_vs_lb.GetYaxis().GetBinCenter(ybin))
    lumi_end_of_run = max(lumis_end_of_run)

    for xbin in xrange(1, lumi_vs_lb.GetNbinsX()+1):
        for ybin in xrange(1, lumi_vs_lb.GetNbinsY()+1):
            
            xbincenter = lumi_vs_lb.GetXaxis().GetBinCenter(xbin)
            ybincenter = lumi_vs_lb.GetYaxis().GetBinCenter(ybin)
            
            bin = lumi_vs_lb.FindBin(xbincenter, ybincenter)
            if lumi_vs_lb.GetBinContent(bin) > 0 and ybincenter < lumi_end_of_run:
                bad_lbs += 1
                for esd in glob.glob(os.path.join(esddir, "*%s*" % run, "*lb%04i*" % int(xbincenter))):
                    print "rm -f", esd

    summary.append("# Run %i :: %7i / %7i LBs are suspect" % (int(run), bad_lbs, int(lbn_max-lbn_min)))

    lumi_vs_lb.GetXaxis().SetNdivisions(505)
    lumi_vs_lb.GetXaxis().SetTitleSize(0.05)
    lumi_vs_lb.GetYaxis().SetTitleSize(0.05)
    lumi_vs_lb.GetZaxis().SetTitleSize(0.05)
    lumi_vs_lb.GetXaxis().SetLabelSize(0.05)
    lumi_vs_lb.GetYaxis().SetLabelSize(0.05)
    lumi_vs_lb.GetZaxis().SetLabelSize(0.05)
    lumi_vs_lb.GetXaxis().SetTitleOffset(1.2)
    lumi_vs_lb.GetYaxis().SetTitleOffset(1.7)
    lumi_vs_lb.GetZaxis().SetTitleOffset(1.3)

    print
    # canvas.SaveAs("lumi_vs_lb_%s.pdf" % run)

print
for line in summary:
    print line
print
