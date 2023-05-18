#include <iostream>
#include "DRsimDetectorConstruction.hh"
#include "DRsimActionInitialization.hh"

#ifdef G4MULTITHREADED
#include "G4MTRunManager.hh"
#else
#include "G4RunManager.hh"
#endif

#include "G4UImanager.hh"
#include "G4OpticalPhysics.hh"
#include "FTFP_BERT.hh"
#include "Randomize.hh"

// #ifdef G4VIS_USE
#include "G4VisExecutive.hh"
// #endif

// #ifdef G4UI_USE
#include "G4UIExecutive.hh"
// #endif

// To visualize : ./bin/DRsim
// To visualize with CORSIKA : ./bin/DRsim init_vis_CORSIKA <seed> Flat_CORSIKA

int main(int argc, char** argv) {
  // #ifdef G4UI_USE
    // Detect interactive mode (if no arguments) and define UI session
    G4UIExecutive* ui = 0;
    // if ( argc == 1 ) ui = new G4UIExecutive(argc, argv);
    ui = new G4UIExecutive(argc, argv);
  // #endif

  G4int seed = 0;
  G4String filename;
  if (argc > 2) seed = atoi(argv[2]);
  if (argc > 3) filename = argv[3];

  CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine);
  CLHEP::HepRandom::setTheSeed(seed);

  // Construct the default run manager
  #ifdef G4MULTITHREADED
    G4MTRunManager* runManager = new G4MTRunManager;
  #else
    G4RunManager* runManager = new G4RunManager;
  #endif

  // Mandatory user initialization classes
  runManager->SetUserInitialization(new DRsimDetectorConstruction());

  // physics module
  G4VModularPhysicsList* physicsList = new FTFP_BERT;
  G4OpticalPhysics* opticalPhysics = new G4OpticalPhysics();
  physicsList->RegisterPhysics(opticalPhysics);
  G4OpticalParameters* opticalParameters = G4OpticalParameters::Instance();
  opticalParameters->SetProcessActivation("Cerenkov", true);
  opticalParameters->SetProcessActivation("Scintillation", true);
  opticalParameters->SetCerenkovTrackSecondariesFirst(true);
  opticalParameters->SetScintTrackSecondariesFirst(true);
  // opticalPhysics->Configure(kCerenkov, true);
  // opticalPhysics->Configure(kScintillation, true);
  // opticalPhysics->SetTrackSecondariesFirst(kCerenkov, true);
  // opticalPhysics->SetTrackSecondariesFirst(kScintillation, true);
  runManager->SetUserInitialization(physicsList);

  // User action initialization
  runManager->SetUserInitialization(new DRsimActionInitialization(seed,filename));

  // Visualization manager construction
  // #ifdef G4VIS_USE
    G4VisManager* visManager = new G4VisExecutive;
    visManager->Initialize();
  // #endif
  G4UImanager* UImanager = G4UImanager::GetUIpointer();

  if ( argc != 1 ) {
    // execute an argument macro file if exist
    G4String command = "/control/execute ";
    G4String macroName = argv[1];
    UImanager->ApplyCommand(command+macroName);
  } else {
    #ifdef G4UI_USE
      #ifdef G4VIS_USE
        UImanager->ApplyCommand("/control/execute init_vis.mac");
      #else
        UImanager->ApplyCommand("/control/execute init.mac");
      #endif
      if (ui->IsGUI()) { UImanager->ApplyCommand("/control/execute gui.mac"); }
      // start interactive session
      ui->SessionStart();
      delete ui;
    #endif
  }

  if (argc == 1) {
    UImanager->ApplyCommand("/control/execute init_vis.mac");
    if (ui->IsGUI()) { UImanager->ApplyCommand("/control/execute gui.mac"); }
    ui->SessionStart();
    delete ui;
  }
  else {
    G4String command = "/control/execute ";
    G4String macroName = argv[1];
    UImanager->ApplyCommand(command+macroName);
    if (ui->IsGUI()) { UImanager->ApplyCommand("/control/execute gui.mac"); }
    ui->SessionStart();
    delete ui;
  }
  // #ifdef G4UI_USE
  //   #ifdef G4VIS_USE
  //     UImanager->ApplyCommand("/control/execute init_vis.mac");
  //   #else
  //     UImanager->ApplyCommand("/control/execute init.mac");
  //   #endif
  //   if (ui->IsGUI()) { UImanager->ApplyCommand("/control/execute gui.mac"); }
  //   // start interactive session
  //   ui->SessionStart();
  //   delete ui;
  // #endif

  // Job termination
  // Free the store: user actions, physics_list and detector_description are
  // owned and deleted by the run manager, so they should not be deleted
  // in the main() program !

  #ifdef G4VIS_USE
    delete visManager;
  #endif
  delete runManager;

  return 0;
}
