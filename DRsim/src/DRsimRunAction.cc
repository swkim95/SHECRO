#include "DRsimRunAction.hh"
#include "DRsimEventAction.hh"
// #include "G4GenericMessenger.hh"
#include "G4AutoLock.hh"
#include "G4Threading.hh"

#include <vector>

using namespace std;

namespace { G4Mutex DRsimRunActionMutex = G4MUTEX_INITIALIZER; }
HepMCG4Reader* DRsimRunAction::sHepMCreader = 0;
CorFlatG4Reader* DRsimRunAction::sCorFlatG4Reader = 0;
RootInterface<DRsimInterface::DRsimEventData>* DRsimRunAction::sRootIO = 0;
int DRsimRunAction::sNumEvt = 0;

DRsimRunAction::DRsimRunAction(G4int seed, G4String filename, G4bool useHepMC, G4bool useCorsika)
: G4UserRunAction()
{
  fSeed = seed;
  fFilename = filename;
  fUseHepMC = useHepMC;
  fUseCorsika = useCorsika;

  G4AutoLock lock(&DRsimRunActionMutex);

  if (!sRootIO) {
    sRootIO = new RootInterface<DRsimInterface::DRsimEventData>(fFilename+"_"+std::to_string(fSeed)+".root", true);
    sRootIO->create("DRsim","DRsimEventData");
  }

  if (fUseHepMC && !sHepMCreader) {
    sHepMCreader = new HepMCG4Reader(fSeed,fFilename);
  }

  if (fUseCorsika && !sCorFlatG4Reader) {
    sCorFlatG4Reader = new CorFlatG4Reader(fSeed,fFilename);
  }

  // DefineCommands();
}

DRsimRunAction::~DRsimRunAction() {
  if (IsMaster()) {
    G4AutoLock lock(&DRsimRunActionMutex);

    if (fUseHepMC && sHepMCreader) {
      delete sHepMCreader;
      sHepMCreader = 0;
    }

    if (fUseCorsika && sCorFlatG4Reader) {
      delete sCorFlatG4Reader;
      sCorFlatG4Reader = 0;
    }

    if (sRootIO) {
      sRootIO->write();
      sRootIO->close();
      delete sRootIO;
      sRootIO = 0;
    }

    // if (fMessenger) delete fMessenger;
  }
}

void DRsimRunAction::BeginOfRunAction(const G4Run*) {

}

void DRsimRunAction::EndOfRunAction(const G4Run*) {

}

// void DRsimRunAction::DefineCommands() {
//   fMessenger = new G4GenericMessenger(this, "/DRsim/file/", "ROOT file reading");
//   G4GenericMessenger::Command& nameCmd = fMessenger->DeclareProperty("fileName",fFilename,"file name");
//   nameCmd.SetParameterName("fileName",true);
//   nameCmd.SetDefaultValue("Flat_Corsika");

//   G4GenericMessenger::Command& seedCmd = fMessenger->DeclareProperty("seed",fSeed,"set seed");
//   seedCmd.SetParameterName("seed",true);
//   seedCmd.SetDefaultValue("0");
// }