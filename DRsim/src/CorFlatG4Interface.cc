#include "CorFlatG4Interface.hh"

#include "G4RunManager.hh"
#include "G4LorentzVector.hh"
#include "G4Event.hh"
#include "G4PrimaryParticle.hh"
#include "G4PrimaryVertex.hh"
#include "G4TransportationManager.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

CorFlatG4Interface::CorFlatG4Interface()
  : fEvt(0)
{}

CorFlatG4Interface::~CorFlatG4Interface() {
  delete fEvt;
}

G4bool CorFlatG4Interface::CheckVertexInsideWorld(const G4ThreeVector& pos) const {
  G4Navigator* navigator = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();

  G4VPhysicalVolume* world = navigator->GetWorldVolume();
  G4VSolid* solid = world->GetLogicalVolume()->GetSolid();
  EInside qinside = solid->Inside(pos);

  if( qinside != kInside ) return false;
  else return true;
}

void CorFlatG4Interface::FlatCorsika2G4(const EVT* anEvent, G4Event* g4event) {

  std::vector<int>*    branch_PDGID  = std::get<0>(*anEvent);
	std::vector<double>* branch_Px     = std::get<1>(*anEvent);
	std::vector<double>* branch_Py     = std::get<2>(*anEvent);
	std::vector<double>* branch_Pz     = std::get<3>(*anEvent);
	std::vector<double>* branch_E      = std::get<4>(*anEvent);
	std::vector<double>* branch_x      = std::get<5>(*anEvent);
	std::vector<double>* branch_y      = std::get<6>(*anEvent);
	std::vector<double>* branch_Time   = std::get<7>(*anEvent);
	std::vector<double>* branch_Weight = std::get<8>(*anEvent);
  // std::tie(branch_PDGID, branch_Px, branch_Py, branch_Pz, branch_E, branch_x, branch_y, branch_Time, branch_Weight) = *anEvent;
  for ( int idx = 0; idx < branch_PDGID->size(); ++idx ) {
    
    int pdgid = branch_PDGID->at(idx);
    double px = branch_Px->at(idx);
    double py = branch_Py->at(idx);
    double pz = branch_Pz->at(idx); // to make beam downward (toward ground)
    double e = branch_E->at(idx);
    double x = branch_x->at(idx);
    double y = branch_y->at(idx);
    double t = branch_Time->at(idx);
    double w = branch_Weight->at(idx);

    // First, only shoot particles for single DRC module (2.5 m * 2.5 m * 1 m)
    if (! ( ((x * 10) >= -1250.) &&
            ((x * 10) <= 1250.)  &&
            ((y * 10) >= -1250.) &&
            ((y * 10) <= 1250.) )  ) continue;
    
    G4LorentzVector xvtx((x * 10), (y * 10), 0., t);
    if (!CheckVertexInsideWorld(xvtx.vect()*mm)) continue;

    // create G4PrimaryVertex and associated G4PrimaryParticles
    G4PrimaryVertex* g4vtx = new G4PrimaryVertex(xvtx.x()*mm, xvtx.y()*mm, xvtx.z()*mm, xvtx.t()*mm/c_light);
    G4int pdgcode = pdgid;
    G4LorentzVector p(px, py, pz, e);
    G4PrimaryParticle* g4prim = new G4PrimaryParticle(pdgcode, p.x()*GeV, p.y()*GeV, p.z()*GeV);

    g4vtx->SetPrimary(g4prim);

    g4event->AddPrimaryVertex(g4vtx);
  }
}

EVT* CorFlatG4Interface::GenerateCorsikaEvent()
{
  EVT* anEvent = new EVT();
  return anEvent;
}

void CorFlatG4Interface::GeneratePrimaryVertex(G4Event* anEvent)
{
  // delete previous event object
  delete fEvt;

  // generate next event
  fEvt = GenerateCorsikaEvent();
  if(!fEvt) {
    G4cout << "CorsikaInterface: no generated particles. run terminated..."
           << G4endl;
    G4RunManager::GetRunManager()->AbortRun();
    return;
  }
  FlatCorsika2G4(fEvt, anEvent);
}
