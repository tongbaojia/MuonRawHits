nfiles = len(svcMgr.EventSelector.InputCollections)
svcMgr.EventSelector.InputCollections = svcMgr.EventSelector.InputCollections[nfiles*3/5 : nfiles*4/5]
muonrawhits_run = muonrawhits_run+"_4"
