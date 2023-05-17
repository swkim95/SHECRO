#ifndef COR_G4_READER_h
#define COR_G4_READER_h 1

#include "CorFlatG4Interface.hh"
// #include <crsIO/TRun.h>
// #include <crsIO/TShower.h>
// #include <crsIO/TParticle.h>
// #include <crsIO/TCherenkov.h>
// #include <crsIO/TLongitudinal.h>

class G4GenericMessenger;

class CorFlatG4Reader : public CorFlatG4Interface {
protected:
  TFile* aFile;
  TTree* aTree;
  G4int verbose;

  std::vector<int>* branch_PDGID;
	std::vector<double>* branch_Px;
	std::vector<double>* branch_Py;
	std::vector<double>* branch_Pz;
	std::vector<double>*  branch_E;
	std::vector<double>*  branch_x;
	std::vector<double>*  branch_y;
	std::vector<double>* branch_Time;
	std::vector<double>* branch_Weight;

  virtual EVT* GenerateCorsikaEvent();

public:
  CorFlatG4Reader(G4int seed, G4String file_path);
  ~CorFlatG4Reader();

  void SetVerboseLevel(G4int i) { verbose = i; }
  G4int GetVerboseLevel() const { return verbose; }

  void Initialize();

private:
  void DefineCommands();

  int n_event_count;

  G4GenericMessenger* fMessenger;
  G4int fSeed;
  G4String fCorsikaPath;
};
#endif
