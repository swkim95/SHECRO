#include "RootInterface.h"
#include "RecoInterface.h"
#include "DRsimInterface.h"
#include "functions.h"

#include "TROOT.h"
#include "TStyle.h"
#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TPaveStats.h"
#include "TString.h"
#include "TLorentzVector.h"
#include "TGraph.h"

#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
  TString filename = argv[1];
  TString outputname = argv[2];

  gStyle->SetOptFit(1);

  RootInterface<DRsimInterface::DRsimEventData>* drInterface = new RootInterface<DRsimInterface::DRsimEventData>(std::string(filename), false);
  drInterface->GetChain("DRsim");

  TH1F* tEdep = new TH1F("totEdep","Total Energy deposit;MeV;Evt",100,0.,10);
  tEdep->Sumw2(); tEdep->SetLineColor(kRed); tEdep->SetLineWidth(2);
  TH1F* tHit_C = new TH1F("Hit_C","# of p.e. of Cerenkov ch.;# of p.e.;Evt",50,0,50.);
  tHit_C->Sumw2(); tHit_C->SetLineColor(kBlue); tHit_C->SetLineWidth(2);
  TH1F* tHit_S = new TH1F("Hit_S","# of p.e. of Scintillation ch.;# of p.e.;Evt",50,0,50);
  tHit_S->Sumw2(); tHit_S->SetLineColor(kRed); tHit_S->SetLineWidth(2);

  TH1F* tT_C = new TH1F("time_C","Cerenkov time;ns;p.e.",600,10.,70.);
  tT_C->Sumw2(); tT_C->SetLineColor(kBlue); tT_C->SetLineWidth(2);
  TH1F* tT_S = new TH1F("time_S","Scint time;ns;p.e.",600,10.,70.);
  tT_S->Sumw2(); tT_S->SetLineColor(kRed); tT_S->SetLineWidth(2);
  TH1F* tWav_S = new TH1F("wavlen_S","Scint wavelength;nm;p.e.",120,300.,900.);
  tWav_S->Sumw2(); tWav_S->SetLineColor(kRed); tWav_S->SetLineWidth(2);
  TH1F* tWav_C = new TH1F("wavlen_C","Cerenkov wavelength;nm;p.e.",120,300.,900.);
  tWav_C->Sumw2(); tWav_C->SetLineColor(kBlue); tWav_C->SetLineWidth(2);
  TH1F* tNhit_S = new TH1F("nHits_S","Number of Scint p.e./SiPM;p.e.;n",200,0.,200.);
  tNhit_S->Sumw2(); tNhit_S->SetLineColor(kRed); tNhit_S->SetLineWidth(2);
  TH1F* tNhit_C = new TH1F("nHits_C","Number of Cerenkov p.e./SiPM;p.e.;n",50,0.,50.);
  tNhit_C->Sumw2(); tNhit_C->SetLineColor(kBlue); tNhit_C->SetLineWidth(2);

  unsigned int entries = drInterface->entries();
  std::cout << drInterface->entries() << std::endl;

  while (drInterface->numEvt() < entries) {
    if (drInterface->numEvt() % 100 == 0) printf("Analyzing %dth event ...\n", drInterface->numEvt());

    DRsimInterface::DRsimEventData drEvt;
    drInterface->read(drEvt);

    float Edep = 0.;
    for (auto edepItr = drEvt.Edeps.begin(); edepItr != drEvt.Edeps.end(); ++edepItr) {
      auto edep = *edepItr;
      Edep += edep.Edep;
    }
    tEdep->Fill(Edep);

    // float Pleak = 0.;
    // float Eleak_nu = 0.;
    // for (auto leak : drEvt.leaks) {
    //   TLorentzVector leak4vec;
    //   leak4vec.SetPxPyPzE(leak.px,leak.py,leak.pz,leak.E);
    //   if ( std::abs(leak.pdgId)==12 || std::abs(leak.pdgId)==14 || std::abs(leak.pdgId)==16 ) {
    //     Eleak_nu += leak4vec.P();
    //   } else {
    //     Pleak += leak4vec.P();
    //   }
    // }
    // tP_leak->Fill(Pleak);
    // tP_leak_nu->Fill(Eleak_nu);

    int nHitC = 0; int nHitS = 0;
    for (auto tower = drEvt.towers.begin(); tower != drEvt.towers.end(); ++tower) {
      int moduleNum = tower->ModuleNum;
      for (auto sipm = tower->SiPMs.begin(); sipm != tower->SiPMs.end(); ++sipm) {
        int plateNum = sipm->x; int fiberNum = sipm->y; 
        if ( RecoInterface::IsCerenkov(sipm->x,sipm->y) ) {
          tNhit_C->Fill(sipm->count);
          for (const auto timepair : sipm->timeStruct) {
            tT_C->Fill(timepair.first.first+0.05,timepair.second);
            nHitC += timepair.second;

            // if (timepair.first.first < 35) {
            //   nHitC += timepair.second;
            //   // t2DhitC->Fill(60*(moduleNum%7)+fiberNum, 60*(moduleNum/7)+plateNum, timepair.second);
            // }
          }
          for (const auto wavpair : sipm->wavlenSpectrum) {
            tWav_C->Fill(wavpair.first.first,wavpair.second);
          }
        } else {
          tNhit_S->Fill(sipm->count);
          nHitS += sipm->count;
          // t2DhitS->Fill(60*(moduleNum%7)+fiberNum, 60*(moduleNum/7)+plateNum, sipm->count);
          for (const auto timepair : sipm->timeStruct) {
            tT_S->Fill(timepair.first.first+0.05,timepair.second);
          }
          for (const auto wavpair : sipm->wavlenSpectrum) {
            tWav_S->Fill(wavpair.first.first,wavpair.second);
          }
        }
      }
    }

    tHit_C->Fill(nHitC);
    tHit_S->Fill(nHitS);
  } // event loop
  // drInterface->close();

  TCanvas* c = new TCanvas("c","");

  tEdep->Draw("Hist"); c->SaveAs(outputname+"_Edep.png");

  tHit_C->Draw("Hist"); c->SaveAs(outputname+"_nHitpEventC.png");
  tHit_S->Draw("Hist"); c->SaveAs(outputname+"_nHitpEventS.png");

  tHit_C->SetBinContent(1, 0.);
  tHit_S->SetBinContent(1, 0.);

  c->cd(); tHit_C->Draw("Hist"); c->SaveAs(outputname+"_nHitpEventC_bin1to0.png");
  c->cd(); tHit_S->Draw("Hist"); c->SaveAs(outputname+"_nHitpEventS_bin1to0.png");


  tT_C->Draw("Hist"); c->SaveAs(outputname+"_tC.png");
  tT_S->Draw("Hist"); c->SaveAs(outputname+"_tS.png");
  tWav_C->Draw("Hist"); c->SaveAs(outputname+"_wavC.png");
  tWav_S->Draw("Hist"); c->SaveAs(outputname+"_wavS.png");
  tNhit_C->Draw("Hist"); c->SaveAs(outputname+"_nhitC.png");
  tNhit_S->Draw("Hist"); c->SaveAs(outputname+"_nhitS.png");
}

