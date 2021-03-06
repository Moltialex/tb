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

        //Warning: This analyses does not require lepton to be isolated

        Manager()->AddRegionSelection("all");
        Manager()->AddRegionSelection("electron final state");
        Manager()->AddRegionSelection("muon final state");

        //Manager()->AddCut("trigger");
        //Manager()->AddCut("pt lepton> 180 GeV");
        //Manager()->AddCut("delta R (lepton, nearest jet)");
        Manager()->AddCut("1+ candidate lepton");
        Manager()->AddCut("2+ candidate jets");
        //Manager()->AddCut("2+ candidate leptons");

        string SRForElectron[]= {"electron final state"};
        Manager()->AddCut("1+ electrons", SRForElectron);
        Manager()->AddCut("pt leading jet> 350 GeV", SRForElectron);
        Manager()->AddCut("pt subleading jet> 30 GeV", SRForElectron);
        Manager()->AddCut("b-tagging electron", SRForElectron);
        Manager()->AddCut("missing ET> 120 GeV", SRForElectron);
        Manager()->AddCut("delta phi (missing ET, electron)< 2 rad", SRForElectron);

        string SRForMuon[]= {"muon final state"};
        Manager()->AddCut("1+ muons", SRForMuon);
        Manager()->AddCut("pt leading jet> 450 GeV", SRForMuon);
        Manager()->AddCut("pt subleading jet> 30 GeV", SRForMuon);
        Manager()->AddCut("b-tagging muon", SRForMuon);
        Manager()->AddCut("missing ET> 50 GeV", SRForMuon);

        string SRForMass[]= {"electron final state", "muon final state"};
        Manager()->AddCut("pT top> 250 GeV", SRForMass);
        Manager()->AddCut("pT leading jet+ subleading jet> 350 GeV", SRForMass);

        Manager()->AddHisto("Invariant mass", 500, 0, 5000);

        return true;
}

// -----------------------------------------------------------------------------
// Finalize
// function called one time at the end of the analysis
// -----------------------------------------------------------------------------
void cms_b2g_15_004::Finalize(const SampleFormat& summary, const std::vector<SampleFormat>& files)
{

}

// -----------------------------------------------------------------------------
// Execute
// function called each time one event is read
// -----------------------------------------------------------------------------
bool cms_b2g_15_004::Execute(SampleFormat& sample, const EventFormat& event)
{
        if(event.rec()!= 0){
                double myEventWeight;
                if(Configuration().IsNoEventWeight()) myEventWeight=1.;
                else if(event.mc()->weight()!=0.) myEventWeight=event.mc()->weight();
                else{
                        WARNING << "Found one event with a zero weight. Skipping..." << endmsg;
                        return false;
                }
                Manager()->InitializeForNewEvent(myEventWeight);
                Manager()->SetCurrentEventWeight(myEventWeight);

                //Declare empty containers
                vector<const RecJetFormat*> myJets;
                vector<const RecLeptonFormat*> myElectrons;
                vector<const RecLeptonFormat*> myMuons;

                if(!Manager()->ApplyCut(event.rec()->electrons().size()+ event.rec()->muons().size()>= 1,"1+ candidate lepton")) return true;
                if(!Manager()->ApplyCut(event.rec()->jets().size()>1, "2+ candidate jets")) return true;

                for(unsigned int i= 0; i< event.rec()->electrons().size(); i++){
                        bool good= true;
                        const RecLeptonFormat *CurrentElectron= &(event.rec()->electrons()[i]);

                        for(unsigned int j= 0; j< event.rec()->jets().size(); j++){
                                const RecJetFormat *CurrentJet= &(event.rec()->jets()[j]);
                                double deltaR= CurrentElectron->dr(CurrentJet);
                                double angle= CurrentElectron->angle(CurrentJet);
                                double relapt= CurrentElectron->momentum().Pt()*sin(angle);
                                if(!(deltaR> 0.4 || relapt> 60.)) good= false;
                        }

                        double pt= CurrentElectron->momentum().Pt();
                        double eta= CurrentElectron->momentum().Eta();
                        if(pt> 180. && fabs(eta)< 2.5 && good) myElectrons.push_back(CurrentElectron);
                }

                for(unsigned int i= 0; i< event.rec()->muons().size(); i++){
                        bool good= true;
                        const RecLeptonFormat *CurrentMuon= &(event.rec()->muons()[i]);

                        for(unsigned int j= 0; j< event.rec()->jets().size(); j++){
                                const RecJetFormat *CurrentJet= &(event.rec()->jets()[j]);
                                double deltaR= CurrentMuon->dr(CurrentJet);
                                double angle= CurrentMuon->angle(CurrentJet);
                                double relapt= CurrentMuon->momentum().Pt()*sin(angle);
                                if(!(deltaR> 0.4 || relapt> 50.)) good= false;
                        }

                        double pt= CurrentMuon->momentum().Pt();
                        double eta= CurrentMuon->momentum().Eta();
                        if(pt> 180. && fabs(eta)< 2.1 && good) myMuons.push_back(CurrentMuon);
                }

                for(unsigned int j= 0; j< event.rec()->jets().size(); j++){
                        const RecJetFormat *CurrentJet= &(event.rec()->jets()[j]);
                        myJets.push_back(CurrentJet);
                }
                SORTER->sort(myJets);

                if(!Manager()->ApplyCut(myElectrons.size()>=1, "1+ electrons")) return true;
                if(!Manager()->ApplyCut(myJets[0]->momentum().Pt()> 350, "pt leading jet> 350 GeV")) return true;
                if(!Manager()->ApplyCut(myJets[1]->momentum().Pt()> 30., "pt subleading jet> 30 GeV")) return true;
                if(!Manager()->ApplyCut((myJets[0]->btag() || myJets[1]->btag()), "b-tagging electron")) return true;
                if(!Manager()->ApplyCut(event.rec()->MET().et()> 120., "missing ET> 120 GeV")) return true;
                if(myElectrons.size()>=1){
                        double phi= event.rec()->MET().phi()- myElectrons[0]->phi();
                        if(!Manager()->ApplyCut(fabs(phi)< 2.,"delta phi (missing ET, electron)< 2 rad")) return true;
                }

                if(!Manager()->ApplyCut(myMuons.size()>=1, "1+ muons")) return true;
                if(!Manager()->ApplyCut(myJets[0]->momentum().Pt()> 450., "pt leading jet> 450 GeV")) return true;
                if(!Manager()->ApplyCut(myJets[1]->momentum().Pt()> 30., "pt subleading jet> 30 GeV")) return true;
                if(!Manager()->ApplyCut((myJets[0]->btag() || myJets[1]->btag()), "b-tagging muon")) return true;
                if(!Manager()->ApplyCut(event.rec()->MET().et()> 50., "missing ET> 50 GeV")) return true;

                if(myElectrons.size()>=1){
                        double Wmass= (myElectrons[0]->momentum()+event.rec()->MET().momentum()).M();
                        double test= sqrt(2*myElectrons[0]->pt()*event.rec()->MET().pt()*(cosh(myElectrons[0]->eta() -event.rec()->MET().eta())- cos(myElectrons[0]->phi() - event.rec()->MET().phi())));
                        if(pow((myElectrons[0]->px()*event.rec()->MET().px() + myElectrons[0]->py()*event.rec()->MET().py() + (80.4*80.4)/2),2)- event.rec()->MET().et()*event.rec()->MET().et()*
                                (myElectrons[0]->e()*myElectrons[0]->e() - myElectrons[0]->pz()*myElectrons[0]->pz())>0){
                        double pzneutri1= (myElectrons[0]->pz()*(myElectrons[0]->px()*event.rec()->MET().px() + myElectrons[0]->py()*event.rec()->MET().py() + (80.4*80.4)/2) + 
                                        myElectrons[0]->e()* sqrt(pow((myElectrons[0]->px()*event.rec()->MET().px() + myElectrons[0]->py()*event.rec()->MET().py() + (80.4*80.4)/2),2)- event.rec()->MET().et()*event.rec()->MET().et()*
                                                (myElectrons[0]->e()*myElectrons[0]->e() - myElectrons[0]->pz()*myElectrons[0]->pz())))/(myElectrons[0]->e()*myElectrons[0]->e()- myElectrons[0]->pz()*myElectrons[0]->pz());
                        double pzneutri2= (myElectrons[0]->pz()*(myElectrons[0]->px()*event.rec()->MET().px() + myElectrons[0]->py()*event.rec()->MET().py() + (80.4*80.4)/2) - 
                                        myElectrons[0]->e()* sqrt(pow((myElectrons[0]->px()*event.rec()->MET().px() + myElectrons[0]->py()*event.rec()->MET().py() + (80.4*80.4)/2),2)- event.rec()->MET().et()*event.rec()->MET().et()*
                                                (myElectrons[0]->e()*myElectrons[0]->e() - myElectrons[0]->pz()*myElectrons[0]->pz())))/(myElectrons[0]->e()*myElectrons[0]->e()- myElectrons[0]->pz()*myElectrons[0]->pz());
                        cout<<"pz1: "<< pzneutri1 << endl; 
                        cout<<"pz2: "<< pzneutri2 << endl; 
                        double nrjneut1= sqrt(event.rec()->MET().px()*event.rec()->MET().px() + event.rec()->MET().py()*event.rec()->MET().py() + pzneutri1*pzneutri1); 
                        double nrjneut2= sqrt(event.rec()->MET().px()*event.rec()->MET().px() + event.rec()->MET().py()*event.rec()->MET().py() + pzneutri2*pzneutri2); 
                        cout<<"e neut1: "<< nrjneut1 << endl;
                        cout<<"e neut2: "<< nrjneut2 << endl;
                        double lachose= sqrt(2*(nrjneut1*myElectrons[0]->e()-myElectrons[0]->px()*event.rec()->MET().px()-myElectrons[0]->py()*event.rec()->MET().py()-myElectrons[0]->pz()*pzneutri1));
                        cout<<"la masse: "<< lachose << endl;
                                                }
                        //cout<<"W: "<< Wmass << endl;
                        //cout<<"E neutri: "<< event.rec()->MET().e() << endl;
                        //cout<<"pt: "<< event.rec()->MET().pt() << endl;
                        //cout<<"px: "<< event.rec()->MET().px() << endl;
                        //cout<<"py: "<< event.rec()->MET().py() << endl;
                        //cout<<"pz: "<< event.rec()->MET().pz() << endl;
                        //cout<<"truc: "<< sqrt(event.rec()->MET().px()*event.rec()->MET().px()+event.rec()->MET().py()*event.rec()->MET().py()) << endl;
                        //cout<<"test: "<< test << endl;
                }

        }

        return true;
}

