nfiles = len(svcMgr.EventSelector.InputCollections)
svcMgr.EventSelector.InputCollections = svcMgr.EventSelector.InputCollections[nfiles*1/5 : nfiles*2/5]
muonrawhits_run = muonrawhits_run+"_2"
