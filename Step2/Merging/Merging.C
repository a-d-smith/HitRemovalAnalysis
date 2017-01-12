#include <iomanip>
#include <stdio.h>

#include "TChain.h"
#include "TH1D.h"
#include "TFile.h"
#include "TNtuple.h"

#include "MergingIO.h"
#include "Merging.h"

void Merge(const std::string &preRegex, const std::string &postRegex, const std::string outputFileName, int nGoodEvents){
    // Load in the chains for Hits, MCparticles and Pfos pre and post hit removal
    // --------------------------------------------------------------------------------------
    TChain *chainHitPre = new TChain("HitDataHit", "chainHitPre");
    chainHitPre->Add(preRegex.c_str());

    TChain *chainPfoPre = new TChain("HitDataPfoPrimary", "chainPfoPre");
    chainPfoPre->Add(preRegex.c_str());

    TChain *chainMCParticlePre = new TChain("HitDataMCParticle", "chainMCParticlePre");
    chainMCParticlePre->Add(preRegex.c_str());

    // --

    TChain *chainHitPost = new TChain("HitDataHit", "chainHitPost");
    chainHitPost->Add(postRegex.c_str());

    TChain *chainMCParticlePost = new TChain("HitDataMCParticle", "chainMCParticlePost");
    chainMCParticlePost->Add(postRegex.c_str());


    // Open the output file 
    TFile *f = new TFile(outputFileName.c_str(), "RECREATE");
    if (!(f->IsOpen())){
        std::cerr << "Error: Can not open root file for output" << std::endl;
    }
    TTree *treeHit   = new TTree("HitDataHit", "HitDataHit");
    TTree *treePfo   = new TTree("HitDataPfo", "HitDataPfo");
    TTree *treePart  = new TTree("HitDataMCParticle", "HitDataMCParticle");
    
    // Set up the branch
    int hitFileId, hitEventId, hitUid, hitParticleUid, hitPfoUid;
    char hitView;
    double hitX, hitZ;
    bool hitIsRemoved, hitIsNeutrinoInduced;
    treeHit->Branch("FileId"           , &hitFileId);
    treeHit->Branch("EventId"          , &hitEventId);
    treeHit->Branch("UniqueId"         , &hitUid);
    treeHit->Branch("View"             , &hitView);
    treeHit->Branch("X"                , &hitX);
    treeHit->Branch("Z"                , &hitZ);
    treeHit->Branch("IsRemoved"        , &hitIsRemoved);
    treeHit->Branch("IsNeutrinoInduced", &hitIsNeutrinoInduced);
    treeHit->Branch("MCParticleUid"    , &hitParticleUid);
    treeHit->Branch("PfoUid"           , &hitPfoUid);

    int  pfoFileId, pfoEventId, pfoUid;
    bool pfoIsRemoved;
    double pfoFractionRemoved;
    std::vector<int> *pfoHitList = new std::vector<int>;
    treePfo->Branch("FileId"         , &pfoFileId);
    treePfo->Branch("EventId"        , &pfoEventId);
    treePfo->Branch("UniqueId"       , &pfoUid);
    treePfo->Branch("IsRemoved"      , &pfoIsRemoved);
    treePfo->Branch("FractionRemoved", &pfoFractionRemoved);
    treePfo->Branch("HitList"        , &pfoHitList);

    int partFileId, partEventId, partUid, partPdg;
    bool partIsNeutrinoInduced;
    double partStartX, partStartY, partStartZ, partEndX, partEndY, partEndZ;
    std::vector<int> *partHitList = new std::vector<int>;
    treePart->Branch("FileId"           , &partFileId);
    treePart->Branch("EventId"          , &partEventId);
    treePart->Branch("UniqueId"         , &partUid);
    treePart->Branch("PdgCode"          , &partPdg);
    treePart->Branch("IsNeutrinoInduced", &partIsNeutrinoInduced);
    treePart->Branch("StartX"           , &partStartX);
    treePart->Branch("StartY"           , &partStartY);
    treePart->Branch("StartZ"           , &partStartZ);
    treePart->Branch("EndX"             , &partEndX);
    treePart->Branch("EndY"             , &partEndY);
    treePart->Branch("EndZ"             , &partEndZ);
    treePart->Branch("HitList"          , &partHitList);

    // Find the first nGoodEvents events which contain neutrino hits
    std::cout << "Reading ... " << std::endl;
    std::cout <<   "  |------------------------------|";
    std::cout << "\r  | Finding Good Events " << std::endl;
    std::vector<SimpleMCEvent> goodEventList;
    int n = 0;
    for (int i=0; i<chainHitPre->GetEntries()       ; i++) {
        if (GetGoodEvent(chainHitPre, i, goodEventList)) n++;
        showLoadingBar(n, nGoodEvents, 30);
        if (n == nGoodEvents) break;
    }
    std::cout << std::endl;

    if (n < nGoodEvents){
        std::cout << "Error: The input files only contain " << n << " good events" << std::endl;
        exit(1);
    }


    // Read in the data pre hit removal
    std::vector<SimpleMCEvent> preEventList;
    std::cout <<   "  |------------------------------|";
    std::cout << "\r  | Reading Pre Hits " << std::endl;
    for (int i=0; i<chainHitPre->GetEntries()       ; i++) {
        ReadNextHit(chainHitPre, i, preEventList, true, goodEventList);
        showLoadingBar(i+1, chainHitPre->GetEntries(), 30);
    }
    std::cout << std::endl;

    std::cout <<   "  |------------------------------|";
    std::cout << "\r  | Reading Pre Pfos " << std::endl;
    for (int i=0; i<chainPfoPre->GetEntries()       ; i++){
        ReadNextPfo(chainPfoPre, i, preEventList, goodEventList);
        showLoadingBar(i+1, chainPfoPre->GetEntries(), 30);
    }
    std::cout << std::endl;

    std::cout <<   "  |------------------------------|";
    std::cout << "\r  | Reading Pre MCParticles " << std::endl;
    for (int i=0; i<chainMCParticlePre->GetEntries(); i++) {
        ReadNextMCParticle(chainMCParticlePre, i, preEventList, goodEventList);
        showLoadingBar(i+1, chainMCParticlePre->GetEntries(), 30);
    }
    std::cout << std::endl;

    // Read in the data post hit removal
    std::vector<SimpleMCEvent> postEventList;
    std::cout <<   "  |------------------------------|";
    std::cout << "\r  | Reading Post Hits " << std::endl;
    for (int i=0; i<chainHitPost->GetEntries()       ; i++) {
        ReadNextHit(chainHitPost, i, postEventList, false, goodEventList);
        showLoadingBar(i+1, chainHitPost->GetEntries(), 30);
    }
    std::cout << std::endl;

    /*
    std::cout <<   "  |------------------------------|";
    std::cout << "\r  | Reading Post MCParticles " << std::endl;
    for (int i=0; i<chainMCParticlePost->GetEntries(); i++) { 
        ReadNextMCParticle(chainMCParticlePost, i, postEventList, goodEventList);
        showLoadingBar(i+1, chainMCParticlePost->GetEntries(), 30);
    }
    std::cout << std::endl;
    */

    // Merge the information pre and post hit removal
    std::cout << "Merging ..." << std::endl;
    std::cout <<   "  |------------------------------|";
    std::cout << "\r  | Merging Event Data " << std::endl;
    showLoadingBar(0, 1, 30);
    std::vector<SimpleMCEvent> mergedEventList;
    MergeEvents(preEventList, postEventList, mergedEventList);


    // Write the merged information to a file
    for (SimpleMCEvent &event : mergedEventList){
        // Hits
        for (SimpleCaloHit &hit : event.GetCaloHitList()){
            hitFileId      = hit.GetId().GetFileId();
            hitEventId     = hit.GetId().GetEventId();
            hitUid         = hit.GetId().GetUid();
            hitParticleUid = hit.GetMCParticleId().GetUid();
            hitPfoUid      = hit.GetPfoId().GetUid();
            hitView        = hit.GetView();
            hitX           = hit.GetX();
            hitZ           = hit.GetZ();
            hitIsRemoved   = hit.IsRemoved();
            hitIsNeutrinoInduced = hit.IsNeutrinoInduced();

            treeHit->Fill();
        }
        // Pfos
        for (SimplePfo &pfo : event.GetPfoList()){
            pfoFileId          = pfo.GetId().GetFileId();
            pfoEventId         = pfo.GetId().GetEventId();
            pfoUid             = pfo.GetId().GetUid();
            pfoIsRemoved       = pfo.IsRemoved();
            pfoFractionRemoved = pfo.GetFractionRemoved();
            
            pfoHitList->clear();
            for (Identifier &id : pfo.GetHitList()){
                pfoHitList->push_back(id.GetUid());
            }

            treePfo->Fill();
        }
        // MCParticles
        for (SimpleMCParticle &part : event.GetMCParticleList()){
            partFileId            = part.GetId().GetFileId();
            partEventId           = part.GetId().GetEventId();
            partUid               = part.GetId().GetUid();
            partPdg               = part.GetPdg();
            partIsNeutrinoInduced = part.IsNeutrinoInduced();
            partStartX            = part.GetStartX();
            partStartY            = part.GetStartY();
            partStartZ            = part.GetStartZ();
            partEndX              = part.GetEndX();
            partEndY              = part.GetEndY();
            partEndZ              = part.GetEndZ();

            partHitList->clear();
            for (Identifier &id : part.GetHitList()){
                partHitList->push_back(id.GetUid());
            }

            treePart->Fill();
        }
    }

    // Close the file
    f->Write();
    f->Close();
    
}
