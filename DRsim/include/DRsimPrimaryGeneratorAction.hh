#ifndef DRsimPrimaryGeneratorAction_h
#define DRsimPrimaryGeneratorAction_h 1

#include "HepMCG4Reader.hh"
#include "CorFlatG4Reader.hh"

#include "globals.hh"
#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "G4ThreeVector.hh"
#include "G4Types.hh"
#include "G4GeneralParticleSource.hh"

class G4GenericMessenger;
class G4Event;
class G4ParticleDefinition;

class DRsimPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction {
public:
  // DRsimPrimaryGeneratorAction(G4int seed, G4bool useHepMC, G4bool useCalib, G4bool useGPS);
  DRsimPrimaryGeneratorAction(G4int seed, G4bool useHepMC, G4bool useCorsika, G4bool useCalib, G4bool useGPS);
  virtual ~DRsimPrimaryGeneratorAction();

  virtual void GeneratePrimaries(G4Event*);

  void SetTheta(G4double theta) { fTheta = theta; }
  G4double GetTheta() const { return fTheta; }

  void SetPhi(G4double phi) { fPhi = phi; }
  G4double GetPhi() const { return fPhi; }

  void SetX0(G4double x0) { fX_0 = x0; }
  void SetY0(G4double y0) { fY_0 = y0; }
  void SetZ0(G4double z0) { fZ_0 = z0; }

  void SetRandX(G4double randx) { fRandX = randx; }
  void SetRandY(G4double randy) { fRandY = randy; }
  void SetRandZ(G4double randz) { fRandZ = randz; }

  static G4ThreadLocal int sIdxEvt;
  static int sNumEvt;

private:
  void DefineCommands();
  void initPtcGun();
  void initGPS();

  G4int fSeed;
  G4bool fUseHepMC;
  G4bool fUseCalib;
  G4bool fUseCorsika;
  G4bool fUseGPS;
  G4ParticleGun* fParticleGun;
  G4GeneralParticleSource* fGPS;
  G4GenericMessenger* fMessenger;
  G4ParticleDefinition* fElectron;
  G4ParticleDefinition* fPositron;
  G4ParticleDefinition* fMuon;
  G4ParticleDefinition* fPion;
  G4ParticleDefinition* fKaon0L;
  G4ParticleDefinition* fProton;
  G4ParticleDefinition* fOptGamma;

  G4double fTheta;
  G4double fPhi;
  
  G4double fRandX;
  G4double fRandY;
  G4double fRandZ;

  G4double fX_0;
  G4double fY_0;
  G4double fZ_0;

  G4ThreeVector fOrg;
  G4ThreeVector fDirection;
};

#endif
