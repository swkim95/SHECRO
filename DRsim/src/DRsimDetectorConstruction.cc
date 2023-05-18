#include "DRsimDetectorConstruction.hh"
#include "DRsimCellParameterisation.hh"
#include "DRsimFilterParameterisation.hh"
#include "DRsimMirrorParameterisation.hh"
#include "DRsimSiPMSD.hh"

#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVParameterised.hh"
#include "G4RotationMatrix.hh"

#include "G4IntersectionSolid.hh"
#include "G4SDManager.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4GeometryManager.hh"

#include "G4Colour.hh"
#include "G4SystemOfUnits.hh"

#include "Randomize.hh"

using namespace std;

G4ThreadLocal DRsimMagneticField* DRsimDetectorConstruction::fMagneticField = 0;
G4ThreadLocal G4FieldManager* DRsimDetectorConstruction::fFieldMgr = 0;

int DRsimDetectorConstruction::fNofRow = 1;
int DRsimDetectorConstruction::fNofModules = fNofRow * fNofRow;

DRsimDetectorConstruction::DRsimDetectorConstruction()
: G4VUserDetectorConstruction(), fMessenger(0), fMaterials(NULL) {
  DefineCommands();
  DefineMaterials();

  clad_C_rMin = 0.49*mm;
  clad_C_rMax = 0.50*mm;
  clad_C_Dz   = 2.5*m;
  clad_C_Sphi = 0.;
  clad_C_Dphi = 2.*M_PI;

  core_C_rMin = 0.*mm;
  core_C_rMax = 0.49*mm;
  core_C_Dz   = 2.5*m;
  core_C_Sphi = 0.;
  core_C_Dphi = 2.*M_PI;

  clad_S_rMin = 0.485*mm;
  clad_S_rMax = 0.50*mm;
  clad_S_Dz   = 2.5*m;
  clad_S_Sphi = 0.;
  clad_S_Dphi = 2.*M_PI;

  core_S_rMin = 0.*mm;
  core_S_rMax = 0.485*mm;
  core_S_Dz   = 2.5*m;
  core_S_Sphi = 0.;
  core_S_Dphi = 2.*M_PI;

  PMTT = 0.3*mm;
  filterT = 0.01*mm;
  reflectorT = 0.03*mm;

  fVisAttrOrange = new G4VisAttributes(G4Colour(1.0,0.5,0.,1.0));
  fVisAttrOrange->SetVisibility(true);
  fVisAttrBlue = new G4VisAttributes(G4Colour(0.,0.,1.0,1.0));
  fVisAttrBlue->SetVisibility(true);
  fVisAttrGray = new G4VisAttributes(G4Colour(0.3,0.3,0.3,0.3));
  fVisAttrGray->SetVisibility(true);
  fVisAttrGreen = new G4VisAttributes(G4Colour(0.3,0.7,0.3));
  fVisAttrGreen->SetVisibility(true);
  fVisAttrRed = new G4VisAttributes(G4Colour(1.0, 0.0, 0.0));
  fVisAttrRed->SetVisibility(true);
  fVisAttrYellow = new G4VisAttributes(G4Colour(1.0, 1.0, 0.0));
  fVisAttrYellow->SetVisibility(true);
  fVisAttrMagenta = new G4VisAttributes(G4Colour(1.0, 0.0, 1.0));
  fVisAttrMagenta->SetVisibility(true);
}

DRsimDetectorConstruction::~DRsimDetectorConstruction() {
  delete fMessenger;
  delete fMaterials;

  delete fVisAttrOrange;
  delete fVisAttrBlue;
  delete fVisAttrGray;
  delete fVisAttrGreen;
  delete fVisAttrRed;
  delete fVisAttrYellow;
  delete fVisAttrMagenta;
}

void DRsimDetectorConstruction::DefineMaterials() {
  fMaterials = DRsimMaterials::GetInstance();
}

G4VPhysicalVolume* DRsimDetectorConstruction::Construct() {
  G4GeometryManager::GetInstance()->OpenGeometry();
  G4PhysicalVolumeStore::GetInstance()->Clean();
  G4LogicalVolumeStore::GetInstance()->Clean();
  G4SolidStore::GetInstance()->Clean();

  checkOverlaps = false;

  G4VSolid* worldSolid             = new G4Box("worldBox",10.*m,10.*m,10.*m);
  worldLogical                     = new G4LogicalVolume(worldSolid,FindMaterial("G4_Galactic"),"worldLogical");
  G4VPhysicalVolume* worldPhysical = new G4PVPlacement(0,G4ThreeVector(),worldLogical,"worldPhysical",0,false,0,checkOverlaps);

  float moduleUnitDimension = 2250.;

  fFrontL     = 0.;
  fTowerDepth = 1000.; 
  fModuleH    = moduleUnitDimension;
  fModuleW    = moduleUnitDimension;
  fFiberUnitH = 1.;
  fFiberUnitW = 1.;
  fSiPMUnit   = 1.;

  UnitMultiplier(50);

  fSiPMUnitDim = fSiPMUnit + 0.2;
  fCellUnit    = fFiberUnitH * 1.5;

  // fRandomSeed = 1;

  doFiber     = true;
  doReflector = true;
  doPMT       = true;

  // fiberUnit   = new G4Box("fiber_SQ", (fFiberUnitH/2.) *mm, (fFiberUnitW/2.) *mm, (fTowerDepth/2.) *mm);
  // fiberClad   = new G4Tubs("fiber",  0, clad_C_rMax, fTowerDepth/2., 0 *deg, 360. *deg);   // S is the same
  // fiberCoreC  = new G4Tubs("fiberC", 0, core_C_rMax, fTowerDepth/2., 0 *deg, 360. *deg);
  // fiberCoreS  = new G4Tubs("fiberS", 0, core_S_rMax, fTowerDepth/2., 0 *deg, 360. *deg);

  fiberUnit   = new G4Box("fiber_SQ", (moduleUnitDimension/2.) *mm, (fFiberUnitW/2.) *mm, (fFiberUnitH/2.) *mm);
  fiberClad   = new G4Tubs("fiber",  0, clad_C_rMax, moduleUnitDimension/2., 0. *deg, 360. *deg);   // S is the same
  fiberCoreC  = new G4Tubs("fiberC", 0, core_C_rMax, moduleUnitDimension/2., 0. *deg, 360. *deg);
  fiberCoreS  = new G4Tubs("fiberS", 0, core_S_rMax, moduleUnitDimension/2., 0. *deg, 360. *deg);

  dimCalc = new dimensionCalc();
  dimCalc->SetFrontL(fFrontL);
  dimCalc->SetTower_height(fTowerDepth);
  dimCalc->SetPMTT(PMTT+filterT);
  dimCalc->SetReflectorT(reflectorT);
  dimCalc->SetNofModules(fNofModules);
  dimCalc->SetNofRow(fNofRow);
  dimCalc->SetModuleHeight(fModuleH);
  dimCalc->SetModuleWidth(fModuleW);

  ModuleBuild(ModuleLogical,PMTGLogical,PMTfilterLogical,PMTcellLogical,PMTcathLogical,ReflectorMirrorLogical,fiberUnitIntersection,fiberCladIntersection,fiberCoreIntersection,fModuleProp);

  delete dimCalc;
  return worldPhysical;
}

void DRsimDetectorConstruction::ConstructSDandField() {
  G4SDManager* SDman = G4SDManager::GetSDMpointer();
  G4String SiPMName = "SiPMSD";

  // ! Not a memory leak - SDs are deleted by G4SDManager. Deleting them manually will cause double delete!
  if ( doPMT ) {
    for (int i = 0; i < fNofModules; i++) {
      DRsimSiPMSD* SiPMSDmodule = new DRsimSiPMSD("Module"+std::to_string(i), "ModuleC"+std::to_string(i), fModuleProp.at(i));
      SDman->AddNewDetector(SiPMSDmodule);
      PMTcathLogical[i]->SetSensitiveDetector(SiPMSDmodule);
    }
  }
}

void DRsimDetectorConstruction::ModuleBuild(G4LogicalVolume* ModuleLogical_[], 
                                            G4LogicalVolume* PMTGLogical_[], G4LogicalVolume* PMTfilterLogical_[], G4LogicalVolume* PMTcellLogical_[], G4LogicalVolume* PMTcathLogical_[], 
                                            G4LogicalVolume* ReflectorMirrorLogical_[],
                                            std::vector<G4LogicalVolume*> fiberUnitIntersection_[], std::vector<G4LogicalVolume*> fiberCladIntersection_[], std::vector<G4LogicalVolume*> fiberCoreIntersection_[], 
                                            std::vector<DRsimInterface::DRsimModuleProperty>& ModuleProp_) {
  
  G4RotationMatrix* rotationMatrix = new G4RotationMatrix();
  rotationMatrix->rotateY(90. *deg);

  for (int i = 0; i < fNofModules; i++) {    
    moduleName = setModuleName(i);
    
    dimCalc->SetisModule(true);
    module = new G4Box("Module", (fModuleH/2.) *mm, (fModuleW/2.) *mm, (fTowerDepth/2.) *mm );
    ModuleLogical_[i] = new G4LogicalVolume(module,FindMaterial("Copper"),moduleName);
    // G4VPhysicalVolume* modulePhysical = new G4PVPlacement(0,dimCalc->GetOrigin(i),ModuleLogical_[i],moduleName,worldLogical,false,0,checkOverlaps);
    new G4PVPlacement(0,dimCalc->GetOrigin(i),ModuleLogical_[i],moduleName,worldLogical,false,0,checkOverlaps);

    if ( doPMT ) {
      dimCalc->SetisModule(false);  
      // pmtg = new G4Box("PMTG", (fModuleH/2.) *mm, (fModuleW/2.) *mm, (PMTT+filterT)/2. *mm );
      pmtg = new G4Box("PMTG", (PMTT+filterT)/2. *mm, (fModuleW/2.) *mm, (fTowerDepth/2.) *mm );
      PMTGLogical_[i]  = new G4LogicalVolume(pmtg,FindMaterial("G4_AIR"),moduleName);
      new G4PVPlacement(0,dimCalc->GetOrigin_PMTG(i),PMTGLogical_[i],moduleName,worldLogical,false,0,checkOverlaps);
    }

    FiberImplement(i,ModuleLogical_,fiberUnitIntersection_,fiberCladIntersection_,fiberCoreIntersection_);

    DRsimInterface::DRsimModuleProperty ModulePropSingle;
    ModulePropSingle.towerXY   = fTowerXY;
    ModulePropSingle.ModuleNum = i;
    ModuleProp_.push_back(ModulePropSingle);

    if ( doPMT ) {
      // G4VSolid* SiPMlayerSolid = new G4Box("SiPMlayerSolid", (fModuleH/2.) *mm, (fModuleW/2.) *mm, (PMTT/2.) *mm );
      G4VSolid* SiPMlayerSolid = new G4Box("SiPMlayerSolid", (PMTT/2.) *mm, (fModuleW/2.) *mm, (fTowerDepth/2.) *mm );
      G4LogicalVolume* SiPMlayerLogical = new G4LogicalVolume(SiPMlayerSolid,FindMaterial("G4_AIR"),"SiPMlayerLogical");
      // new G4PVPlacement(0,G4ThreeVector(0.,0.,filterT/2.),SiPMlayerLogical,"SiPMlayerPhysical",PMTGLogical_[i],false,0,checkOverlaps);
      // new G4PVPlacement(0,G4ThreeVector(-(fModuleH/2.) - (filterT/2.), 0., fTowerDepth/2.),SiPMlayerLogical,"SiPMlayerPhysical",PMTGLogical_[i],false,0,checkOverlaps);
      new G4PVPlacement(0,G4ThreeVector(filterT/2., 0, 0),SiPMlayerLogical,"SiPMlayerPhysical",PMTGLogical_[i],false,0,checkOverlaps);

      // G4VSolid* filterlayerSolid = new G4Box("filterlayerSolid", (fModuleH/2.) *mm, (fModuleW/2.) *mm, (filterT/2.) *mm );
      G4VSolid* filterlayerSolid = new G4Box("filterlayerSolid", (filterT/2.) *mm, (fModuleW/2.) *mm, (fTowerDepth/2.) *mm );
      G4LogicalVolume* filterlayerLogical = new G4LogicalVolume(filterlayerSolid,FindMaterial("Glass"),"filterlayerLogical");
      // new G4PVPlacement(0,G4ThreeVector(0.,0.,-PMTT/2.),filterlayerLogical,"filterlayerPhysical",PMTGLogical_[i],false,0,checkOverlaps);
      // new G4PVPlacement(0,G4ThreeVector(-(fModuleH)/2. -PMTT/2. ,0. , fTowerDepth/2.),filterlayerLogical,"filterlayerPhysical",PMTGLogical_[i],false,0,checkOverlaps);
      new G4PVPlacement(0,G4ThreeVector(-PMTT/2., 0, 0),filterlayerLogical,"filterlayerPhysical",PMTGLogical_[i],false,0,checkOverlaps);

      // G4VSolid* PMTcellSolid = new G4Box("PMTcellSolid", fSiPMUnit/2. *mm, fSiPMUnit/2. *mm, PMTT/2. *mm );
      G4VSolid* PMTcellSolid = new G4Box("PMTcellSolid", PMTT/2. *mm, fSiPMUnit/2. *mm, fSiPMUnit/2. *mm );
      PMTcellLogical_[i] = new G4LogicalVolume(PMTcellSolid,FindMaterial("Glass"),"PMTcellLogical_");

      // DRsimCellParameterisation* PMTcellParam = new DRsimCellParameterisation(fTowerXY.first,fTowerXY.second);
      DRsimCellParameterisation* PMTcellParam = new DRsimCellParameterisation(fFiberX, fFiberY, fFiberWhich);
      G4PVParameterised* PMTcellPhysical = new G4PVParameterised("PMTcellPhysical",PMTcellLogical_[i],SiPMlayerLogical,kXAxis,fTowerXY.first*fTowerXY.second,PMTcellParam);

      // G4VSolid* PMTcathSolid = new G4Box("PMTcathSolid", fSiPMUnit/2. *mm, fSiPMUnit/2. *mm, filterT/2. *mm );
      G4VSolid* PMTcathSolid = new G4Box("PMTcathSolid", filterT/2. *mm, fSiPMUnit/2. *mm, fSiPMUnit/2. *mm );
      PMTcathLogical_[i] = new G4LogicalVolume(PMTcathSolid,FindMaterial("Silicon"),"PMTcathLogical_");
      // new G4PVPlacement(0,G4ThreeVector(0.,0.,(PMTT-filterT)/2.*mm),PMTcathLogical_[i],"PMTcathPhysical",PMTcellLogical_[i],false,0,checkOverlaps);
      new G4PVPlacement(0,G4ThreeVector((PMTT-filterT)/2.*mm, 0.,0 *mm),PMTcathLogical_[i],"PMTcathPhysical",PMTcellLogical_[i],false,0,checkOverlaps);
      new G4LogicalSkinSurface("Photocath_surf",PMTcathLogical_[i],FindSurface("SiPMSurf"));

      // G4VSolid* filterSolid = new G4Box("filterSolid", fSiPMUnit/2. *mm, fSiPMUnit/2. *mm, filterT/2. *mm );
      G4VSolid* filterSolid = new G4Box("filterSolid", filterT/2. *mm, fSiPMUnit/2. *mm, fSiPMUnit/2. *mm );
      PMTfilterLogical_[i] = new G4LogicalVolume(filterSolid,FindMaterial("Gelatin"),"PMTfilterLogical_");

      int filterNo = (int)(fTowerXY.first * fTowerXY.second) / 2;
      // if ( fTowerXY.first % 2 == 1)
      if ( fTowerXY.first % 2 == 0)
        filterNo++;

      // DRsimFilterParameterisation* filterParam = new DRsimFilterParameterisation(fTowerXY.first,fTowerXY.second);
      DRsimFilterParameterisation* filterParam = new DRsimFilterParameterisation(fFiberX, fFiberY, fFiberWhich);
      G4PVParameterised* filterPhysical = new G4PVParameterised("filterPhysical",PMTfilterLogical_[i],filterlayerLogical,kXAxis,filterNo,filterParam);
      new G4LogicalBorderSurface("filterSurf",filterPhysical,PMTcellPhysical,FindSurface("FilterSurf"));
          
      PMTcathLogical_[i]->SetVisAttributes(fVisAttrGreen);
      PMTfilterLogical_[i]->SetVisAttributes(fVisAttrMagenta);
    }

    if ( doReflector ) {
      // G4VSolid* ReflectorlayerSolid = new G4Box("ReflectorlayerSolid", (fModuleH/2.) *mm, (fModuleW/2.) *mm, (reflectorT/2.) *mm );
      G4VSolid* ReflectorlayerSolid = new G4Box("ReflectorlayerSolid", (reflectorT/2.) *mm, (fModuleW/2.) *mm, (fTowerDepth/2.) *mm );
      G4LogicalVolume* ReflectorlayerLogical = new G4LogicalVolume(ReflectorlayerSolid,FindMaterial("G4_Galactic"),"ReflectorlayerLogical");
      new G4PVPlacement(0,dimCalc->GetOrigin_Reflector(i),ReflectorlayerLogical,"ReflectorlayerPhysical",worldLogical,false,0,checkOverlaps);

      // G4VSolid* mirrorSolid = new G4Box("mirrorSolid", fSiPMUnit/2. *mm, fSiPMUnit/2. *mm, reflectorT/2. *mm );
      G4VSolid* mirrorSolid = new G4Box("mirrorSolid", reflectorT/2. *mm, fSiPMUnit/2. *mm, fSiPMUnit/2. *mm );
      ReflectorMirrorLogical_[i] = new G4LogicalVolume(mirrorSolid,FindMaterial("Aluminum"),"ReflectorMirrorLogical_");

      // DRsimMirrorParameterisation* mirrorParam = new DRsimMirrorParameterisation(fTowerXY.first,fTowerXY.second);
      DRsimMirrorParameterisation* mirrorParam = new DRsimMirrorParameterisation(fFiberX, fFiberY, fFiberWhich);
      G4PVParameterised* mirrorPhysical = new G4PVParameterised("mirrorPhysical",ReflectorMirrorLogical_[i],ReflectorlayerLogical,kXAxis,fTowerXY.first*fTowerXY.second/2,mirrorParam);
      // new G4LogicalBorderSurface("MirrorSurf",mirrorPhysical,modulePhysical,FindSurface("MirrorSurf"));
      new G4LogicalSkinSurface("MirrorSurf",ReflectorMirrorLogical_[i],FindSurface("MirrorSurf"));

      ReflectorMirrorLogical_[i]->SetVisAttributes(fVisAttrYellow);
    }
  }
}

void DRsimDetectorConstruction::DefineCommands() {}

void DRsimDetectorConstruction::FiberImplement(G4int i, G4LogicalVolume* ModuleLogical__[], 
                                              std::vector<G4LogicalVolume*> fiberUnitIntersection__[], std::vector<G4LogicalVolume*> fiberCladIntersection__[], 
                                              std::vector<G4LogicalVolume*> fiberCoreIntersection__[]) {

  fFiberX.clear();
  fFiberY.clear();
  fFiberWhich.clear();

  int NofFiber = (int)(fModuleW / fCellUnit);   
  // int NofPlate = (int)(fModuleH / fCellUnit);
  int NofPlate = (int)(fTowerDepth / fCellUnit);
  fBottomEdge = fmod(fModuleW, fCellUnit) / 2.;
  // fLeftEdge = fmod(fModuleH, fCellUnit) / 2.;
  fLeftEdge = fmod(fTowerDepth, fCellUnit) / 2.;

  double randDeviation = 0.; //  double randDeviation = fFiberUnitH - 1.;
  fTowerXY = std::make_pair(NofPlate,NofFiber);
  
  G4bool fWhich = false;  
  for (int k = 0; k < NofPlate; k++) {
    for (int j = 0; j < NofFiber; j++) { 
      /*
        ? fX : # of plate , fY : # of fiber in the plate
      */
      // G4float fX = -fModuleH *mm / 2. + k * fCellUnit *mm + fCellUnit / 2. *mm + fBottomEdge *mm;
      // G4float fY = -fModuleW *mm / 2. + j * fCellUnit *mm + fCellUnit / 2. *mm + fLeftEdge *mm;
      G4float fX = -fTowerDepth *mm / 2. + k * fCellUnit *mm + fCellUnit / 2. *mm + fBottomEdge *mm; // this is fZ actually
      G4float fY = -fModuleW *mm / 2. + j * fCellUnit *mm + fCellUnit / 2. *mm + fLeftEdge *mm;
      fWhich = !fWhich;
      fFiberX.push_back(fX);
      fFiberY.push_back(fY);
      fFiberWhich.push_back(fWhich);
    }
    if ( NofFiber%2==0 ) { fWhich = !fWhich; }   
  }
  
  if ( doFiber ) {
    G4RotationMatrix* rotationMatrix = new G4RotationMatrix();
    rotationMatrix->rotateY(90. *deg);
    for (unsigned int j = 0; j<fFiberX.size(); j++) {

      if ( !fFiberWhich.at(j) ) { //c fibre
        // tfiberCladIntersection = new G4IntersectionSolid("fiberClad",fiberClad,module,0,G4ThreeVector(-fFiberX.at(j),-fFiberY.at(j),0.));
        tfiberCladIntersection = new G4IntersectionSolid("fiberClad",fiberClad,module,rotationMatrix,G4ThreeVector(0, 0, 0));
        fiberCladIntersection__[i].push_back(new G4LogicalVolume(tfiberCladIntersection,FindMaterial("FluorinatedPolymer"),name));
        // new G4PVPlacement(0,G4ThreeVector(fFiberX.at(j),fFiberY.at(j),0),fiberCladIntersection__[i].at(j),name,ModuleLogical__[i],false,j,checkOverlaps);
        new G4PVPlacement(rotationMatrix,G4ThreeVector(0, fFiberY.at(j), fFiberX.at(j)),fiberCladIntersection__[i].at(j),name,ModuleLogical__[i],false,j,checkOverlaps);

        // tfiberCoreIntersection = new G4IntersectionSolid("fiberCore",fiberCoreC,module,0,G4ThreeVector(-fFiberX.at(j),-fFiberY.at(j),0.));
        tfiberCoreIntersection = new G4IntersectionSolid("fiberCore",fiberCoreC,module,rotationMatrix,G4ThreeVector(0, 0, 0));
        fiberCoreIntersection__[i].push_back(new G4LogicalVolume(tfiberCoreIntersection,FindMaterial("PMMA"),name));
        // new G4PVPlacement(0,G4ThreeVector(0.,0.,0.),fiberCoreIntersection__[i].at(j),name,fiberCladIntersection__[i].at(j),false,j,checkOverlaps);
        new G4PVPlacement(0,G4ThreeVector(0.,0.,0.),fiberCoreIntersection__[i].at(j),name,fiberCladIntersection__[i].at(j),false,j,checkOverlaps);

        fiberCladIntersection__[i].at(j)->SetVisAttributes(fVisAttrGray);
        fiberCoreIntersection__[i].at(j)->SetVisAttributes(fVisAttrBlue);
      } else { // s fibre

        // tfiberCladIntersection = new G4IntersectionSolid("fiberClad",fiberClad,module,0,G4ThreeVector(-fFiberX.at(j),-fFiberY.at(j),0.));
        tfiberCladIntersection = new G4IntersectionSolid("fiberClad",fiberClad,module,rotationMatrix,G4ThreeVector(0, 0, 0));
        fiberCladIntersection__[i].push_back(new G4LogicalVolume(tfiberCladIntersection,FindMaterial("PMMA"),name));
        // new G4PVPlacement(0,G4ThreeVector(fFiberX.at(j),fFiberY.at(j),0),fiberCladIntersection__[i].at(j),name,ModuleLogical__[i],false,j,checkOverlaps);
        new G4PVPlacement(rotationMatrix,G4ThreeVector(0,fFiberY.at(j),fFiberX.at(j)),fiberCladIntersection__[i].at(j),name,ModuleLogical__[i],false,j,checkOverlaps);

        // tfiberCoreIntersection = new G4IntersectionSolid("fiberCore",fiberCoreS,module,0,G4ThreeVector(-fFiberX.at(j),-fFiberY.at(j),0.));
        tfiberCoreIntersection = new G4IntersectionSolid("fiberCore",fiberCoreS,module,rotationMatrix,G4ThreeVector(0, 0, 0));
        fiberCoreIntersection__[i].push_back(new G4LogicalVolume(tfiberCoreIntersection,FindMaterial("Polystyrene"),name));
        // new G4PVPlacement(0,G4ThreeVector(0.,0.,0.),fiberCoreIntersection__[i].at(j),name,fiberCladIntersection__[i].at(j),false,j,checkOverlaps);
        new G4PVPlacement(0,G4ThreeVector(0.,0.,0.),fiberCoreIntersection__[i].at(j),name,fiberCladIntersection__[i].at(j),false,j,checkOverlaps);

        fiberCladIntersection__[i].at(j)->SetVisAttributes(fVisAttrGray);
        fiberCoreIntersection__[i].at(j)->SetVisAttributes(fVisAttrOrange);
      }
    }
  }
}


