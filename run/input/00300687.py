import AthenaPoolCnvSvc.ReadAthenaPool

muonrawhits_run = "00300687"
dirpath = "/n/atlasfs/atlasdata/tuna/data16_13TeV_ZeroBias/data16_13TeV.%s.physics_ZeroBias.recon.ESD.*/" % (muonrawhits_run)

import glob
# svcMgr.EventSelector.InputCollections = sorted(glob.glob(dirpath+"*"))
# svcMgr.EventSelector.InputCollections = ["/n/atlasfs/atlasdata/tuna/data16_13TeV_ZeroBias/data16_13TeV.00300687.physics_ZeroBias.recon.ESD.f708._lb0500._SFO-ALL._0001.1"]

svcMgr.EventSelector.InputCollections = sorted(glob.glob("/n/atlasfs/atlasdata/tuna/data16_13TeV_ZeroBias/fuck/round2/data16_13TeV.00300687.*"))

