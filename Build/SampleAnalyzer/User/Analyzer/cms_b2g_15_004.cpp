#include "SampleAnalyzer/User/Analyzer/cms_b2g_15_004.h"
using namespace MA5;
using namespace std;

// -----------------------------------------------------------------------------
// Initialize
// function called one time at the beginning of the analysis
// -----------------------------------------------------------------------------
bool cms_b2g_15_004::Initialize(const MA5::Configuration& cfg, const std::map<std::string,std::string>& parameters)
{
        // Information on the analysis, authors, ...
        // VERY IMPORTANT FOR DOCUMENTATION, TRACEABILITY, BUG REPORTS
        INFO << "        <><><><><><><><><><><><><><><><><><><><><><><><><>" << endmsg;
        INFO << "        <>  Analysis: CMS-B2G-15-004, arXiv:XXXX.XXXXX  <>" << endmsg;
        INFO << "        <>            (W' boson, top and bottom quark, lepton final state)       <>" << endmsg;
        INFO << "        <>  Recasted by: A.MOLTER                       <>" << endmsg;
        INFO << "        <>  Contact: alexis.molter@protonmail.com       <>" << endmsg;
        INFO << "        <>  Based on MadAnalysis 5 v1.3                 <>" << endmsg;
        INFO << "        <>  For more information, see                   <>" << endmsg;
        INFO << "        <>  http://madanalysis.irmp.ucl.ac.be/wiki/PhysicsAnalysisDatabase" << endmsg;

        Manager()->AddRegionSelection("all");
        Manager()->AddRegionSelection("1 lepton final state");
        Manager()->AddRegionSelection("2+ leptons final state");
        Manager()->AddRegionSelection("electron final state");
        Manager()->AddRegionSelection("muon final state");
        Manager()->AddRegionSelection("1 b-tagged");
        Manager()->AddRegionSelection("2 b-tagged");

        Manager()->AddCut("trigger");
        Manager()->AddCut("pt lepton> 180 GeV");
        Manager()->AddCut("delta R (lepton, nearest jet)");
        Manager()->AddCut("1 lepton final state");
        Manager()->AddCut("2+ leptons final state");

        string SRForElectron[]= {"1 lepton final state", "2+ leptons final state", "electron final state"};
        Manager()->AddCut("pt relatif (electron, nearest jet)", SRForElectron);
        Manager()->AddCut("pt leading jet> 350 GeV", SRForElectron);
        Manager()->AddCut("pt subleading jet> 30 GeV", SRForElectron);
        Manager()->AddCut("missing ET> 120 GeV", SRForElectron);
        Manager()->AddCut("delta phi (missing ET, electron)< 2 rad", SRForElectron);

        string SRForMuon[]= {"1 lepton final state", "2+ leptons final state", "muon final state"};
        Manager()->AddCut("pt relatif (electron, nearest jet)", SRForMuon);
        Manager()->AddCut("pt leading jet> 450 GeV", SRForMuon);
        Manager()->AddCut("pt subleading jet> 30 GeV", SRForMuon);
        Manager()->AddCut("missing ET> 50 GeV", SRForMuon);

        return true;
}

// -----------------------------------------------------------------------------
// Finalize
// function called one time at the end of the analysis
// -----------------------------------------------------------------------------
void cms_b2g_15_004::Finalize(const SampleFormat& summary, const std::vector<SampleFormat>& files)
{
        cout << "BEGIN Finalization" << endl;
        // saving histos
        cout << "END   Finalization" << endl;
}

// -----------------------------------------------------------------------------
// Execute
// function called each time one event is read
// -----------------------------------------------------------------------------
bool cms_b2g_15_004::Execute(SampleFormat& sample, const EventFormat& event)
{
        return true;
}

