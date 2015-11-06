theApp.EvtMax = 500
svcMgr += CfgMgr.AthenaEventLoopMgr(EventPrintoutInterval=100)

# Needed to locate input file
import AthenaPoolCnvSvc.ReadAthenaPool

# eospath = "root://eosatlas.cern.ch///eos/atlas/atlastier0/rucio/data15_13TeV/physics_ZeroBias/00280464/data15_13TeV.00280464.physics_ZeroBias.recon.ESD.f629/"
# eospath = "root://eosatlas.cern.ch///eos/atlas/atlastier0/rucio/data15_13TeV/physics_ZeroBias/00280673/data15_13TeV.00280673.physics_ZeroBias.recon.ESD.f629/"

eospath = "/n/atlasfs/atlasdata/tuna/data15_13TeV.00280464.physics_ZeroBias.recon.ESD.f629/"

svcMgr.EventSelector.InputCollections = [

    eospath+"data15_13TeV.00280464.physics_ZeroBias.recon.ESD.f629._lb0500._SFO-ALL._0001_000.1",
    eospath+"data15_13TeV.00280464.physics_ZeroBias.recon.ESD.f629._lb0500._SFO-ALL._0001_001.1",
    eospath+"data15_13TeV.00280464.physics_ZeroBias.recon.ESD.f629._lb0500._SFO-ALL._0001_002.1",
    eospath+"data15_13TeV.00280464.physics_ZeroBias.recon.ESD.f629._lb0500._SFO-ALL._0001_003.1",

    # eospath+"data15_13TeV.00280464.physics_ZeroBias.recon.ESD.f629._lb0430._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280464.physics_ZeroBias.recon.ESD.f629._lb0430._SFO-ALL._0001_001.1",
    # eospath+"data15_13TeV.00280464.physics_ZeroBias.recon.ESD.f629._lb0440._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280464.physics_ZeroBias.recon.ESD.f629._lb0440._SFO-ALL._0001_001.1",
    # eospath+"data15_13TeV.00280464.physics_ZeroBias.recon.ESD.f629._lb0450._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280464.physics_ZeroBias.recon.ESD.f629._lb0450._SFO-ALL._0001_001.1",
    # eospath+"data15_13TeV.00280464.physics_ZeroBias.recon.ESD.f629._lb0460._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280464.physics_ZeroBias.recon.ESD.f629._lb0460._SFO-ALL._0001_001.1",
    # eospath+"data15_13TeV.00280464.physics_ZeroBias.recon.ESD.f629._lb0470._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280464.physics_ZeroBias.recon.ESD.f629._lb0470._SFO-ALL._0001_001.1",
    # eospath+"data15_13TeV.00280464.physics_ZeroBias.recon.ESD.f629._lb0480._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280464.physics_ZeroBias.recon.ESD.f629._lb0480._SFO-ALL._0001_001.1",
    # eospath+"data15_13TeV.00280464.physics_ZeroBias.recon.ESD.f629._lb0490._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280464.physics_ZeroBias.recon.ESD.f629._lb0490._SFO-ALL._0001_001.1",
    # eospath+"data15_13TeV.00280464.physics_ZeroBias.recon.ESD.f629._lb0500._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280464.physics_ZeroBias.recon.ESD.f629._lb0500._SFO-ALL._0001_001.1",
    # eospath+"data15_13TeV.00280464.physics_ZeroBias.recon.ESD.f629._lb0510._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280464.physics_ZeroBias.recon.ESD.f629._lb0510._SFO-ALL._0001_001.1",
    # eospath+"data15_13TeV.00280464.physics_ZeroBias.recon.ESD.f629._lb0520._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280464.physics_ZeroBias.recon.ESD.f629._lb0520._SFO-ALL._0001_001.1",
    # eospath+"data15_13TeV.00280464.physics_ZeroBias.recon.ESD.f629._lb0530._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280464.physics_ZeroBias.recon.ESD.f629._lb0530._SFO-ALL._0001_001.1",
    # eospath+"data15_13TeV.00280464.physics_ZeroBias.recon.ESD.f629._lb0540._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280464.physics_ZeroBias.recon.ESD.f629._lb0540._SFO-ALL._0001_001.1",
    # eospath+"data15_13TeV.00280464.physics_ZeroBias.recon.ESD.f629._lb0550._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280464.physics_ZeroBias.recon.ESD.f629._lb0550._SFO-ALL._0001_001.1",
    # eospath+"data15_13TeV.00280464.physics_ZeroBias.recon.ESD.f629._lb0560._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280464.physics_ZeroBias.recon.ESD.f629._lb0560._SFO-ALL._0001_001.1",
    # eospath+"data15_13TeV.00280464.physics_ZeroBias.recon.ESD.f629._lb0570._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280464.physics_ZeroBias.recon.ESD.f629._lb0570._SFO-ALL._0001_001.1",
    # eospath+"data15_13TeV.00280464.physics_ZeroBias.recon.ESD.f629._lb0580._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280464.physics_ZeroBias.recon.ESD.f629._lb0580._SFO-ALL._0001_001.1",
    # eospath+"data15_13TeV.00280464.physics_ZeroBias.recon.ESD.f629._lb0590._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280464.physics_ZeroBias.recon.ESD.f629._lb0590._SFO-ALL._0001_001.1",
    # eospath+"data15_13TeV.00280464.physics_ZeroBias.recon.ESD.f629._lb0600._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280464.physics_ZeroBias.recon.ESD.f629._lb0600._SFO-ALL._0001_001.1",
    # eospath+"data15_13TeV.00280464.physics_ZeroBias.recon.ESD.f629._lb0610._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280464.physics_ZeroBias.recon.ESD.f629._lb0610._SFO-ALL._0001_001.1",
    # eospath+"data15_13TeV.00280464.physics_ZeroBias.recon.ESD.f629._lb0620._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280464.physics_ZeroBias.recon.ESD.f629._lb0620._SFO-ALL._0001_001.1",
    # eospath+"data15_13TeV.00280464.physics_ZeroBias.recon.ESD.f629._lb0630._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280464.physics_ZeroBias.recon.ESD.f629._lb0630._SFO-ALL._0001_001.1",
    # eospath+"data15_13TeV.00280464.physics_ZeroBias.recon.ESD.f629._lb0640._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280464.physics_ZeroBias.recon.ESD.f629._lb0640._SFO-ALL._0001_001.1",
    # eospath+"data15_13TeV.00280464.physics_ZeroBias.recon.ESD.f629._lb0650._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280464.physics_ZeroBias.recon.ESD.f629._lb0650._SFO-ALL._0001_001.1",
    # eospath+"data15_13TeV.00280464.physics_ZeroBias.recon.ESD.f629._lb0660._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280464.physics_ZeroBias.recon.ESD.f629._lb0660._SFO-ALL._0001_001.1",
    # eospath+"data15_13TeV.00280464.physics_ZeroBias.recon.ESD.f629._lb0670._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280464.physics_ZeroBias.recon.ESD.f629._lb0670._SFO-ALL._0001_001.1",
    # eospath+"data15_13TeV.00280464.physics_ZeroBias.recon.ESD.f629._lb0680._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280464.physics_ZeroBias.recon.ESD.f629._lb0680._SFO-ALL._0001_001.1",
    # eospath+"data15_13TeV.00280464.physics_ZeroBias.recon.ESD.f629._lb0690._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280464.physics_ZeroBias.recon.ESD.f629._lb0690._SFO-ALL._0001_001.1",
    # eospath+"data15_13TeV.00280464.physics_ZeroBias.recon.ESD.f629._lb0700._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280464.physics_ZeroBias.recon.ESD.f629._lb0700._SFO-ALL._0001_001.1",
    # eospath+"data15_13TeV.00280464.physics_ZeroBias.recon.ESD.f629._lb0710._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280464.physics_ZeroBias.recon.ESD.f629._lb0710._SFO-ALL._0001_001.1",
    # eospath+"data15_13TeV.00280464.physics_ZeroBias.recon.ESD.f629._lb0720._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280464.physics_ZeroBias.recon.ESD.f629._lb0720._SFO-ALL._0001_001.1",
    # eospath+"data15_13TeV.00280464.physics_ZeroBias.recon.ESD.f629._lb0730._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280464.physics_ZeroBias.recon.ESD.f629._lb0730._SFO-ALL._0001_001.1",
    # eospath+"data15_13TeV.00280464.physics_ZeroBias.recon.ESD.f629._lb0740._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280464.physics_ZeroBias.recon.ESD.f629._lb0740._SFO-ALL._0001_001.1",
    # eospath+"data15_13TeV.00280464.physics_ZeroBias.recon.ESD.f629._lb0750._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280464.physics_ZeroBias.recon.ESD.f629._lb0750._SFO-ALL._0001_001.1",
    # eospath+"data15_13TeV.00280464.physics_ZeroBias.recon.ESD.f629._lb0760._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280464.physics_ZeroBias.recon.ESD.f629._lb0760._SFO-ALL._0001_001.1",
    # eospath+"data15_13TeV.00280464.physics_ZeroBias.recon.ESD.f629._lb0770._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280464.physics_ZeroBias.recon.ESD.f629._lb0770._SFO-ALL._0001_001.1",
    # eospath+"data15_13TeV.00280464.physics_ZeroBias.recon.ESD.f629._lb0780._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280464.physics_ZeroBias.recon.ESD.f629._lb0780._SFO-ALL._0001_001.1",

    # eospath+"data15_13TeV.00280673.physics_ZeroBias.recon.ESD.f629._lb0200._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280673.physics_ZeroBias.recon.ESD.f629._lb0210._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280673.physics_ZeroBias.recon.ESD.f629._lb0220._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280673.physics_ZeroBias.recon.ESD.f629._lb0230._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280673.physics_ZeroBias.recon.ESD.f629._lb0240._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280673.physics_ZeroBias.recon.ESD.f629._lb0250._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280673.physics_ZeroBias.recon.ESD.f629._lb0260._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280673.physics_ZeroBias.recon.ESD.f629._lb0270._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280673.physics_ZeroBias.recon.ESD.f629._lb0280._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280673.physics_ZeroBias.recon.ESD.f629._lb0290._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280673.physics_ZeroBias.recon.ESD.f629._lb0300._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280673.physics_ZeroBias.recon.ESD.f629._lb0310._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280673.physics_ZeroBias.recon.ESD.f629._lb0320._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280673.physics_ZeroBias.recon.ESD.f629._lb0330._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280673.physics_ZeroBias.recon.ESD.f629._lb0340._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280673.physics_ZeroBias.recon.ESD.f629._lb0350._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280673.physics_ZeroBias.recon.ESD.f629._lb0360._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280673.physics_ZeroBias.recon.ESD.f629._lb0370._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280673.physics_ZeroBias.recon.ESD.f629._lb0380._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280673.physics_ZeroBias.recon.ESD.f629._lb0390._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280673.physics_ZeroBias.recon.ESD.f629._lb0400._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280673.physics_ZeroBias.recon.ESD.f629._lb0410._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280673.physics_ZeroBias.recon.ESD.f629._lb0420._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280673.physics_ZeroBias.recon.ESD.f629._lb0430._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280673.physics_ZeroBias.recon.ESD.f629._lb0440._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280673.physics_ZeroBias.recon.ESD.f629._lb0450._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280673.physics_ZeroBias.recon.ESD.f629._lb0460._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280673.physics_ZeroBias.recon.ESD.f629._lb0470._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280673.physics_ZeroBias.recon.ESD.f629._lb0480._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280673.physics_ZeroBias.recon.ESD.f629._lb0490._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280673.physics_ZeroBias.recon.ESD.f629._lb0500._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280673.physics_ZeroBias.recon.ESD.f629._lb0510._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280673.physics_ZeroBias.recon.ESD.f629._lb0520._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280673.physics_ZeroBias.recon.ESD.f629._lb0530._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280673.physics_ZeroBias.recon.ESD.f629._lb0540._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280673.physics_ZeroBias.recon.ESD.f629._lb0550._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280673.physics_ZeroBias.recon.ESD.f629._lb0560._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280673.physics_ZeroBias.recon.ESD.f629._lb0570._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280673.physics_ZeroBias.recon.ESD.f629._lb0580._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280673.physics_ZeroBias.recon.ESD.f629._lb0590._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280673.physics_ZeroBias.recon.ESD.f629._lb0600._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280673.physics_ZeroBias.recon.ESD.f629._lb0610._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280673.physics_ZeroBias.recon.ESD.f629._lb0620._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280673.physics_ZeroBias.recon.ESD.f629._lb0630._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280673.physics_ZeroBias.recon.ESD.f629._lb0640._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280673.physics_ZeroBias.recon.ESD.f629._lb0660._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280673.physics_ZeroBias.recon.ESD.f629._lb0670._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280673.physics_ZeroBias.recon.ESD.f629._lb0680._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280673.physics_ZeroBias.recon.ESD.f629._lb0690._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280673.physics_ZeroBias.recon.ESD.f629._lb0700._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280673.physics_ZeroBias.recon.ESD.f629._lb0710._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280673.physics_ZeroBias.recon.ESD.f629._lb0720._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280673.physics_ZeroBias.recon.ESD.f629._lb0730._SFO-ALL._0001_000.1",
    # eospath+"data15_13TeV.00280673.physics_ZeroBias.recon.ESD.f629._lb0740._SFO-ALL._0001_000.1",

    ]

# eospath = "root://eosatlas.cern.ch///eos/atlas/atlastier0/rucio/data15_13TeV/physics_Main/00278968/data15_13TeV.00278968.physics_Main.merge.DESDM_MCP.f628_m1501/"
# svcMgr.EventSelector.InputCollections = [eospath+"data15_13TeV.00278968.physics_Main.merge.DESDM_MCP.f628_m1501._0001.1",
#                                          ]

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

# Define your output file name and stream name
rootStreamName = "MyFirstHistoStream"
rootFileName   = "myHistosAth.root"
alg.RootStreamName   = rootStreamName
alg.RootDirName      = "/MyHists"

from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
MyFirstHistoXAODStream = MSMgr.NewRootStream( rootStreamName, rootFileName )
xaodStream = MSMgr.NewPoolRootStream( "StreamXAOD", "xAOD.out.root" )
xaodStream.AddItem( ["xAOD::JetContainer#*","xAOD::JetAuxContainer#*"] )
xaodStream.AddMetaDataItem("EventBookkeeperCollection#*")
algSeq += alg

svcMgr += CfgMgr.THistSvc()
svcMgr.THistSvc.Output += ["MYSTREAM DATAFILE='myfile.root' OPT='RECREATE'"]
svcMgr.THistSvc.Output += ["ANOTHERSTREAM DATAFILE='anotherfile.root' OPT='RECREATE'"]

from LumiBlockComps.LuminosityToolDefault import LuminosityToolDefault
theLumiTool = LuminosityToolDefault()
from AthenaCommon.AppMgr import ToolSvc
ToolSvc += theLumiTool

from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
cfg = TriggerConfigGetter('ReadPool')

from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
tdt = Trig__TrigDecisionTool("TrigDecisionTool")
ToolSvc += tdt

