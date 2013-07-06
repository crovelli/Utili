#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include <TEntryList.h>
#include <iostream>
#include <set>

using namespace std;

void duplicatesRemoval() {

  // minimal set of variables to be read from the input tree
  Float_t   channel;
  Float_t   run;
  Float_t   lumi;
  ULong64_t event;
  TBranch  *b_channel=0; 
  TBranch  *b_run=0; 
  TBranch  *b_lumi=0;
  TBranch  *b_event=0;

  // opening the input tree root file
  TFile *inFile = TFile::Open("/cmsrm/pc24_2/jorda/data/finalresults_V04/Data/DataAll.root");
  if (inFile==0) {
    printf("Error: cannot open the input root file");
    return;
  }
  TTree *fChain = (TTree *)inFile->Get("latino");

  // reading my input tree
  fChain->SetMakeClass(1);
  fChain->SetBranchAddress("channel", &channel, &b_channel);
  fChain->SetBranchAddress("run",     &run,     &b_run);
  fChain->SetBranchAddress("lumi",    &lumi,    &b_lumi);
  fChain->SetBranchAddress("event",   &event,   &b_event);

  // preparing the output file
  TFile fileOut("/tmp/crovelli/outputFile.root","RECREATE");
  fileOut.cd();

  // preparing the clean tree after duplicated events removal
  TTree *tCleaned = (TTree*)fChain->CloneTree(0);  
  tCleaned->SetName("tCleaned");

  // needed to remove duplicates 
  set<Long64_t> eventIds;
  Long64_t myProduct;
  TEntryList *tlist = new TEntryList(fChain); 
  tlist->SetName("listCleaned");

  // checking the original tree based on run and event numbers
  Long64_t nb;
  Long64_t nentries = fChain->GetEntries();
  for (Long64_t jentry=0; jentry<nentries;jentry++) {
    nb = fChain->GetEntry(jentry); 

    if (jentry%5000==0) cout << jentry << endl;

    myProduct = ((Long64_t)run<<31)+event;

    // only one channel per time
    if (channel!=4) continue;

    // if we have not seen this event yet, add it to the set and to the entry list
    if (eventIds.count(myProduct)==0) {
      eventIds.insert(myProduct);
      tlist->Enter(jentry,fChain);
      tCleaned->Fill();
    } else {
      cout << "this event is duplicated: " << myProduct << " " << run << " " << event << endl;
    }
  }

  // sorting the new tree
  /*
  int newEntries = (Int_t)myTrees->GetEntries();
  myTrees->Draw("newProduct_t","","goff");
  Int_t *index = new Int_t[newEntries];
  TMath::Sort(newEntries,myTrees->GetV1(),index);
 
  TTree *tSorted = (TTree*)myTrees->CloneTree(0);
  tSorted->SetName("myTreesSorted");
  for (Int_t i=0;i<newEntries;i++) {
    myTrees->GetEntry(index[i]);
    tSorted->Fill();
  }
  */

  // saving te trees
  tCleaned-> Write();
  tlist->Write();
  fileOut.Close();

  // delete [] index;
}
