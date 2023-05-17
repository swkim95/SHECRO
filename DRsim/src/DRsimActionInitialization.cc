#include "DRsimActionInitialization.hh"
#include "DRsimPrimaryGeneratorAction.hh"
#include "DRsimRunAction.hh"
#include "DRsimEventAction.hh"
#include "DRsimSteppingAction.hh"

#include "G4GenericMessenger.hh"

using namespace std;
DRsimActionInitialization::DRsimActionInitialization(G4int seed, G4String filename)
: G4VUserActionInitialization()
{
  fSeed = seed;
  fFilename = filename;

  DefineCommands();
}

DRsimActionInitialization::~DRsimActionInitialization() {
  if (fMessenger) delete fMessenger;
}

void DRsimActionInitialization::BuildForMaster() const {
  SetUserAction(new DRsimRunAction(fSeed,fFilename,fUseHepMC, fUseCorsika));
}

void DRsimActionInitialization::Build() const {
  SetUserAction(new DRsimPrimaryGeneratorAction(fSeed,fUseHepMC, fUseCorsika, fUseCalib,fUseGPS));
  SetUserAction(new DRsimRunAction(fSeed,fFilename,fUseHepMC, fUseCorsika));

  DRsimEventAction* eventAction = new DRsimEventAction();
  SetUserAction(eventAction);

  SetUserAction(new DRsimSteppingAction(eventAction));
}

void DRsimActionInitialization::DefineCommands() {
  fMessenger = new G4GenericMessenger(this, "/DRsim/action/", "action initialization control");
  G4GenericMessenger::Command& ioCmd = fMessenger->DeclareProperty("useHepMC",fUseHepMC,"use HepMC");
  ioCmd.SetParameterName("useHepMC",true);
  ioCmd.SetDefaultValue("False");

  G4GenericMessenger::Command& CorIOCmd = fMessenger->DeclareProperty("useCorsika",fUseCorsika,"use Corsika");
  CorIOCmd.SetParameterName("useCorsika",true);
  CorIOCmd.SetDefaultValue("False");

  G4GenericMessenger::Command& calibCmd = fMessenger->DeclareProperty("useCalib",fUseCalib,"use Calib");
  calibCmd.SetParameterName("useCalib",true);
  calibCmd.SetDefaultValue("False");

  G4GenericMessenger::Command& gpsCmd = fMessenger->DeclareProperty("useGPS",fUseGPS,"use GPS");
  gpsCmd.SetParameterName("useGPS",true);
  gpsCmd.SetDefaultValue("False");

  G4GenericMessenger::Command& nameCmd = fMessenger->DeclareProperty("fileName",fFilename,"file name");
  nameCmd.SetParameterName("fileName",true);
  nameCmd.SetDefaultValue("Flat_Corsika");

  G4GenericMessenger::Command& seedCmd = fMessenger->DeclareProperty("seed",fSeed,"set seed");
  seedCmd.SetParameterName("seed",true);
  seedCmd.SetDefaultValue("0");
}
