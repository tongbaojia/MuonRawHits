import AthenaPoolCnvSvc.ReadAthenaPool

muonrawhits_run = "minbias"
mcpath  = "/n/atlasfs/atlasdata/tuna/valid1.119994.Pythia8_A2MSTW2008LO_minbias_inelastic.recon.ESD.e3099_s2578_r7112/*"

import glob
svcMgr.EventSelector.InputCollections = sorted(glob.glob(mcpath))

