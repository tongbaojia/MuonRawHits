import AthenaPoolCnvSvc.ReadAthenaPool

muonrawhits_run = "00304128"
dirpath = "/n/atlasfs/atlasdata/tuna/data16_13TeV_ZeroBias/data16_13TeV.%s.physics_ZeroBias.recon.ESD.*/" % (muonrawhits_run)

import glob
svcMgr.EventSelector.InputCollections = sorted(glob.glob(dirpath+"data16_13TeV.*"))
