nfiles = len(svcMgr.EventSelector.InputCollections)
svcMgr.EventSelector.InputCollections = svcMgr.EventSelector.InputCollections[nfiles*4/5 : nfiles*5/5]
muonrawhits_run = muonrawhits_run+"_5"
