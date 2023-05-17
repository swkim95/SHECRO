#ifndef DRsimRunAction_h
#define DRsimRunAction_h 1

#include "RootInterface.h"
#include "DRsimInterface.h"
#include "HepMCG4Reader.hh"
#include "CorFlatG4Reader.hh"

#include "G4UserRunAction.hh"
#include "globals.hh"

class G4Run;

class DRsimRunAction : public G4UserRunAction {
public:
  // DRsimRunAction(G4int seed, G4String filename, G4bool useHepMC);
  DRsimRunAction(G4int seed, G4String filename, G4bool useHepMC, G4bool useCorsika);
  virtual ~DRsimRunAction();

  virtual void BeginOfRunAction(const G4Run*);
  virtual void EndOfRunAction(const G4Run*);

  static HepMCG4Reader* sHepMCreader;
  static CorFlatG4Reader* sCorFlatG4Reader;
  static RootInterface<DRsimInterface::DRsimEventData>* sRootIO;
  static int sNumEvt;

private:
  G4int fSeed;
  G4String fFilename;
  G4bool fUseHepMC;
  G4bool fUseCorsika;
  // G4GenericMessenger* fMessenger;

  void DefineCommands();
};

#endif
