#
# steering for running MuonRawHits.
# run like:
# $ athena.py input/${run}.py muonrawhits.py 
#

# theApp.EvtMax = 10
svcMgr += CfgMgr.AthenaEventLoopMgr(EventPrintoutInterval=1000)

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput = svcMgr.EventSelector.InputCollections
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo = 'atlas'

from RecExConfig.InputFilePeeker import inputFileSummary
globalflags.DataSource = 'data' # if inputFileSummary['evt_type'][0] == "IS_DATA" else 'geant4'
globalflags.DetDescrVersion = inputFileSummary['geometry']

# e.g. if I am accessing CaloCellContainer, I need the calo detector description
from AthenaCommon.DetFlags import DetFlags
DetFlags.detdescr.Muon_setOn()
DetFlags.detdescr.ID_setOn()
include("RecExCond/AllDet_detDescr.py")

# Fetch the AthAlgSeq, i.e., one of the existing master sequences where one should attach all algorithms
algSeq = CfgMgr.AthSequencer("AthAlgSeq")
alg = CfgMgr.BaseAnalysis("SegmentAnalysis", OutputLevel=INFO);
ServiceMgr.MessageSvc.defaultLimit = 9999999
algSeq += alg

import time
svcMgr += CfgMgr.THistSvc()
svcMgr.THistSvc.Output += ["ANOTHERSTREAM DATAFILE='ntuple.%s.Run%s.root' OPT='RECREATE'" % (time.strftime('%Y-%m-%d-%Hh%Mm%Ss'),
                                                                                             muonrawhits_run,
                                                                                             )]

from LumiBlockComps.LuminosityToolDefault import LuminosityToolDefault
theLumiTool = LuminosityToolDefault()
from AthenaCommon.AppMgr import ToolSvc
ToolSvc += theLumiTool

from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
cfg = TriggerConfigGetter('ReadPool')

from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
tdt = Trig__TrigDecisionTool("TrigDecisionTool")
ToolSvc += tdt

from AthenaCommon.CfgGetter import getPublicTool,getPublicToolClone
CalibCscStripFitter = getPublicTool("CalibCscStripFitter")
ToolSvc += CalibCscStripFitter

