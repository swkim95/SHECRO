#include "CorFlatG4Reader.hh"
#include "G4GenericMessenger.hh"

#include <iostream>
#include <fstream>
#include <sstream> 
#include <iomanip> 

CorFlatG4Reader::CorFlatG4Reader(G4int seed, G4String file_path)
: verbose(1), n_event_count(0), fMessenger(0), fSeed(seed), fCorsikaPath(file_path)
{
  DefineCommands();
  Initialize();
}

CorFlatG4Reader::~CorFlatG4Reader() {
  delete aTree;
  delete aFile;
  delete fMessenger;
}

void CorFlatG4Reader::Initialize() {
  // std::stringstream stream;
	// stream << std::setw(6) << std::setfill('0') << std::to_string(fSeed);
  // fCorsikaPath += "_"+stream.str()+".root";
  fCorsikaPath += "_"+std::to_string(fSeed)+".root";
  aFile = TFile::Open(fCorsikaPath.c_str());
  aTree = (TTree*) aFile->Get("sim");
}

EVT* CorFlatG4Reader::GenerateCorsikaEvent() {
  // EVT* aShower;

  branch_PDGID  = new std::vector<int>(0);
	branch_Px     = new std::vector<double>(0);
	branch_Py     = new std::vector<double>(0);
	branch_Pz     = new std::vector<double>(0);
	branch_E      = new std::vector<double>(0);
	branch_x      = new std::vector<double>(0);
	branch_y      = new std::vector<double>(0);
	branch_Time   = new std::vector<double>(0);
	branch_Weight = new std::vector<double>(0);

  branch_PDGID->clear();
	branch_Px->clear();
	branch_Py->clear();
	branch_Pz->clear();
	branch_E->clear();
	branch_x->clear();
	branch_y->clear();
	branch_Time->clear();
	branch_Weight->clear();

  aTree->SetBranchAddress("PDGID", &branch_PDGID);
  aTree->SetBranchAddress("Px", &branch_Px);
  aTree->SetBranchAddress("Py", &branch_Py);
  aTree->SetBranchAddress("Pz", &branch_Pz);
  aTree->SetBranchAddress("E", &branch_E);
  aTree->SetBranchAddress("x", &branch_x);
  aTree->SetBranchAddress("y", &branch_y);
  aTree->SetBranchAddress("Time", &branch_Time);
  aTree->SetBranchAddress("Weight", &branch_Weight);

  if (n_event_count > aTree->GetEntries()) return 0;
  else {
    aTree->GetEntry(n_event_count);
    n_event_count++;
  }

  EVT* aShower = new EVT(branch_PDGID, branch_Px, branch_Py, branch_Pz, branch_E, branch_x, branch_y, branch_Time, branch_Weight);

  // aShower = std::make_tuple(branch_PDGID, branch_Px, branch_Py, branch_Pz, branch_E, branch_x, branch_y, branch_Time, branch_Weight);
 
  // if( verbose>0 ) HepMC3::Print::listing(*evt);;
  // if (verbose > 0) {
  //   for(int idx = 0; idx < branch_PDGID->size(); idx++) {
  //     std::cout << "Particle # : " << idx << std::endl;
  //     std::cout << "PDGID : " << branch_PDGID->at(idx) << std::endl;
  //     std::cout << "Px : " << branch_Px->at(idx) << " Py : "
  //   }
  // }

  return aShower;
}

void CorFlatG4Reader::DefineCommands() {
  fMessenger = new G4GenericMessenger(this, "/DRsim/Corsika/", "Corsika IO control");

  G4GenericMessenger::Command& verboseCmd = fMessenger->DeclareMethod("verbose",&CorFlatG4Reader::SetVerboseLevel,"verbose level");
  verboseCmd.SetParameterName("verbose",true);
  verboseCmd.SetDefaultValue("1");
}


