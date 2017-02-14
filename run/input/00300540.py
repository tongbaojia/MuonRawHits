import AthenaPoolCnvSvc.ReadAthenaPool

muonrawhits_run = "00300540"
dirpath = "/n/atlasfs/atlasdata/tuna/data16_13TeV_ZeroBias/data16_13TeV.%s.physics_ZeroBias.recon.ESD.*/" % (muonrawhits_run)

import glob
# svcMgr.EventSelector.InputCollections = sorted(glob.glob(dirpath+"*"))

svcMgr.EventSelector.InputCollections = ["/n/atlasfs/atlasdata/tuna/data16_13TeV_ZeroBias/data16_13TeV.00300540.physics_ZeroBias.recon.ESD.f705/data16_13TeV.00300540.physics_ZeroBias.recon.ESD.f705._lb0500._SFO-ALL._0001.1"]

# svcMgr.EventSelector.InputCollections = ["/n/atlasfs/atlasdata/tuna/data16_13TeV_ZeroBias/data16_13TeV.00300540.physics_ZeroBias.merge.RAW/data16_13TeV.00300540.physics_ZeroBias.merge.ESD._lb0500._SFO-ALL._0001.1"]




