#include <iomanip>
#include <stdio.h>

#include "TChain.h"
#include "TH1D.h"
#include "TFile.h"
#include "TNtuple.h"

#include "LoadMergedData.h"

void LoadMergedData(const std::string &regex, std::vector<SimpleMCEvent> &eventList){

    // Load in the chains for Hits, MCparticles and Pfos pre and post hit removal
    // --------------------------------------------------------------------------------------
    TChain *chainHit = new TChain("HitDataHit", "chainHit");
    chainHit->Add(regex.c_str());

    TChain *chainPfo = new TChain("HitDataPfo", "chainPfo");
    chainPfo->Add(regex.c_str());

    /*
    TChain *chainMCParticle = new TChain("HitDataMCParticle", "chainMCParticle");
    chainMCParticle->Add(regex.c_str());
    */

    // Read in the data 
    std::cout << "Reading ... " << std::endl;
    std::cout <<   "  |------------------------------|";
    std::cout << "\r  | Reading Hits " << std::endl;
    for (int i=0; i<chainHit->GetEntries(); i++) {
        ReadNextHit(chainHit, i, eventList);
        showLoadingBar(i+1, chainHit->GetEntries(), 30);
    };
    std::cout << std::endl;

    std::cout <<   "  |------------------------------|";
    std::cout << "\r  | Reading Pfos " << std::endl;
    for (int i=0; i<chainPfo->GetEntries(); i++){
        ReadNextPfo(chainPfo, i, eventList);
        showLoadingBar(i+1, chainPfo->GetEntries(), 30);
    }
    std::cout << std::endl;

    /*
    std::cout <<   "  |------------------------------|";
    std::cout << "\r  | Reading MCParticles " << std::endl;
    for (int i=0; i<chainMCParticle->GetEntries(); i++) {
        ReadNextMCParticle(chainMCParticle, i, eventList);
        showLoadingBar(i+1, chainMCParticle->GetEntries(), 30);
    }
    std::cout << std::endl;
    */
}
