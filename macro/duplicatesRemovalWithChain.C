#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TChain.h"
#include <TEntryList.h>
#include <iostream>
#include <set>

using namespace std;

void duplicatesRemovalWithChain() {

  // minimal set of variables to be read from the input chain
  Int_t   run;
  Int_t   lumi;
  ULong64_t event;
  // 
  TBranch  *b_run=0; 
  TBranch  *b_lumi=0;
  TBranch  *b_event=0;

  // opening the input tree root file
  TChain *fChain = new TChain("latino");
  // fChain->Add("/cmsrm/pc24_2/jorda/data/TopHiggs5.3.X_V04/V04/Data/DoubleMu-Run2012AB/DoubleMu-Run2012*_*-datasetMME.root");
  // fChain->Add("/cmsrm/pc24_2/jorda/data/TopHiggs5.3.X_V04/V04/Data/DoubleMu-Run2012C/DoubleMu-Run2012*_*-datasetMME.root");
  fChain->Add("/cmsrm/pc24_2/jorda/data/TopHiggs5.3.X_V04/V04/Data/DoubleMu-Run2012D/DoubleMu-Run2012*_*-datasetMME.root");
  // fChain->Add("/cmsrm/pc24_2/jorda/data/TopHiggs5.3.X_V04/V04/Data/DoubleElectron-Run2012AB/DoubleElectron-Run2012*_*-datasetMME.root");
  // fChain->Add("/cmsrm/pc24_2/jorda/data/TopHiggs5.3.X_V04/V04/Data/DoubleElectron-Run2012C/DoubleElectron-Run2012*_*-datasetMME.root");
  // fChain->Add("/cmsrm/pc24_2/jorda/data/TopHiggs5.3.X_V04/V04/Data/DoubleElectron-Run2012D/DoubleElectron-Run2012*_*-datasetMME.root");
  // fChain->Add("/cmsrm/pc24_2/jorda/data/TopHiggs5.3.X_V04/V04/Data/MuEG-Run2012AB/MuEG-Run2012*_*-datasetMME.root");
  // fChain->Add("/cmsrm/pc24_2/jorda/data/TopHiggs5.3.X_V04/V04/Data/MuEG-Run2012C/MuEG-Run2012*_*-datasetMME.root");
  // fChain->Add("/cmsrm/pc24_2/jorda/data/TopHiggs5.3.X_V04/V04/Data/MuEG-Run2012D/MuEG-Run2012*_*-datasetMME.root");
  
  // reading my input tree
  fChain->SetBranchAddress("run",     &run,     &b_run);
  fChain->SetBranchAddress("lumi",    &lumi,    &b_lumi);
  fChain->SetBranchAddress("event",   &event,   &b_event);

  // preparing the output file
  TFile fileOut("/tmp/crovelli/outputFile.root","RECREATE");
  fileOut.cd();

  // needed to remove duplicates 
  set<Long64_t> eventIds;
  Long64_t myProduct;
  fChain->Draw(">>tlist", "run>-1", "entrylist");
  TEntryList *tlist = (TEntryList*)gDirectory->Get("tlist");
  if (tlist==0) { cout << "your tlist is empty" << endl; return; }
  tlist->SetName("listCleaned");
  tlist->Reset();

  // checking the original tree based on run and event numbers
  Long64_t nb;
  Long64_t nentries = fChain->GetEntries();
  for (Long64_t jentry=0; jentry<nentries;jentry++) {
    nb = fChain->GetEntry(jentry); 

    if (jentry%5000==0) cout << jentry << endl;

    myProduct = ((Long64_t)run<<31)+event;

    // if we have not seen this event yet, add it to the set and to the entry list
    if (eventIds.count(myProduct)==0) {
      eventIds.insert(myProduct);
      tlist->Enter(jentry,fChain);
    } else {
      cout << "this event is duplicated: " << myProduct << " " << run << " " << event << " " << fChain->GetFile()->GetName() << endl;
    }
  }

  // saving te trees
  tlist->Write();
  fileOut.Close();
}
