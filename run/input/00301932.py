import AthenaPoolCnvSvc.ReadAthenaPool

muonrawhits_run = "00301932"
dirpath = "/n/atlasfs/atlasdata/tuna/data16_13TeV_ZeroBias/data16_13TeV.%s.physics_ZeroBias.recon.ESD.*/" % (muonrawhits_run)

import glob
# svcMgr.EventSelector.InputCollections = sorted(glob.glob(dirpath+"data16_13TeV.00301932.*"))

shit_fuck = glob.glob(dirpath+"round2/data16_13TeV.00301932.*_lb01*") + glob.glob(dirpath+"round2/data16_13TeV.00301932.*_lb02*") + glob.glob(dirpath+"round2/data16_13TeV.00301932.*_lb12*")
svcMgr.EventSelector.InputCollections = sorted(shit_fuck)
