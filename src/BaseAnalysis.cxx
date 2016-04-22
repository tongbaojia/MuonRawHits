// MuonRawHits includes
#include "BaseAnalysis.h"

#include "xAODEventInfo/EventInfo.h"
#include "xAODParticleEvent/IParticleLink.h"
#include "MuonRecHelperTools/MuonEDMHelperTool.h"

#include "AthLinks/ElementLink.h"
#include "AthContainers/DataVector.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/ServiceHandle.h"

#include <iostream>
#include <string>

#include "Identifier/Identifier.h"
#include "MuonIdHelpers/MuonStationIndex.h"
#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonIdHelpers/CscIdHelper.h"
#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonIdHelpers/TgcIdHelper.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonReadoutGeometry/CscReadoutElement.h"
#include "MuonReadoutGeometry/TgcReadoutElement.h"

#include "MuonPrepRawData/CscStripPrepDataCollection.h"
#include "MuonPrepRawData/CscStripPrepDataContainer.h"
#include "MuonPrepRawData/CscStripPrepData.h"

#include "MuonPrepRawData/CscPrepDataCollection.h"
#include "MuonPrepRawData/CscPrepDataContainer.h"
#include "MuonPrepRawData/CscPrepData.h"

#include "CscClusterization/ICscClusterFitter.h"
#include "CscClusterization/ICscClusterUtilTool.h"

#include "TrkSegment/SegmentCollection.h"

#include "TrigDecisionTool/ChainGroup.h"
#include "TrigDecisionTool/TrigDecisionTool.h"

BaseAnalysis::BaseAnalysis(const std::string& name, ISvcLocator* pSvcLocator):
    AthHistogramAlgorithm(name, pSvcLocator),
    m_helperTool("Muon::MuonEDMHelperTool/MuonEDMHelperTool"),
    m_clusterUtilTool("CscClusterUtilTool/CscClusterUtilTool"),
    m_trigDecTool("Trig::TrigDecisionTool/TrigDecisionTool"),
    m_lumiTool("LuminosityTool")
{
    declareProperty("MuonEDMHelperTool",  m_helperTool);
    declareProperty("CscClusterUtilTool", m_clusterUtilTool);
    declareProperty("TrigDecisionTool",   m_trigDecTool);
    declareProperty("LuminosityTool",     m_lumiTool);
}

BaseAnalysis::~BaseAnalysis() {}

StatusCode BaseAnalysis::initialize() {
    
    ServiceHandle<ITHistSvc>             histSvc("THistSvc",      name());
    ServiceHandle<StoreGateSvc> detectorStoreSvc("DetectorStore", name());

    CHECK(detectorStoreSvc.retrieve());
    CHECK(detectorStoreSvc->retrieve(m_detMgr, "Muon"));

    m_mdtIdHelper = m_detMgr->mdtIdHelper();
    m_cscIdHelper = m_detMgr->cscIdHelper();
    m_rpcIdHelper = m_detMgr->rpcIdHelper();
    m_tgcIdHelper = m_detMgr->tgcIdHelper();

    CHECK(histSvc.retrieve());
    CHECK(m_lumiTool.retrieve());
    CHECK(m_trigDecTool.retrieve());
    CHECK(m_clusterUtilTool.retrieve());

    tree = new TTree("physics", "physics");
    CHECK(histSvc->regTree("/ANOTHERSTREAM/tree", tree));
    CHECK(initialize_branches());

    return StatusCode::SUCCESS;
}

StatusCode BaseAnalysis::execute() {

    CHECK(fill_eventinfo());

    if ((!m_trigDecTool->isPassed("HLT_noalg_zb_L1ZB")) && (!isMC))
        return StatusCode::SUCCESS;

    CHECK(clear_branches());

    if (do_ntuples){
        CHECK(fill_trigger());
        CHECK(fill_mdt());
        CHECK(fill_csc());
        CHECK(fill_csc_segments());

        tree->Fill();
    }

    if (do_geometry){
        CHECK(dump_mdt_geometry());
        CHECK(dump_csc_geometry());
    }

    return StatusCode::SUCCESS;
}

StatusCode BaseAnalysis::finalize() {
    return StatusCode::SUCCESS;
}

StatusCode BaseAnalysis::initialize_branches() {

    tree->Branch("RunNumber",           &RunNumber);
    tree->Branch("EventNumber",         &EventNumber);
    tree->Branch("lbn",                 &lbn);
    tree->Branch("bcid",                &bcid);
    tree->Branch("isMC",                &isMC);
    tree->Branch("colliding_bunches",   &colliding_bunches);
    tree->Branch("avgIntPerXing",       &avgIntPerXing);
    tree->Branch("actIntPerXing",       &actIntPerXing);
    tree->Branch("lbAverageLuminosity", &lbAverageLuminosity);
    tree->Branch("lbLuminosityPerBCID", &lbLuminosityPerBCID);

    tree->Branch("prescale_L1",  &prescale_L1);
    tree->Branch("prescale_HLT", &prescale_HLT);

    tree->Branch("mdt_chamber_n",            &mdt_chamber_n);
    tree->Branch("mdt_chamber_r",            &mdt_chamber_r);
    tree->Branch("mdt_chamber_phi",          &mdt_chamber_phi);
    tree->Branch("mdt_chamber_eta",          &mdt_chamber_eta);
    tree->Branch("mdt_chamber_type",         &mdt_chamber_type);
    tree->Branch("mdt_chamber_side",         &mdt_chamber_side);
    tree->Branch("mdt_chamber_eta_station",  &mdt_chamber_eta_station);
    tree->Branch("mdt_chamber_phi_sector",   &mdt_chamber_phi_sector);
    tree->Branch("mdt_chamber_tube_n",       &mdt_chamber_tube_n);
    tree->Branch("mdt_chamber_tube_r",       &mdt_chamber_tube_r);
    tree->Branch("mdt_chamber_tube_adc",     &mdt_chamber_tube_adc);
    tree->Branch("mdt_chamber_tube_tdc",     &mdt_chamber_tube_tdc);
    tree->Branch("mdt_chamber_tube_id",      &mdt_chamber_tube_id);
    tree->Branch("mdt_chamber_tube_n_adc50", &mdt_chamber_tube_n_adc50);
    
    tree->Branch("csc_chamber_n",                 &csc_chamber_n);
    tree->Branch("csc_chamber_r",                 &csc_chamber_r);
    tree->Branch("csc_chamber_phi",               &csc_chamber_phi);
    tree->Branch("csc_chamber_eta",               &csc_chamber_eta);
    tree->Branch("csc_chamber_type",              &csc_chamber_type);
    tree->Branch("csc_chamber_side",              &csc_chamber_side);
    tree->Branch("csc_chamber_phi_sector",        &csc_chamber_phi_sector);
    tree->Branch("csc_chamber_cluster_n",         &csc_chamber_cluster_n);
    tree->Branch("csc_chamber_cluster_r",         &csc_chamber_cluster_r);
    tree->Branch("csc_chamber_cluster_rmax",      &csc_chamber_cluster_rmax);
    tree->Branch("csc_chamber_cluster_qsum",      &csc_chamber_cluster_qsum);
    tree->Branch("csc_chamber_cluster_qmax",      &csc_chamber_cluster_qmax);
    tree->Branch("csc_chamber_cluster_qleft",     &csc_chamber_cluster_qleft);
    tree->Branch("csc_chamber_cluster_qright",    &csc_chamber_cluster_qright);
    tree->Branch("csc_chamber_cluster_strips",    &csc_chamber_cluster_strips);
    tree->Branch("csc_chamber_cluster_n_qmax100", &csc_chamber_cluster_n_qmax100);
    tree->Branch("csc_chamber_cluster_n_notecho", &csc_chamber_cluster_n_notecho);
    tree->Branch("csc_chamber_cluster_segment",   &csc_chamber_cluster_segment);

    tree->Branch("csc_segment_n",            &csc_segment_n);
    tree->Branch("csc_segment_r",            &csc_segment_r);
    tree->Branch("csc_segment_phi",          &csc_segment_phi);
    tree->Branch("csc_segment_eta",          &csc_segment_eta);
    tree->Branch("csc_segment_type",         &csc_segment_type);
    tree->Branch("csc_segment_side",         &csc_segment_side);
    tree->Branch("csc_segment_phi_sector",   &csc_segment_phi_sector);
    tree->Branch("csc_segment_nphiclusters", &csc_segment_nphiclusters);
    tree->Branch("csc_segment_netaclusters", &csc_segment_netaclusters);

    return StatusCode::SUCCESS;
}

double BaseAnalysis::r(double x, double y)
{
    return sqrt(x*x + y*y);
}
double BaseAnalysis::phi(double x, double y)
{
    return x == 0.0 && y == 0.0 ? 0.0 : TMath::ATan2(y, x);
}
double BaseAnalysis::theta(double x, double y, double z)
{
    return x == 0.0 && y == 0.0 && z == 0.0 ? 0.0 : TMath::ATan2(r(x, y), z);
}
double BaseAnalysis::eta(double x, double y, double z)
{
    if (z == 0) return 0;
    return -1.0*TMath::Log(TMath::Tan(theta(x, y, z)/2.0));
}

std::string BaseAnalysis::OfflineToOnline(std::string type, int eta, int phi)
{
    if (type.size() != 3){
        ATH_MSG_ERROR(type);
        ATH_MSG_FATAL("Bad input chamber type");
    }

    std::string side    = OfflineToOnlineSide(eta);
    std::string station = std::to_string(OfflineToOnlineEta(type, eta));
    std::string sector  = std::to_string(OfflineToOnlinePhi(type, phi));
    if (OfflineToOnlinePhi(type, phi) < 10){
        sector = '0'+sector;
    }
    std::string chamber = type + station + side + sector;
    return chamber;
}

int BaseAnalysis::OfflineToOnlinePhi(std::string type, int phi)
{
    if      (type.at(2) == 'S') return 2*phi;
    else if (type.at(2) == 'L') return 2*phi - 1;
    else                        return phi;
}

int BaseAnalysis::OfflineToOnlineEta(std::string type, int eta)
{
    return abs(eta);
}

std::string BaseAnalysis::OfflineToOnlineSide(int eta)
{
    if      (eta  > 0) return "A";
    else if (eta == 0) return "B";
    else               return "C";
}

int BaseAnalysis::CollidingBunches()
{
    int nbunches = 0;
    const std::vector<float>& lumi_per_bcid = m_lumiTool->lbLuminosityPerBCIDVector();
    unsigned int nbcid = lumi_per_bcid.size();

    for (unsigned int ibcid = 0; ibcid < nbcid; ibcid++){
        if (lumi_per_bcid[ibcid] > 0){
            nbunches++;
        }
    }

    return nbunches;
}

int BaseAnalysis::ignore_csc_layer(std::string side, int phi_sector, int layer)
{
    // https://atlasop.cern.ch/twiki/bin/view/Main/MuonSpectrometerWhiteBoard
    if (side == "C" && phi_sector ==  1 && layer == 1) return 1;
    if (side == "C" && phi_sector ==  3 && layer == 2) return 1;
    if (side == "A" && phi_sector ==  9 && layer == 1) return 1;

    // https://atlasop.cern.ch/elisa/display/280219
    if (side == "A" && phi_sector ==  5 && layer == 1) return 1;
    if (side == "A" && phi_sector ==  5 && layer == 2) return 1;
    if (side == "A" && phi_sector ==  9 && layer == 1) return 1;
    if (side == "A" && phi_sector ==  9 && layer == 2) return 1;
    if (side == "A" && phi_sector == 13 && layer == 4) return 1;
    if (side == "A" && phi_sector == 14 && layer == 2) return 1;
    if (side == "A" && phi_sector == 15 && layer == 3) return 1;
    if (side == "A" && phi_sector == 15 && layer == 4) return 1;
    if (side == "A" && phi_sector == 16 && layer == 4) return 1;
    if (side == "C" && phi_sector ==  1 && layer == 2) return 1;
    if (side == "C" && phi_sector ==  3 && layer == 1) return 1;
    if (side == "C" && phi_sector ==  7 && layer == 4) return 1;
    if (side == "C" && phi_sector == 12 && layer == 4) return 1;
    if (side == "C" && phi_sector == 13 && layer == 1) return 1;
    if (side == "C" && phi_sector == 15 && layer == 4) return 1;
    
    return 0;
}

int BaseAnalysis::ignore_mdt_tube(std::string chamber, int ml, int layer, int tube)
{
    // from Tiesheng
    if (chamber == "EIL2A15" && ml == 1 && layer == 1 && tube == 24) return 1;
    if (chamber == "EIL2A15" && ml == 1 && layer == 1 && tube >= 33) return 1;
    if (chamber == "EIL2A15" && ml == 1 && layer == 2 && tube == 35) return 1;
    if (chamber == "EIL2A15" && ml == 1 && layer == 4 && tube == 32) return 1;
    if (chamber == "EIL2A15" && ml == 1 && layer == 4 && tube >= 35) return 1;

    // these tubes have >10% occupancy in 284285 ZeroBias
    if (chamber == "BEE1A03" && ml == 1 && layer == 1 && tube == 41) return 1;
    if (chamber == "BEE1A03" && ml == 1 && layer == 1 && tube == 42) return 1;
    if (chamber == "BEE1A03" && ml == 1 && layer == 3 && tube == 19) return 1;
    if (chamber == "BEE2A06" && ml == 1 && layer == 1 && tube ==  5) return 1;
    if (chamber == "BEE2A06" && ml == 1 && layer == 1 && tube ==  8) return 1;
    if (chamber == "BEE2A06" && ml == 1 && layer == 1 && tube == 11) return 1;
    if (chamber == "BEE2A06" && ml == 1 && layer == 1 && tube == 41) return 1;
    if (chamber == "BEE2A06" && ml == 1 && layer == 1 && tube == 42) return 1;
    if (chamber == "BIL1A13" && ml == 2 && layer == 4 && tube == 29) return 1;
    if (chamber == "BIL2A07" && ml == 2 && layer == 1 && tube ==  1) return 1;
    if (chamber == "BIL2A13" && ml == 2 && layer == 4 && tube == 16) return 1;
    if (chamber == "BIL2A13" && ml == 2 && layer == 4 && tube == 17) return 1;
    if (chamber == "BIL2C07" && ml == 2 && layer == 4 && tube == 22) return 1;
    if (chamber == "BIL2C09" && ml == 2 && layer == 4 && tube == 18) return 1;
    if (chamber == "BIL2C09" && ml == 2 && layer == 4 && tube == 30) return 1;
    if (chamber == "BME1A07" && ml == 1 && layer == 3 && tube ==  1) return 1;
    if (chamber == "BME1A07" && ml == 1 && layer == 4 && tube ==  1) return 1;
    if (chamber == "BME1A07" && ml == 1 && layer == 4 && tube ==  2) return 1;
    if (chamber == "BME1A07" && ml == 1 && layer == 4 && tube == 27) return 1;
    if (chamber == "BME1A07" && ml == 1 && layer == 4 && tube == 28) return 1;
    if (chamber == "BME1A07" && ml == 1 && layer == 4 && tube == 32) return 1;
    if (chamber == "BME1A07" && ml == 1 && layer == 4 && tube == 37) return 1;
    if (chamber == "BME1A07" && ml == 1 && layer == 4 && tube == 38) return 1;
    if (chamber == "BME1A07" && ml == 1 && layer == 4 && tube == 40) return 1;
    if (chamber == "BME1A07" && ml == 1 && layer == 4 && tube == 46) return 1;
    if (chamber == "BME1A07" && ml == 1 && layer == 4 && tube == 73) return 1;
    if (chamber == "BME1A07" && ml == 1 && layer == 4 && tube == 77) return 1;
    if (chamber == "BME1A07" && ml == 2 && layer == 1 && tube == 78) return 1;
    if (chamber == "BME1A07" && ml == 2 && layer == 2 && tube == 77) return 1;
    if (chamber == "BME1A07" && ml == 2 && layer == 2 && tube == 78) return 1;
    if (chamber == "BME1A07" && ml == 2 && layer == 3 && tube == 78) return 1;
    if (chamber == "BME1A07" && ml == 2 && layer == 4 && tube == 42) return 1;
    if (chamber == "BME1A07" && ml == 2 && layer == 4 && tube == 71) return 1;
    if (chamber == "BME1A07" && ml == 2 && layer == 4 && tube == 72) return 1;
    if (chamber == "BME1A07" && ml == 2 && layer == 4 && tube == 75) return 1;
    if (chamber == "BME1A07" && ml == 2 && layer == 4 && tube == 76) return 1;
    if (chamber == "BME1A07" && ml == 2 && layer == 4 && tube == 77) return 1;
    if (chamber == "BME1A07" && ml == 2 && layer == 4 && tube == 78) return 1;
    if (chamber == "BME1C07" && ml == 1 && layer == 1 && tube == 78) return 1;
    if (chamber == "BME1C07" && ml == 1 && layer == 2 && tube == 78) return 1;
    if (chamber == "BME1C07" && ml == 1 && layer == 3 && tube == 78) return 1;
    if (chamber == "BME1C07" && ml == 1 && layer == 4 && tube == 78) return 1;
    if (chamber == "BME1C07" && ml == 2 && layer == 2 && tube == 78) return 1;
    if (chamber == "BME1C07" && ml == 2 && layer == 4 && tube == 78) return 1;
    if (chamber == "BMF1A06" && ml == 1 && layer == 1 && tube ==  1) return 1;
    if (chamber == "BMF1A06" && ml == 1 && layer == 2 && tube == 23) return 1;
    if (chamber == "BMF1A06" && ml == 1 && layer == 3 && tube ==  7) return 1;
    if (chamber == "BMF1A06" && ml == 1 && layer == 3 && tube == 15) return 1;
    if (chamber == "BMF1A06" && ml == 1 && layer == 3 && tube == 24) return 1;
    if (chamber == "BMF1A07" && ml == 1 && layer == 3 && tube == 57) return 1;
    if (chamber == "BMF1A07" && ml == 2 && layer == 2 && tube == 44) return 1;
    if (chamber == "BMF1A07" && ml == 2 && layer == 2 && tube == 47) return 1;
    if (chamber == "BMF1A07" && ml == 2 && layer == 3 && tube == 62) return 1;
    if (chamber == "BMF1C06" && ml == 1 && layer == 1 && tube == 62) return 1;
    if (chamber == "BMF1C06" && ml == 1 && layer == 1 && tube == 63) return 1;
    if (chamber == "BMF1C06" && ml == 1 && layer == 1 && tube == 69) return 1;
    if (chamber == "BML1C01" && ml == 1 && layer == 2 && tube == 32) return 1;
    if (chamber == "BML1C01" && ml == 2 && layer == 2 && tube == 32) return 1;
    if (chamber == "BML1C07" && ml == 1 && layer == 3 && tube ==  1) return 1;
    if (chamber == "BML1C07" && ml == 2 && layer == 3 && tube ==  1) return 1;
    if (chamber == "BML2C09" && ml == 2 && layer == 2 && tube == 56) return 1;
    if (chamber == "BML4A03" && ml == 1 && layer == 3 && tube == 18) return 1;
    if (chamber == "BML6A03" && ml == 1 && layer == 3 && tube ==  1) return 1;
    if (chamber == "BMS1A08" && ml == 1 && layer == 1 && tube == 17) return 1;
    if (chamber == "BMS1A08" && ml == 1 && layer == 1 && tube == 46) return 1;
    if (chamber == "BMS1A08" && ml == 1 && layer == 1 && tube == 53) return 1;
    if (chamber == "BMS1A08" && ml == 1 && layer == 2 && tube == 20) return 1;
    if (chamber == "BMS1A08" && ml == 1 && layer == 3 && tube == 20) return 1;
    if (chamber == "BMS1A08" && ml == 1 && layer == 3 && tube == 45) return 1;
    if (chamber == "BMS1A08" && ml == 2 && layer == 1 && tube == 16) return 1;
    if (chamber == "BMS1A08" && ml == 2 && layer == 1 && tube == 25) return 1;
    if (chamber == "BMS1A08" && ml == 2 && layer == 2 && tube == 16) return 1;
    if (chamber == "BMS1A08" && ml == 2 && layer == 2 && tube == 24) return 1;
    if (chamber == "BMS1A08" && ml == 2 && layer == 2 && tube == 28) return 1;
    if (chamber == "BMS1A08" && ml == 2 && layer == 2 && tube == 30) return 1;
    if (chamber == "BMS1A08" && ml == 2 && layer == 2 && tube == 36) return 1;
    if (chamber == "BMS1A08" && ml == 2 && layer == 3 && tube == 17) return 1;
    if (chamber == "BMS1A10" && ml == 2 && layer == 1 && tube ==  1) return 1;
    if (chamber == "BMS1A10" && ml == 2 && layer == 1 && tube == 17) return 1;
    if (chamber == "BMS1A10" && ml == 2 && layer == 1 && tube == 18) return 1;
    if (chamber == "BMS1A10" && ml == 2 && layer == 1 && tube == 19) return 1;
    if (chamber == "BMS1A10" && ml == 2 && layer == 1 && tube == 22) return 1;
    if (chamber == "BMS1A10" && ml == 2 && layer == 1 && tube == 24) return 1;
    if (chamber == "BMS1A10" && ml == 2 && layer == 1 && tube == 25) return 1;
    if (chamber == "BMS1A10" && ml == 2 && layer == 1 && tube == 26) return 1;
    if (chamber == "BMS1A10" && ml == 2 && layer == 1 && tube == 27) return 1;
    if (chamber == "BMS1A10" && ml == 2 && layer == 1 && tube == 28) return 1;
    if (chamber == "BMS1A10" && ml == 2 && layer == 1 && tube == 29) return 1;
    if (chamber == "BMS1A10" && ml == 2 && layer == 1 && tube == 30) return 1;
    if (chamber == "BMS1A10" && ml == 2 && layer == 1 && tube == 32) return 1;
    if (chamber == "BMS1A10" && ml == 2 && layer == 1 && tube == 33) return 1;
    if (chamber == "BMS1A16" && ml == 1 && layer == 3 && tube ==  1) return 1;
    if (chamber == "BMS1A16" && ml == 1 && layer == 3 && tube == 29) return 1;
    if (chamber == "BMS1A16" && ml == 1 && layer == 3 && tube == 30) return 1;
    if (chamber == "BMS1A16" && ml == 1 && layer == 3 && tube == 31) return 1;
    if (chamber == "BMS1A16" && ml == 1 && layer == 3 && tube == 32) return 1;
    if (chamber == "BMS1A16" && ml == 2 && layer == 1 && tube == 25) return 1;
    if (chamber == "BMS1A16" && ml == 2 && layer == 1 && tube == 27) return 1;
    if (chamber == "BMS1A16" && ml == 2 && layer == 1 && tube == 56) return 1;
    if (chamber == "BMS1A16" && ml == 2 && layer == 2 && tube == 34) return 1;
    if (chamber == "BMS1A16" && ml == 2 && layer == 2 && tube == 35) return 1;
    if (chamber == "BMS1C02" && ml == 1 && layer == 1 && tube == 54) return 1;
    if (chamber == "BMS1C02" && ml == 1 && layer == 1 && tube == 56) return 1;
    if (chamber == "BMS1C02" && ml == 1 && layer == 2 && tube == 56) return 1;
    if (chamber == "BMS1C02" && ml == 1 && layer == 3 && tube == 13) return 1;
    if (chamber == "BMS1C02" && ml == 1 && layer == 3 && tube == 56) return 1;
    if (chamber == "BMS1C06" && ml == 1 && layer == 1 && tube == 54) return 1;
    if (chamber == "BMS1C08" && ml == 1 && layer == 1 && tube == 17) return 1;
    if (chamber == "BMS1C08" && ml == 1 && layer == 2 && tube == 56) return 1;
    if (chamber == "BMS1C08" && ml == 1 && layer == 3 && tube == 56) return 1;
    if (chamber == "BMS1C08" && ml == 2 && layer == 2 && tube == 16) return 1;
    if (chamber == "BMS1C08" && ml == 2 && layer == 2 && tube == 56) return 1;
    if (chamber == "BMS1C08" && ml == 2 && layer == 3 && tube == 16) return 1;
    if (chamber == "BMS1C08" && ml == 2 && layer == 3 && tube == 17) return 1;
    if (chamber == "BMS1C08" && ml == 2 && layer == 3 && tube == 54) return 1;
    if (chamber == "BMS1C16" && ml == 1 && layer == 1 && tube == 56) return 1;
    if (chamber == "BMS1C16" && ml == 1 && layer == 3 && tube == 14) return 1;
    if (chamber == "BMS1C16" && ml == 2 && layer == 2 && tube == 56) return 1;
    if (chamber == "BMS2A08" && ml == 1 && layer == 1 && tube ==  1) return 1;
    if (chamber == "BMS2A08" && ml == 1 && layer == 3 && tube ==  1) return 1;
    if (chamber == "BMS2A10" && ml == 1 && layer == 1 && tube == 11) return 1;
    if (chamber == "BMS2C04" && ml == 2 && layer == 3 && tube ==  1) return 1;
    if (chamber == "BMS2C08" && ml == 1 && layer == 1 && tube == 24) return 1;
    if (chamber == "BMS2C08" && ml == 1 && layer == 1 && tube == 38) return 1;
    if (chamber == "BMS2C08" && ml == 1 && layer == 1 && tube == 42) return 1;
    if (chamber == "BMS2C08" && ml == 2 && layer == 1 && tube == 27) return 1;
    if (chamber == "BMS2C08" && ml == 2 && layer == 1 && tube == 48) return 1;
    if (chamber == "BMS2C08" && ml == 2 && layer == 3 && tube == 22) return 1;
    if (chamber == "BMS2C08" && ml == 2 && layer == 3 && tube == 43) return 1;
    if (chamber == "BMS2C16" && ml == 2 && layer == 2 && tube ==  1) return 1;
    if (chamber == "BMS2C16" && ml == 2 && layer == 3 && tube ==  1) return 1;
    if (chamber == "BMS4A10" && ml == 2 && layer == 1 && tube ==  1) return 1;
    if (chamber == "BMS4C02" && ml == 2 && layer == 1 && tube ==  1) return 1;
    if (chamber == "BMS4C08" && ml == 2 && layer == 1 && tube ==  2) return 1;
    if (chamber == "BMS4C08" && ml == 2 && layer == 1 && tube ==  8) return 1;
    if (chamber == "BMS4C16" && ml == 1 && layer == 1 && tube == 10) return 1;
    if (chamber == "BMS5A04" && ml == 2 && layer == 1 && tube == 32) return 1;
    if (chamber == "BMS5C02" && ml == 1 && layer == 2 && tube == 32) return 1;
    if (chamber == "BMS6A06" && ml == 1 && layer == 1 && tube == 39) return 1;
    if (chamber == "BMS6C10" && ml == 2 && layer == 3 && tube ==  1) return 1;
    if (chamber == "BOF1A06" && ml == 2 && layer == 3 && tube == 12) return 1;
    if (chamber == "BOF1C06" && ml == 2 && layer == 3 && tube == 62) return 1;
    if (chamber == "BOF4C07" && ml == 2 && layer == 2 && tube ==  7) return 1;
    if (chamber == "BOG2C07" && ml == 2 && layer == 1 && tube == 31) return 1;
    if (chamber == "BOG2C07" && ml == 2 && layer == 3 && tube ==  5) return 1;
    if (chamber == "BOG3A06" && ml == 2 && layer == 1 && tube == 38) return 1;
    if (chamber == "BOG4A07" && ml == 1 && layer == 3 && tube == 38) return 1;
    if (chamber == "BOG4A07" && ml == 1 && layer == 3 && tube == 40) return 1;
    if (chamber == "BOG4C06" && ml == 1 && layer == 1 && tube == 23) return 1;
    if (chamber == "BOG4C06" && ml == 1 && layer == 1 && tube == 25) return 1;
    if (chamber == "BOG4C06" && ml == 1 && layer == 1 && tube == 26) return 1;
    if (chamber == "BOG4C06" && ml == 1 && layer == 2 && tube == 21) return 1;
    if (chamber == "BOG4C06" && ml == 1 && layer == 2 && tube == 22) return 1;
    if (chamber == "BOG4C06" && ml == 1 && layer == 2 && tube == 23) return 1;
    if (chamber == "BOG4C06" && ml == 1 && layer == 2 && tube == 24) return 1;
    if (chamber == "BOG4C06" && ml == 1 && layer == 2 && tube == 25) return 1;
    if (chamber == "BOG4C06" && ml == 1 && layer == 2 && tube == 26) return 1;
    if (chamber == "BOG4C06" && ml == 1 && layer == 3 && tube == 22) return 1;
    if (chamber == "BOG4C06" && ml == 1 && layer == 3 && tube == 24) return 1;
    if (chamber == "BOG4C06" && ml == 1 && layer == 3 && tube == 27) return 1;
    if (chamber == "BOG4C07" && ml == 2 && layer == 2 && tube == 39) return 1;
    if (chamber == "BOG4C07" && ml == 2 && layer == 2 && tube == 40) return 1;
    if (chamber == "BOL3A07" && ml == 1 && layer == 1 && tube ==  1) return 1;
    if (chamber == "BOL5A05" && ml == 2 && layer == 3 && tube ==  1) return 1;
    if (chamber == "BOL6A13" && ml == 2 && layer == 1 && tube ==  1) return 1;
    if (chamber == "BOL6A13" && ml == 2 && layer == 1 && tube ==  2) return 1;
    if (chamber == "BOL6A13" && ml == 2 && layer == 2 && tube ==  1) return 1;
    if (chamber == "BOL6A13" && ml == 2 && layer == 3 && tube ==  2) return 1;
    if (chamber == "BOL7C13" && ml == 1 && layer == 2 && tube == 56) return 1;
    if (chamber == "BOL7C13" && ml == 1 && layer == 3 && tube == 49) return 1;
    if (chamber == "BOL7C13" && ml == 1 && layer == 3 && tube == 50) return 1;
    if (chamber == "BOL7C13" && ml == 1 && layer == 3 && tube == 54) return 1;
    if (chamber == "BOL7C13" && ml == 1 && layer == 3 && tube == 56) return 1;
    if (chamber == "BOL7C13" && ml == 1 && layer == 3 && tube == 57) return 1;
    if (chamber == "BOL7C13" && ml == 1 && layer == 3 && tube == 63) return 1;
    if (chamber == "BOL7C13" && ml == 1 && layer == 3 && tube == 66) return 1;
    if (chamber == "BOL7C13" && ml == 2 && layer == 1 && tube == 65) return 1;
    if (chamber == "BOL7C13" && ml == 2 && layer == 1 && tube == 70) return 1;
    if (chamber == "BOL7C13" && ml == 2 && layer == 1 && tube == 71) return 1;
    if (chamber == "BOL7C13" && ml == 2 && layer == 1 && tube == 72) return 1;
    if (chamber == "BOL7C13" && ml == 2 && layer == 3 && tube == 49) return 1;
    if (chamber == "BOL7C13" && ml == 2 && layer == 3 && tube == 56) return 1;
    if (chamber == "BOS1C06" && ml == 1 && layer == 1 && tube ==  1) return 1;
    if (chamber == "BOS2A16" && ml == 1 && layer == 2 && tube == 21) return 1;
    if (chamber == "BOS2A16" && ml == 1 && layer == 3 && tube == 21) return 1;
    if (chamber == "BOS2A16" && ml == 2 && layer == 1 && tube ==  1) return 1;
    if (chamber == "BOS2C08" && ml == 1 && layer == 3 && tube == 19) return 1;
    if (chamber == "BOS2C08" && ml == 1 && layer == 3 && tube == 20) return 1;
    if (chamber == "BOS2C08" && ml == 1 && layer == 3 && tube == 22) return 1;
    if (chamber == "BOS2C10" && ml == 2 && layer == 3 && tube == 19) return 1;
    if (chamber == "BOS2C10" && ml == 2 && layer == 3 && tube == 56) return 1;
    if (chamber == "BOS2C10" && ml == 2 && layer == 3 && tube == 72) return 1;
    if (chamber == "BOS3A04" && ml == 2 && layer == 2 && tube == 13) return 1;
    if (chamber == "BOS3A06" && ml == 2 && layer == 2 && tube == 23) return 1;
    if (chamber == "BOS4A04" && ml == 1 && layer == 3 && tube ==  1) return 1;
    if (chamber == "BOS4A04" && ml == 2 && layer == 3 && tube ==  1) return 1;
    if (chamber == "BOS4A10" && ml == 2 && layer == 2 && tube == 72) return 1;
    if (chamber == "BOS4A10" && ml == 2 && layer == 3 && tube == 13) return 1;
    if (chamber == "BOS4A16" && ml == 1 && layer == 1 && tube ==  1) return 1;
    if (chamber == "BOS4C10" && ml == 2 && layer == 1 && tube ==  1) return 1;
    if (chamber == "BOS4C10" && ml == 2 && layer == 2 && tube == 72) return 1;
    if (chamber == "BOS4C16" && ml == 2 && layer == 3 && tube == 72) return 1;
    if (chamber == "BOS5C08" && ml == 1 && layer == 1 && tube == 63) return 1;
    if (chamber == "BOS5C08" && ml == 1 && layer == 1 && tube == 64) return 1;
    if (chamber == "BOS5C08" && ml == 1 && layer == 2 && tube ==  1) return 1;
    if (chamber == "BOS5C08" && ml == 1 && layer == 3 && tube ==  1) return 1;
    if (chamber == "BOS5C08" && ml == 1 && layer == 3 && tube ==  2) return 1;
    if (chamber == "BOS5C08" && ml == 1 && layer == 3 && tube == 32) return 1;
    if (chamber == "BOS6A02" && ml == 1 && layer == 3 && tube == 38) return 1;
    if (chamber == "BOS6A02" && ml == 1 && layer == 3 && tube == 39) return 1;
    if (chamber == "BOS6A04" && ml == 2 && layer == 3 && tube == 25) return 1;
    if (chamber == "BOS6A04" && ml == 2 && layer == 3 && tube == 47) return 1;
    if (chamber == "BOS6A04" && ml == 2 && layer == 3 && tube == 48) return 1;
    if (chamber == "BOS6A04" && ml == 2 && layer == 3 && tube == 53) return 1;
    if (chamber == "BOS6A04" && ml == 2 && layer == 3 && tube == 56) return 1;
    if (chamber == "BOS6C02" && ml == 1 && layer == 1 && tube == 31) return 1;
    if (chamber == "BOS6C02" && ml == 1 && layer == 1 && tube == 32) return 1;
    if (chamber == "BOS6C02" && ml == 1 && layer == 2 && tube == 43) return 1;
    if (chamber == "BOS6C02" && ml == 1 && layer == 3 && tube == 39) return 1;
    if (chamber == "BOS6C02" && ml == 2 && layer == 1 && tube == 48) return 1;
    if (chamber == "BOS6C02" && ml == 2 && layer == 1 && tube == 49) return 1;
    if (chamber == "BOS6C02" && ml == 2 && layer == 3 && tube == 49) return 1;
    if (chamber == "BOS6C02" && ml == 2 && layer == 3 && tube == 50) return 1;
    if (chamber == "BOS6C02" && ml == 2 && layer == 3 && tube == 61) return 1;
    if (chamber == "BOS6C04" && ml == 1 && layer == 1 && tube ==  3) return 1;
    if (chamber == "BOS6C04" && ml == 1 && layer == 2 && tube == 43) return 1;
    if (chamber == "BOS6C04" && ml == 1 && layer == 3 && tube == 32) return 1;
    if (chamber == "BOS6C04" && ml == 2 && layer == 2 && tube == 48) return 1;
    if (chamber == "BOS6C04" && ml == 2 && layer == 3 && tube == 14) return 1;
    if (chamber == "BOS6C06" && ml == 1 && layer == 1 && tube == 44) return 1;
    if (chamber == "BOS6C06" && ml == 1 && layer == 2 && tube == 28) return 1;
    if (chamber == "BOS6C06" && ml == 2 && layer == 1 && tube == 14) return 1;
    if (chamber == "BOS6C06" && ml == 2 && layer == 3 && tube ==  1) return 1;
    if (chamber == "BOS6C06" && ml == 2 && layer == 3 && tube == 43) return 1;
    if (chamber == "BOS6C06" && ml == 2 && layer == 3 && tube == 46) return 1;
    if (chamber == "BOS6C06" && ml == 2 && layer == 3 && tube == 47) return 1;
    if (chamber == "BOS6C06" && ml == 2 && layer == 3 && tube == 48) return 1;
    if (chamber == "BOS6C08" && ml == 1 && layer == 3 && tube == 46) return 1;
    if (chamber == "BOS6C08" && ml == 1 && layer == 3 && tube == 64) return 1;
    if (chamber == "BOS6C08" && ml == 2 && layer == 3 && tube ==  5) return 1;
    if (chamber == "BOS6C10" && ml == 2 && layer == 3 && tube == 53) return 1;
    if (chamber == "BOS6C16" && ml == 1 && layer == 1 && tube == 13) return 1;
    if (chamber == "EIL2A09" && ml == 2 && layer == 4 && tube ==  6) return 1;
    if (chamber == "EIL3C11" && ml == 1 && layer == 2 && tube ==  1) return 1;
    if (chamber == "EML2A13" && ml == 2 && layer == 3 && tube == 37) return 1;
    if (chamber == "EML2A15" && ml == 2 && layer == 3 && tube == 10) return 1;
    if (chamber == "EML5C01" && ml == 2 && layer == 3 && tube == 64) return 1;
    if (chamber == "EMS2A08" && ml == 2 && layer == 1 && tube ==  9) return 1;
    if (chamber == "EMS3A02" && ml == 1 && layer == 3 && tube == 59) return 1;
    if (chamber == "EOL1A03" && ml == 1 && layer == 2 && tube == 55) return 1;
    if (chamber == "EOL2A01" && ml == 1 && layer == 3 && tube == 24) return 1;
    if (chamber == "EOL2C11" && ml == 2 && layer == 3 && tube == 56) return 1;
    if (chamber == "EOL3A01" && ml == 1 && layer == 3 && tube == 15) return 1;
    if (chamber == "EOL3A01" && ml == 2 && layer == 1 && tube ==  9) return 1;
    if (chamber == "EOL3A01" && ml == 2 && layer == 1 && tube == 17) return 1;
    if (chamber == "EOL3A01" && ml == 2 && layer == 2 && tube ==  7) return 1;
    if (chamber == "EOL3A01" && ml == 2 && layer == 2 && tube == 25) return 1;
    if (chamber == "EOL3A01" && ml == 2 && layer == 3 && tube ==  6) return 1;
    if (chamber == "EOL3A01" && ml == 2 && layer == 3 && tube ==  7) return 1;
    if (chamber == "EOL3A01" && ml == 2 && layer == 3 && tube ==  9) return 1;
    if (chamber == "EOL3A05" && ml == 1 && layer == 1 && tube ==  8) return 1;
    if (chamber == "EOL3A05" && ml == 1 && layer == 1 && tube == 48) return 1;
    if (chamber == "EOL3A05" && ml == 2 && layer == 1 && tube == 48) return 1;
    if (chamber == "EOL3A05" && ml == 2 && layer == 3 && tube == 14) return 1;
    if (chamber == "EOL3C07" && ml == 1 && layer == 2 && tube == 48) return 1;
    if (chamber == "EOL4A03" && ml == 1 && layer == 1 && tube == 32) return 1;
    if (chamber == "EOL4C05" && ml == 1 && layer == 1 && tube == 48) return 1;
    if (chamber == "EOL4C07" && ml == 1 && layer == 1 && tube ==  9) return 1;
    if (chamber == "EOL5C05" && ml == 2 && layer == 3 && tube == 48) return 1;
    if (chamber == "EOS2A04" && ml == 1 && layer == 3 && tube == 25) return 1;
    if (chamber == "EOS2C16" && ml == 2 && layer == 1 && tube ==  9) return 1;
    if (chamber == "EOS6A02" && ml == 1 && layer == 3 && tube ==  9) return 1;
    if (chamber == "EOS6C02" && ml == 2 && layer == 3 && tube == 34) return 1;
        
    return 0;
}

void BaseAnalysis::debug_luminosity()
{
    const xAOD::EventInfo* eventInfo = 0;
    evtStore()->retrieve(eventInfo);

    std::cout << std::endl;
    std::cout << "avgIntPerXing       " << eventInfo->averageInteractionsPerCrossing()        << std::endl;
    std::cout << "actIntPerXing       " << eventInfo->actualInteractionsPerCrossing()         << std::endl;
    std::cout << "bcid                " << eventInfo->bcid()                                  << std::endl;
    std::cout << "lbLuminosityPerBCID " << m_lumiTool->lbLuminosityPerBCID(eventInfo->bcid()) << std::endl;
    std::cout << "lbAverageLuminosity " << m_lumiTool->lbAverageLuminosity()                  << std::endl;
    std::cout << std::endl;
}

StatusCode BaseAnalysis::fill_eventinfo() {

    const xAOD::EventInfo* eventInfo = 0;
    CHECK(evtStore()->retrieve(eventInfo));

    RunNumber           = eventInfo->runNumber();
    EventNumber         = eventInfo->eventNumber();
    lbn                 = eventInfo->lumiBlock();
    bcid                = eventInfo->bcid();
    isMC                = eventInfo->eventType(xAOD::EventInfo::IS_SIMULATION);
    colliding_bunches   = CollidingBunches();
    avgIntPerXing       = eventInfo->averageInteractionsPerCrossing();
    actIntPerXing       = eventInfo->actualInteractionsPerCrossing();
    lbAverageLuminosity = m_lumiTool->lbAverageLuminosity();
    lbLuminosityPerBCID = m_lumiTool->lbLuminosityPerBCID(eventInfo->bcid());

    if (isMC)
        RunNumber = eventInfo->mcChannelNumber();

    return StatusCode::SUCCESS;
}

StatusCode BaseAnalysis::fill_trigger() {

    prescale_L1  = (isMC) ? 1.0 : m_trigDecTool->getPrescale("L1_ZB");
    prescale_HLT = (isMC) ? 1.0 : m_trigDecTool->getPrescale("HLT_noalg_zb_L1ZB");

    // std::cout << "isPassed:             L1_ZB " << m_trigDecTool->isPassed("L1_ZB")             << std::endl;
    // std::cout << "isPassed: HLT_noalg_zb_L1ZB " << m_trigDecTool->isPassed("HLT_noalg_zb_L1ZB") << std::endl;
    // std::cout << "isPassed:      HLT_j40_L1ZB " << m_trigDecTool->isPassed("HLT_j40_L1ZB")      << std::endl;
    // std::cout << "isPassed" << std::endl;

    return StatusCode::SUCCESS;
}

StatusCode BaseAnalysis::clear_branches() {

    mdt_chamber_n = 0;
    mdt_chamber_r.clear();
    mdt_chamber_phi.clear();
    mdt_chamber_eta.clear();
    mdt_chamber_name.clear();

    mdt_chamber_type.clear();
    mdt_chamber_side.clear();
    mdt_chamber_eta_station.clear();
    mdt_chamber_phi_sector.clear();

    mdt_chamber_tube_n.clear();
    mdt_chamber_tube_n_adc50.clear();
    mdt_chamber_tube_r.clear();
    mdt_chamber_tube_adc.clear();
    mdt_chamber_tube_tdc.clear();
    mdt_chamber_tube_id.clear();
    
    csc_chamber_n = 0;
    csc_chamber_r.clear();
    csc_chamber_phi.clear();
    csc_chamber_eta.clear();
    csc_chamber_type.clear();
    csc_chamber_side.clear();
    csc_chamber_phi_sector.clear();

    csc_chamber_cluster_n.clear();
    csc_chamber_cluster_n_qmax100.clear();
    csc_chamber_cluster_n_notecho.clear();
    csc_chamber_cluster_r.clear();
    csc_chamber_cluster_rmax.clear();
    csc_chamber_cluster_qsum.clear();
    csc_chamber_cluster_qmax.clear();
    csc_chamber_cluster_qleft.clear();
    csc_chamber_cluster_qright.clear();
    csc_chamber_cluster_strips.clear();
    csc_chamber_cluster_measuresphi.clear();
    csc_chamber_cluster_segment.clear();

    csc_segment_n = 0;
    csc_segment_r.clear();
    csc_segment_phi.clear();
    csc_segment_eta.clear();
    csc_segment_netaclusters.clear();
    csc_segment_nphiclusters.clear();
    csc_segment_name.clear();
    csc_segment_type.clear();
    csc_segment_side.clear();
    csc_segment_phi_sector.clear();

    return StatusCode::SUCCESS;
}

StatusCode BaseAnalysis::dump_mdt_geometry() {

    const Muon::MdtPrepDataContainer* mdts(0);
    CHECK(evtStore()->retrieve(mdts, "MDT_DriftCircles"));

    int ml     = 0;
    int ml_min = 0;
    int ml_max = 0;
    
    int layer     = 0;
    int layer_min = 0;
    int layer_max = 0;
    
    int tube     = 0;
    int tube_min = 0;
    int tube_max = 0;

    std::string chamber_name;
    double chamber_area = 0.0;
    double tube_area    = 0.0;
    double tube_radius  = 0.0;
    double tube_length  = 0.0;
    double tube_r       = 0.0;

    Identifier chamberid;
    Identifier channelid;

    for (auto chamber: *mdts) {

        chamberid = chamber->identify();

        const MuonGM::MdtReadoutElement* readout = m_detMgr->getMdtReadoutElement(chamberid);

        ml_min = m_mdtIdHelper->multilayerMin(chamberid);
        ml_max = m_mdtIdHelper->multilayerMax(chamberid);
        
        layer_min = m_mdtIdHelper->tubeLayerMin(chamberid);
        layer_max = m_mdtIdHelper->tubeLayerMax(chamberid);

        tube_min = m_mdtIdHelper->tubeMin(chamberid);
        tube_max = m_mdtIdHelper->tubeMax(chamberid);

        chamber_area = 0;
        chamber_name = OfflineToOnline(readout->getStationType(),
                                       readout->getStationEta(),
                                       readout->getStationPhi());

        for (ml = ml_min; ml <= ml_max; ml++){

            channelid = m_mdtIdHelper->channelID(readout->getStationType(),
                                                 readout->getStationEta(),
                                                 readout->getStationPhi(),
                                                 ml, 1, 1);
            const MuonGM::MdtReadoutElement* ml_readout = m_detMgr->getMdtReadoutElement(channelid);

            if (!ml_readout) ATH_MSG_FATAL("Failed to retrieve ML readout element.");

            for (layer = layer_min; layer <= layer_max; layer++){
                for (tube = tube_min; tube <= tube_max; tube++){

                    if (ignore_mdt_tube(chamber_name, ml, layer, tube))
                        continue;

                    tube_radius = ml_readout->innerTubeRadius();
                    tube_length = ml_readout->getActiveTubeLength(layer, tube);
                    tube_area   = tube_length * 2 * tube_radius;

                    const Amg::Vector3D& global_position = ml_readout->tubePos(ml, layer, tube);
                    tube_r = r(global_position.x(), global_position.y());

                    printf("tube %8s %2i %2i %2i %10.2f %10.2f %10.2f %10.2f \n",
                           chamber_name.c_str(), ml, layer, tube, tube_r,
                           tube_radius, tube_length, tube_area);

                    chamber_area = chamber_area + tube_area;
                }
            }
        }
        printf("chamber %8s %10.2f \n", chamber_name.c_str(), chamber_area);
    }

    return StatusCode::SUCCESS;
}

StatusCode BaseAnalysis::dump_csc_geometry() {

    int layer     = 0;
    int layer_min = 0;
    int layer_max = 0;

    int strip     = 0;
    int strip_min = 0;
    int strip_max = 0;

    std::string chamber_type;
    std::string chamber_name;
    int chamber_eta     = 0;
    int chamber_phi     = 0;
    double chamber_area = 0;
    double strip_length = 0;
    double strip_width  = 0;
    double strip_area   = 0;
    double strip_r      = 0;
    double epsilon      = 0;

    int measures_phi  = 0;

    Identifier chamberid;
    Identifier channelid;
    Identifier elementid;

    const Muon::CscPrepDataContainer* cscs(0);
    CHECK(evtStore()->retrieve(cscs, "CSC_Clusters"));

    for (auto chamber: *cscs) {

        if (chamber->size() == 0) {
            std::cout << " SKIPPING " << std::endl;
            continue;
        }

        chamberid = chamber->identify();

        const MuonGM::CscReadoutElement* readout(0);
        for (auto cluster: *chamber) {
            readout = cluster->detectorElement();
            break;
        }

        chamber_type = readout->getStationType();
        chamber_eta  = readout->getStationEta();
        chamber_phi  = readout->getStationPhi();

        chamber_name = OfflineToOnline(chamber_type, chamber_eta, chamber_phi);
        chamber_area = 0;

        layer_min = m_cscIdHelper->wireLayerMin(chamberid);
        layer_max = m_cscIdHelper->wireLayerMax(chamberid);

        strip_min = m_cscIdHelper->stripMin(chamberid);
        strip_max = m_cscIdHelper->stripMax(chamberid);

        for (layer = layer_min; layer <= layer_max; layer++){

            if (ignore_csc_layer(OfflineToOnlineSide(chamber_eta), 
                                 OfflineToOnlinePhi(chamber_type, chamber_phi), 
                                 layer)) continue;

            for (strip = strip_min; strip <= strip_max; strip++){
 
                strip_length = readout->stripLength(layer, measures_phi, strip, epsilon);
                strip_width  = readout->StripWidth(measures_phi);
                strip_area   = strip_length * strip_width;

                const Amg::Vector3D& global_position = readout->stripPos(chamber_eta,
                                                                         m_cscIdHelper->chamberLayer(chamberid),
                                                                         layer, measures_phi, strip);
                strip_r = r(global_position.x(), global_position.y());

                printf("strip %8s %2i %3i %10.2f %10.2f %10.2f %10.2f \n",
                       chamber_name.c_str(),
                       layer,
                       strip,
                       strip_r,
                       strip_length,
                       strip_width,
                       strip_area);

                chamber_area = chamber_area + strip_area;
            }
        }

        printf("chamber %8s %10.2f \n", chamber_name.c_str(), chamber_area);
    }

    return StatusCode::SUCCESS;
}

StatusCode BaseAnalysis::fill_mdt() {

    double tube_x = 0;
    double tube_y = 0;

    double ml_x = 0;
    double ml_y = 0;
    double ml_z = 0;

    std::string _name = "";
    std::string _type = "";
    int _eta = 0;
    int _phi = 0;

    Identifier tubeid;

    const Muon::MdtPrepDataContainer* mdts = 0;
    CHECK(evtStore()->retrieve(mdts, "MDT_DriftCircles"));

    for (auto chamber: *mdts) {

        int first = 1;

        for (auto tube: *chamber) {
          
            const MuonGM::MdtReadoutElement* readout = tube->detectorElement();
                
            const Amg::Vector3D&    global_position  = tube->globalPosition();
            const Amg::Vector3D& ml_global_position  = readout->globalPosition();
            
            if (first){

                ml_x = ml_global_position.x();
                ml_y = ml_global_position.y();
                ml_z = ml_global_position.z();
                
                _name = readout->getStationName();
                _type = readout->getStationType();
                _eta  = readout->getStationEta();
                _phi  = readout->getStationPhi();

                mdt_chamber_n++;
                mdt_chamber_r.push_back(    r(ml_x, ml_y));
                mdt_chamber_phi.push_back(phi(ml_x, ml_y));
                mdt_chamber_eta.push_back(eta(ml_x, ml_y, ml_z));

                mdt_chamber_name.push_back(OfflineToOnline(_type, _eta, _phi));
                mdt_chamber_type.push_back(_type);
                mdt_chamber_side.push_back(OfflineToOnlineSide(_eta));
                mdt_chamber_eta_station.push_back(OfflineToOnlineEta(_type, _eta));
                mdt_chamber_phi_sector.push_back( OfflineToOnlinePhi(_type, _phi));

                mdt_chamber_tube_n.push_back(0);
                mdt_chamber_tube_r.push_back(  std::vector<int>());
                mdt_chamber_tube_adc.push_back(std::vector<int>());
                mdt_chamber_tube_tdc.push_back(std::vector<int>());
                mdt_chamber_tube_id.push_back(std::vector<int>());
                mdt_chamber_tube_n_adc50.push_back(0);

                first = 0;
            }

            tubeid = tube->identify();
            if (ignore_mdt_tube(mdt_chamber_name.back(),
                                m_mdtIdHelper->multilayer(tubeid),
                                m_mdtIdHelper->tubeLayer(tubeid),
                                m_mdtIdHelper->tube(tubeid)))
                continue;

            tube_x = global_position.x();
            tube_y = global_position.y();

            mdt_chamber_tube_n.back()++;
            mdt_chamber_tube_r.back().push_back((int)(r(tube_x, tube_y)));
            mdt_chamber_tube_adc.back().push_back(tube->adc());
            mdt_chamber_tube_tdc.back().push_back(tube->tdc());
            mdt_chamber_tube_id.back().push_back(m_mdtIdHelper->multilayer(tubeid)*1000 + 
                                                 m_mdtIdHelper->tubeLayer(tubeid)*100   + 
                                                 m_mdtIdHelper->tube(tubeid));

            if (tube->adc() > 50)
                mdt_chamber_tube_n_adc50.back()++;

            // std::cout << mdt_chamber_name.back()            << " "
            //           << mdt_chamber_type.back()            << " "
            //           << mdt_chamber_side.back()            << " "
            //           << mdt_chamber_eta_station.back()     << " "
            //           << mdt_chamber_phi_sector.back()      << " "
            //           << m_mdtIdHelper->multilayer(tubeid)  << " "
            //           << m_mdtIdHelper->tubeLayer(tubeid)   << " "
            //           << m_mdtIdHelper->tube(tubeid)        << " "
            //           << tube->adc()                     << " "
            //           << tube->tdc()                     << "          "
            //           << tubeid                             << " "
            //           << std::endl;
            
            // id = XYZZ
            //  X = multilayer
            //  Y = tubelayer
            // ZZ = tube

        }
    }

    return StatusCode::SUCCESS;
}

StatusCode BaseAnalysis::fill_csc() {

    double cluster_x = 0;
    double cluster_y = 0;

    double ch_x = 0;
    double ch_y = 0;
    double ch_z = 0;

    int measures_phi = 0;
    int qleft        = 0;
    int qright       = 0;
    int qmax         = 0;
    int rmax         = 0;
    int on_segment   = 0;

    std::string _type = "";
    int _eta = 0;
    int _phi = 0;

    unsigned int iter    = 0;
    unsigned int itermax = 0;

    Identifier qmaxid;

    ICscClusterFitter::StripFitList stripfits;
    std::vector<Identifier>         stripids;

    const Muon::CscPrepDataContainer* cscs = 0;
    CHECK(evtStore()->retrieve(cscs, "CSC_Clusters"));

    const xAOD::MuonSegmentContainer* segments     = 0;
    const Trk::SegmentCollection*     segments_trk = 0;
    const Trk::Segment*               segment_trk  = 0;
    CHECK(evtStore()->retrieve(segments,     "MuonSegments"));
    CHECK(evtStore()->retrieve(segments_trk, "MuonSegments"));

    for (auto chamber: *cscs) {

        int first = 1;

        for (auto cluster: *chamber) {

            Identifier clusterid                     = cluster->identify();
            const MuonGM::CscReadoutElement* readout = cluster->detectorElement();
            const Amg::Vector3D&    global_position  = cluster->globalPosition();
            const Amg::Vector3D& ch_global_position  = readout->globalPosition();

            if (first){

                ch_x = ch_global_position.x();
                ch_y = ch_global_position.y();
                ch_z = ch_global_position.z();

                _type = readout->getStationType();
                _eta  = readout->getStationEta();
                _phi  = readout->getStationPhi();

                csc_chamber_n++;
                csc_chamber_r.push_back((int)r(ch_x, ch_y));
                csc_chamber_phi.push_back(phi(ch_x, ch_y));
                csc_chamber_eta.push_back(eta(ch_x, ch_y, ch_z));

                csc_chamber_type.push_back(_type);
                csc_chamber_side.push_back(OfflineToOnlineSide(_eta));
                csc_chamber_phi_sector.push_back(OfflineToOnlinePhi(_type, _phi));

                csc_chamber_cluster_n.push_back(0);
                csc_chamber_cluster_r.push_back(     std::vector<int>());
                csc_chamber_cluster_rmax.push_back(  std::vector<int>());
                csc_chamber_cluster_qsum.push_back(  std::vector<int>());
                csc_chamber_cluster_qmax.push_back(  std::vector<int>());
                csc_chamber_cluster_qleft.push_back( std::vector<int>());
                csc_chamber_cluster_qright.push_back(std::vector<int>());
                csc_chamber_cluster_strips.push_back(std::vector<int>());
                csc_chamber_cluster_segment.push_back(std::vector<int>());
                csc_chamber_cluster_n_qmax100.push_back(0);
                csc_chamber_cluster_n_notecho.push_back(0);

                first = 0;
            }

            if (ignore_csc_layer(csc_chamber_side.back(), 
                                 csc_chamber_phi_sector.back(), 
                                 m_cscIdHelper->wireLayer(clusterid))) continue;

            measures_phi = m_cscIdHelper->measuresPhi(clusterid);
            if (measures_phi) continue;

            cluster_x = global_position.x();
            cluster_y = global_position.y();
            
            csc_chamber_cluster_n.back()++;
            csc_chamber_cluster_r.back().push_back((int)(r(cluster_x, cluster_y)));
            csc_chamber_cluster_qsum.back().push_back(cluster->charge());
            csc_chamber_cluster_strips.back().push_back(cluster->rdoList().size());

            // below: retrieving the highest-charge strip (qmax)
            stripfits.clear();
            m_clusterUtilTool->getStripFits(cluster, stripfits);

            stripids.clear();
            stripids = cluster->rdoList();

            if (stripids.size() != stripfits.size()) 
                ATH_MSG_FATAL("Number of strip ids is not equal to number of strip fits!");

            qmax = 0; qleft = 0; qright = 0;
            itermax = stripfits.size();
            for (iter = 0; iter < itermax; ++iter){
                if (stripfits[iter].charge > qmax){
                    // if only C++ had a zip function
                    qmax   =                      (int)(stripfits[iter  ].charge);
                    qleft  = (iter > 0)         ? (int)(stripfits[iter-1].charge) : 0;
                    qright = (iter < itermax-1) ? (int)(stripfits[iter+1].charge) : 0;
                    qmaxid = stripids[iter];
                }
            }
            if (qmax == 0) 
                ATH_MSG_FATAL("Max strip charge is zero!");

            const Amg::Vector3D& qmax_position = readout->stripPos(qmaxid);
            rmax = (int)(r(qmax_position.x(), qmax_position.y()));
            
            csc_chamber_cluster_rmax.back().push_back(rmax);
            csc_chamber_cluster_qmax.back().push_back(qmax);
            csc_chamber_cluster_qleft.back().push_back(qleft);
            csc_chamber_cluster_qright.back().push_back(qright);

            if (csc_chamber_cluster_qmax.back().back() > 100*1000.0)
                csc_chamber_cluster_n_qmax100.back()++;
            if (qleft > 0 && qright > 0)
                csc_chamber_cluster_n_notecho.back()++;

            // below: segment navigation
            on_segment = 0;

            for (auto segment: *segments) {

                if (segment->chamberIndex() != Muon::MuonStationIndex::CSS && 
                    segment->chamberIndex() != Muon::MuonStationIndex::CSL)
                    continue;

                // fuck EDMs
                segment_trk = segments_trk->at(segment->muonSegment().index());

                for (auto measurement: segment_trk->containedMeasurements()){
                    if (clusterid == ((const Trk::RIO_OnTrack*)(measurement))->identify()){
                        on_segment = 1;
                        break;
                    }
                }
            }
            csc_chamber_cluster_segment.back().push_back(on_segment);
        }
    }

    return StatusCode::SUCCESS;
}

StatusCode BaseAnalysis::fill_csc_segments() {

    int _eta = 0;
    int _phi = 0;
    std::string _type;
    Identifier id;

    const xAOD::MuonSegmentContainer* segments     = 0;
    const Trk::SegmentCollection*     segments_trk = 0;
    const Trk::Segment*               segment_trk  = 0;
    CHECK(evtStore()->retrieve(segments,     "MuonSegments"));
    CHECK(evtStore()->retrieve(segments_trk, "MuonSegments"));

    for (auto segment: *segments) {

        if (segment->chamberIndex() != Muon::MuonStationIndex::CSS && 
            segment->chamberIndex() != Muon::MuonStationIndex::CSL)
            continue;

        _eta  = segment->etaIndex();
        _phi  = segment->sector();
        _type = segment->chamberIndex() == Muon::MuonStationIndex::CSS ? "CSS" : "CSL";

        csc_segment_n++;
        csc_segment_r.push_back((int)r(segment->x(), segment->y()));
        csc_segment_phi.push_back( phi(segment->x(), segment->y()));
        csc_segment_eta.push_back( eta(segment->x(), segment->y(), segment->z()));

        csc_segment_name.push_back(OfflineToOnline(_type, _eta, _phi));
        csc_segment_type.push_back(_type);
        csc_segment_side.push_back(OfflineToOnlineSide(_eta));
        csc_segment_phi_sector.push_back( OfflineToOnlinePhi(_type, _phi));

        csc_segment_netaclusters.push_back(0);
        csc_segment_nphiclusters.push_back(0);

        segment_trk = segments_trk->at(segment->muonSegment().index());

        for (auto measurement: segment_trk->containedMeasurements()){
            id = ((const Trk::RIO_OnTrack*)(measurement))->identify();

            if (m_cscIdHelper->measuresPhi(id)) csc_segment_nphiclusters.back()++;
            else                                csc_segment_netaclusters.back()++;
        }
    }

    return StatusCode::SUCCESS;
}
