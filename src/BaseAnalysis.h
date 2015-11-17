#ifndef MUONRAWHITS_BASEANALYSIS_H
#define MUONRAWHITS_BASEANALYSIS_H 

#include "AthenaBaseComps/AthHistogramAlgorithm.h"
#include "xAODMuon/Muon.h"
#include "xAODMuon/MuonSegment.h"
#include "TrkTrack/Track.h"
#include "TH1.h"
#include <vector>
#include "muonEvent/Muon.h"
#include "MuonSegment/MuonSegment.h"

#include "AthLinks/ElementLink.h"
#include "AthContainers/DataVector.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"

#include "LumiBlockComps/ILuminosityTool.h"

#include "TrigDecisionTool/ChainGroup.h"
#include "TrigDecisionInterface/ITrigDecisionTool.h"

class MdtIdHelper;
class CscIdHelper;
class RpcIdHelper;
class TgcIdHelper;

class ICscClusterUtilTool;

namespace Muon {
    class MuonEDMHelperTool;
}
namespace MuonGM {
    class MuonDetectorManager;
}
namespace TrigConf {
    class xAODConfigTool;
}
namespace Trig {
    class ChainGroup;
    class TrigDecisionTool;
}

class BaseAnalysis: public ::AthHistogramAlgorithm {

 public:

  BaseAnalysis(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~BaseAnalysis();

  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

  virtual StatusCode initialize_branches();
  virtual StatusCode clear_branches();

  virtual StatusCode fill_eventinfo();
  virtual StatusCode fill_trigger();
  virtual StatusCode fill_mdt();
  virtual StatusCode fill_csc();

  virtual StatusCode dump_mdt_geometry();
  virtual StatusCode dump_csc_geometry();

  virtual double r(    double x, double y);
  virtual double phi(  double x, double y);
  virtual double eta(  double x, double y, double z);
  virtual double theta(double x, double y, double z);

  virtual int CollidingBunches();

  std::string OfflineToOnline(std::string type, int eta, int phi);
  std::string OfflineToOnlineSide(int eta);
  int         OfflineToOnlineEta(std::string type, int eta);
  int         OfflineToOnlinePhi(std::string type, int phi);

  int ignore_csc_layer(std::string side, int phi_sector, int layer);
  int ignore_mdt_tube(std::string chamber, int ml, int layer, int tube);

  void debug_luminosity();

  ToolHandle< Muon::MuonEDMHelperTool > m_helperTool; 
  ToolHandle< ICscClusterUtilTool >     m_clusterUtilTool;
  ToolHandle< Trig::TrigDecisionTool >  m_trigDecTool;
  ToolHandle< ILuminosityTool >         m_lumiTool;

  StoreGateSvc* m_storeGateSvc;

  const MuonGM::MuonDetectorManager*  m_detMgr;

  const MdtIdHelper*  m_mdtIdHelper;
  const CscIdHelper*  m_cscIdHelper;
  const RpcIdHelper*  m_rpcIdHelper;
  const TgcIdHelper*  m_tgcIdHelper;

 private:

  TTree *tree;

  const Trig::ChainGroup* m_all; //!
  std::vector<std::string> triggers; //!

  int RunNumber; //! 
  int EventNumber; //!
  int lbn; //!
  int bcid; //!
  int colliding_bunches; //!
  double avgIntPerXing; //!
  double actIntPerXing; //!
  double lbAverageLuminosity; //!
  double lbLuminosityPerBCID; //!

  double prescale_L1; //!
  double prescale_HLT; //!

  int mdt_chamber_n;
  std::vector<double>      mdt_chamber_r; //!
  std::vector<double>      mdt_chamber_phi; //!
  std::vector<double>      mdt_chamber_eta; //!
  std::vector<std::string> mdt_chamber_name; //!

  std::vector<std::string> mdt_chamber_type; //!
  std::vector<std::string> mdt_chamber_side; //!
  std::vector<int>         mdt_chamber_eta_station; //!
  std::vector<int>         mdt_chamber_phi_sector; //!

  std::vector<int>              mdt_chamber_tube_n; //!
  std::vector<std::vector<int>> mdt_chamber_tube_r; //!
  std::vector<std::vector<int>> mdt_chamber_tube_adc; //!

  int csc_chamber_n;
  std::vector<int>         csc_chamber_r; //!
  std::vector<double>      csc_chamber_phi; //!
  std::vector<double>      csc_chamber_eta; //!
  std::vector<int>         csc_chamber_clusters; //!
  std::vector<int>         csc_chamber_clusters_eta; //!
  std::vector<int>         csc_chamber_clusters_phi; //!
  std::vector<std::string> csc_chamber_type; //!
  std::vector<std::string> csc_chamber_side; //!
  std::vector<int>         csc_chamber_phi_sector; //!
  
  std::vector<int>              csc_chamber_cluster_n; //!
  std::vector<std::vector<int>> csc_chamber_cluster_r; //!
  std::vector<std::vector<int>> csc_chamber_cluster_rmax; //!
  std::vector<std::vector<int>> csc_chamber_cluster_qsum; //!
  std::vector<std::vector<int>> csc_chamber_cluster_qmax; //!
  std::vector<std::vector<int>> csc_chamber_cluster_strips; //!
  std::vector<std::vector<int>> csc_chamber_cluster_measuresphi; //!

};

#endif //> !MUONRAWHITS_BASEANALYSIS_H
