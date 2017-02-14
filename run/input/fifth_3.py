nfiles = len(svcMgr.EventSelector.InputCollections)
svcMgr.EventSelector.InputCollections = svcMgr.EventSelector.InputCollections[nfiles*2/5 : nfiles*3/5]
muonrawhits_run = muonrawhits_run+"_3"
