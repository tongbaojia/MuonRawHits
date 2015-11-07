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

#include "TrigDecisionTool/ChainGroup.h"
#include "TrigDecisionTool/TrigDecisionTool.h"

BaseAnalysis::BaseAnalysis( const std::string& name, ISvcLocator* pSvcLocator ) : 
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

    // if (! m_trigDecTool->isPassed("HLT_mu20_iloose_L1MU15")){
    //     return StatusCode::SUCCESS;
    // }

    CHECK(clear_branches());
  
    CHECK(fill_eventinfo());
    CHECK(fill_trigger());
    CHECK(fill_mdt());
    CHECK(fill_csc());

    // CHECK(dump_mdt_geometry());
    // CHECK(dump_csc_geometry());

    tree->Fill();

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
    tree->Branch("colliding_bunches",   &colliding_bunches);
    tree->Branch("avgIntPerXing",       &avgIntPerXing);
    tree->Branch("actIntPerXing",       &actIntPerXing);
    tree->Branch("lbAverageLuminosity", &lbAverageLuminosity);
    tree->Branch("lbLuminosityPerBCID", &lbLuminosityPerBCID);

    tree->Branch("prescale_L1",  &prescale_L1);
    tree->Branch("prescale_HLT", &prescale_HLT);

    tree->Branch("mdt_chamber_n",           &mdt_chamber_n);
    tree->Branch("mdt_chamber_r",           &mdt_chamber_r);
    tree->Branch("mdt_chamber_phi",         &mdt_chamber_phi);
    tree->Branch("mdt_chamber_eta",         &mdt_chamber_eta);
    tree->Branch("mdt_chamber_type",        &mdt_chamber_type);
    tree->Branch("mdt_chamber_side",        &mdt_chamber_side);
    tree->Branch("mdt_chamber_eta_station", &mdt_chamber_eta_station);
    tree->Branch("mdt_chamber_phi_sector",  &mdt_chamber_phi_sector);
    tree->Branch("mdt_chamber_tube_n",      &mdt_chamber_tube_n);
    tree->Branch("mdt_chamber_tube_r",      &mdt_chamber_tube_r);
    tree->Branch("mdt_chamber_tube_adc",    &mdt_chamber_tube_adc);
    
    tree->Branch("csc_chamber_n",              &csc_chamber_n);
    tree->Branch("csc_chamber_r",              &csc_chamber_r);
    tree->Branch("csc_chamber_phi",            &csc_chamber_phi);
    tree->Branch("csc_chamber_eta",            &csc_chamber_eta);
    tree->Branch("csc_chamber_type",           &csc_chamber_type);
    tree->Branch("csc_chamber_side",           &csc_chamber_side);
    tree->Branch("csc_chamber_phi_sector",     &csc_chamber_phi_sector);
    tree->Branch("csc_chamber_cluster_n",      &csc_chamber_cluster_n);
    tree->Branch("csc_chamber_cluster_r",      &csc_chamber_cluster_r);
    tree->Branch("csc_chamber_cluster_rmax",   &csc_chamber_cluster_rmax);
    tree->Branch("csc_chamber_cluster_qsum",   &csc_chamber_cluster_qsum);
    tree->Branch("csc_chamber_cluster_qmax",   &csc_chamber_cluster_qmax);
    tree->Branch("csc_chamber_cluster_strips", &csc_chamber_cluster_strips);

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
    if (side == "C" && phi_sector == 1 && layer == 1) return 1;
    if (side == "C" && phi_sector == 3 && layer == 2) return 1;
    if (side == "A" && phi_sector == 9 && layer == 1) return 1;

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
    colliding_bunches   = CollidingBunches();
    avgIntPerXing       = eventInfo->averageInteractionsPerCrossing();
    actIntPerXing       = eventInfo->actualInteractionsPerCrossing();
    lbAverageLuminosity = m_lumiTool->lbAverageLuminosity();
    lbLuminosityPerBCID = m_lumiTool->lbLuminosityPerBCID(eventInfo->bcid());

    return StatusCode::SUCCESS;
}

StatusCode BaseAnalysis::fill_trigger() {

    // std::cout << "fill_trigger: before" << std::endl << std::endl;

    // triggers.clear();
    // // triggers = m_trigDecTool->getListOfTriggers(".*");

    // for (unsigned int itrig = 0; itrig < triggers.size(); ++itrig) {
    //     // std::cout << triggers.at(itrig).c_str() << std::endl;
    // }

    // std::cout << "L1_ZB "   << m_trigDecTool->isPassed("L1_ZB")   << std::endl;
    // std::cout << "L1_MU15 " << m_trigDecTool->isPassed("L1_MU15") << std::endl;

    // std::cout << "Prescale:             L1_ZB " << m_trigDecTool->getPrescale("L1_ZB")             << std::endl;
    // std::cout << "Prescale: HLT_noalg_zb_L1ZB " << m_trigDecTool->getPrescale("HLT_noalg_zb_L1ZB") << std::endl;

    // std::cout << "fill_trigger: " << triggers.size() << std::endl << std::endl;
    // std::cout << "fill_trigger: after" <<  std::endl << std::endl;

    prescale_L1  = m_trigDecTool->getPrescale("L1_ZB");
    prescale_HLT = m_trigDecTool->getPrescale("HLT_noalg_zb_L1ZB");

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
    mdt_chamber_tube_r.clear();
    mdt_chamber_tube_adc.clear();
    
    csc_chamber_n = 0;
    csc_chamber_r.clear();
    csc_chamber_phi.clear();
    csc_chamber_eta.clear();
    csc_chamber_type.clear();
    csc_chamber_side.clear();
    csc_chamber_phi_sector.clear();

    csc_chamber_cluster_n.clear();
    csc_chamber_cluster_r.clear();
    csc_chamber_cluster_rmax.clear();
    csc_chamber_cluster_qsum.clear();
    csc_chamber_cluster_qmax.clear();
    csc_chamber_cluster_strips.clear();
    csc_chamber_cluster_measuresphi.clear();

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

    Muon::MdtPrepDataContainer::const_iterator chamber;
    for (chamber = mdts->begin(); chamber != mdts->end() ; ++chamber) {

        chamberid = (*chamber)->identify();

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
    int first         = 0;

    Identifier chamberid;
    Identifier channelid;
    Identifier elementid;

    const Muon::CscPrepDataContainer* cscs(0);
    CHECK(evtStore()->retrieve(cscs, "CSC_Clusters"));
    Muon::CscPrepDataContainer::const_iterator chamber;

    for (chamber = cscs->begin(); chamber != cscs->end(); ++chamber) {

        chamberid = (*chamber)->identify();
        // m_cscIdHelper->print(chamberid);

        // doesnt work
        // const MuonGM::CscReadoutElement* readout = m_detMgr->getCscReadoutElement(chamberid);

        // doesnt work
        // elementid = m_cscIdHelper->elementID(m_cscIdHelper->stationName(chamberid),
        //                                      m_cscIdHelper->stationEta(chamberid), 
        //                                      m_cscIdHelper->stationPhi(chamberid));
        // const MuonGM::CscReadoutElement* readout = m_detMgr->getCscReadoutElement(elementid);

        // doesnt work
        // channelid = m_cscIdHelper->channelID(m_cscIdHelper->stationName(chamberid),
        //                                      m_cscIdHelper->stationEta(chamberid),
        //                                      m_cscIdHelper->stationPhi(chamberid),
        //                                      1, 1, measures_phi, 1);
        // const MuonGM::CscReadoutElement* readout = m_detMgr->getCscReadoutElement(channelid);

        // if (!readout){
        //     std::cout << "--- fuckers ---" << std::endl;
        //     continue;
        // }

        if ((*chamber)->size() == 0) {
            std::cout << " SKIPPING " << std::endl;
            continue;
        }

        first = 1;
        const MuonGM::CscReadoutElement* readout(0);
        for (Muon::CscPrepDataCollection::const_iterator cluster = (*chamber)->begin(); cluster != (*chamber)->end(); ++cluster) {
            if (first) readout = (*cluster)->detectorElement();
            else continue;
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

    const Muon::MdtPrepDataContainer* mdts(0);
    CHECK(evtStore()->retrieve(mdts, "MDT_DriftCircles"));

    Muon::MdtPrepDataContainer::const_iterator chamber;
    for (chamber = mdts->begin(); chamber != mdts->end() ; ++chamber) {

        int first = 1;

        for (Muon::MdtPrepDataCollection::const_iterator tube = (*chamber)->begin(); tube != (*chamber)->end(); ++tube){
          
            const MuonGM::MdtReadoutElement* readout = (*tube)->detectorElement();
                
            const Amg::Vector3D&    global_position  = (*tube)->globalPosition();
            const Amg::Vector3D& ml_global_position  = readout->globalPosition();
            
            if (first){

                ml_x = ml_global_position.x();
                ml_y = ml_global_position.y();
                ml_z = ml_global_position.z();
                
                std::string _type = readout->getStationType();
                int _eta          = readout->getStationEta();
                int _phi          = readout->getStationPhi();

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

                first = 0;
            }
            
            tube_x = global_position.x();
            tube_y = global_position.y();

            mdt_chamber_tube_n.back()++;
            mdt_chamber_tube_r.back().push_back((int)(r(tube_x, tube_y)));
            mdt_chamber_tube_adc.back().push_back((*tube)->adc());
            
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
    int qmax         = 0;
    int rmax         = 0;

    Identifier qmaxid;

    ICscClusterFitter::StripFitList stripfits;
    std::vector<Identifier>         stripids;

    const Muon::CscPrepDataContainer* cscs(0);
    CHECK(evtStore()->retrieve(cscs, "CSC_Clusters"));

    Muon::CscPrepDataContainer::const_iterator chamber;
    for (chamber = cscs->begin(); chamber != cscs->end(); ++chamber) {

        int first = 1;

        for (Muon::CscPrepDataCollection::const_iterator cluster = (*chamber)->begin(); cluster != (*chamber)->end(); ++cluster) {

            const MuonGM::CscReadoutElement* readout = (*cluster)->detectorElement();
                
            const Amg::Vector3D&    global_position = (*cluster)->globalPosition();
            const Amg::Vector3D& ch_global_position = readout->globalPosition();

            const Muon::CscPrepData& prd = **cluster;
            Identifier clusterid = prd.identify();

            if (first){

                ch_x = ch_global_position.x();
                ch_y = ch_global_position.y();
                ch_z = ch_global_position.z();

                std::string _type = readout->getStationType();
                int _eta          = readout->getStationEta();
                int _phi          = readout->getStationPhi();

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
                csc_chamber_cluster_strips.push_back(std::vector<int>());

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
            csc_chamber_cluster_qsum.back().push_back((*cluster)->charge());
            csc_chamber_cluster_strips.back().push_back((*cluster)->rdoList().size());

            // below: retrieving the highest-charge strip (qmax)
            stripfits.clear();
            m_clusterUtilTool->getStripFits((*cluster), stripfits);

            stripids.clear();
            stripids = (*cluster)->rdoList();

            if (stripids.size() != stripfits.size()) 
                ATH_MSG_FATAL("Number of strip ids is not equal to number of strip fits!");

            qmax = 0;
            for (unsigned int iter = 0; iter < stripfits.size(); ++iter){
                if (stripfits[iter].charge > qmax){
                    // if only C++ had a zip function
                    qmax = (int)(stripfits[iter].charge);
                    qmaxid = stripids[iter];
                }
            }

            if (qmax == 0) 
                ATH_MSG_FATAL("Max strip charge is zero!");

            const Amg::Vector3D& qmax_position = readout->stripPos(qmaxid);
            rmax = (int)(r(qmax_position.x(), qmax_position.y()));

            csc_chamber_cluster_qmax.back().push_back(qmax);
            csc_chamber_cluster_rmax.back().push_back(rmax);

        }
    }

    return StatusCode::SUCCESS;
}
