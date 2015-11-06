import AthenaPoolCnvSvc.ReadAthenaPool

muonrawhits_run = "00280614"
dirpath = "/n/atlasfs/atlasdata/tuna/data15_13TeV.%s.physics_ZeroBias.recon.ESD.f629/" % (muonrawhits_run)

import glob
svcMgr.EventSelector.InputCollections = sorted(glob.glob(dirpath+"*"))

