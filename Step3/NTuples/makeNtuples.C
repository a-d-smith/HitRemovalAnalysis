#include <iostream>
#include <iomanip>
#include <numeric>

#include "../../SimpleObjects/SimpleObjects.h"
#include "../../SimpleObjects/SimpleObjectHelper.h"

void makeNtuples(const char *filePath, int outputFileId){
    // Load the data
    std::vector<SimpleMCEvent> eventList;
    LoadMergedData(filePath, eventList);

    // Setup the ouput file
    TFile *f = TFile::Open(("MergedData-nTuples-" + std::to_string(outputFileId) + ".root").c_str(), "RECREATE");

    // Raw hit data 
    TNtuple *ntHitData = new TNtuple("HitData", "HitData", "fileId:eventId:uniqueId:view:x:z:isRemoved:type:inPfo:pfoId");
    // type = 0 -> Ghost
    // type = 1 -> Neutrino
    // type = 2 -> Cosmic

    // The number of hits of each type that are removed (or not) in and not in a pfo per event (that *DID* make sense)
    TNtuple *ntHitDist = new TNtuple("HitDist", "HitDist", "fileId:eventId:nNuPfo:nCoPfo:nGhPfo:nNuPfoRemoved:nCoPfoRemoved:nGhPfoRemoved:nNuFree:nCoFree:nGhFree:nNuFreeRemoved:nCoFreeRemoved:nGhFreeRemoved");

    // The of hits of each type that are removed (or not) on a pfo by pfo basis
    TNtuple *ntPfoDist = new TNtuple("PfoDist", "PfoDist", "fileId:eventId:uniqueId:nNu:nCo:nGh:nNuRemoved:nCoRemoved:nGhRemoved:nNuTot:nCoTot:nGhTot");

    for (SimpleMCEvent &event : eventList){
        // ------------------------------------------------------------------
        int nNeutrinoHits = 0;
        int nGhostHits    = 0;
        int nCosmicHits   = 0;

        int nNeutrinoHitsRemoved = 0;
        int nGhostHitsRemoved    = 0;
        int nCosmicHitsRemoved   = 0;

        int nFreeNeutrinoHits = 0;
        int nFreeCosmicHits   = 0;
        int nFreeGhostHits    = 0;

        int nFreeNeutrinoHitsRemoved = 0;
        int nFreeCosmicHitsRemoved   = 0;
        int nFreeGhostHitsRemoved    = 0;

        for (SimpleCaloHit &hit : event.GetCaloHitList()){
            // Hits not in a pfo (free)
            if (hit.GetPfoId().GetUid() == -1){
                if (IsGhostHit(hit, event)){
                    ntHitData->Fill(hit.GetId().GetFileId(), hit.GetId().GetEventId(), hit.GetId().GetUid(), GetIntView(hit.GetView()), hit.GetX(), hit.GetZ(), (hit.IsRemoved() ? 1 : 0), 0, 0, hit.GetPfoId().GetUid()); 
                    nFreeGhostHits++;
                    if (hit.IsRemoved()){
                        nFreeGhostHitsRemoved++;
                        std::cerr << "A free ghost hit has been removed!" << std::endl;
                    }
                }
                else{
                    if (hit.IsNeutrinoInduced()){
                        ntHitData->Fill(hit.GetId().GetFileId(), hit.GetId().GetEventId(), hit.GetId().GetUid(), GetIntView(hit.GetView()), hit.GetX(), hit.GetZ(), (hit.IsRemoved() ? 1 : 0), 1, 0, hit.GetPfoId().GetUid()); 
                        nFreeNeutrinoHits++;
                        if (hit.IsRemoved()){
                            nFreeNeutrinoHitsRemoved++;
                            std::cerr << "A free neutrino hit has been removed!" << std::endl;
                        }
                    }
                    else{
                        nFreeCosmicHits++;
                        ntHitData->Fill(hit.GetId().GetFileId(), hit.GetId().GetEventId(), hit.GetId().GetUid(), GetIntView(hit.GetView()), hit.GetX(), hit.GetZ(), (hit.IsRemoved() ? 1 : 0), 2, 0, hit.GetPfoId().GetUid()); 
                        if (hit.IsRemoved()){
                            nFreeCosmicHitsRemoved++;
                            std::cerr << "A free cosmic hit has been removed!" << std::endl;
                        }
                    }
                }
            }
            // Hits in a pfo
            else{
                if (IsGhostHit(hit, event)){
                    ntHitData->Fill(hit.GetId().GetFileId(), hit.GetId().GetEventId(), hit.GetId().GetUid(), GetIntView(hit.GetView()), hit.GetX(), hit.GetZ(), (hit.IsRemoved() ? 1 : 0), 0, 1, hit.GetPfoId().GetUid()); 
                    nGhostHits++;
                    if (hit.IsRemoved()) nGhostHitsRemoved++;
                }
                else{
                    if (hit.IsNeutrinoInduced()){
                        ntHitData->Fill(hit.GetId().GetFileId(), hit.GetId().GetEventId(), hit.GetId().GetUid(), GetIntView(hit.GetView()), hit.GetX(), hit.GetZ(), (hit.IsRemoved() ? 1 : 0), 1, 1, hit.GetPfoId().GetUid()); 
                        nNeutrinoHits++;
                        if (hit.IsRemoved()) nNeutrinoHitsRemoved++;
                    }
                    else{
                        ntHitData->Fill(hit.GetId().GetFileId(), hit.GetId().GetEventId(), hit.GetId().GetUid(), GetIntView(hit.GetView()), hit.GetX(), hit.GetZ(), (hit.IsRemoved() ? 1 : 0), 2, 1, hit.GetPfoId().GetUid()); 
                        nCosmicHits++;
                        if (hit.IsRemoved()) nCosmicHitsRemoved++;
                    }
                }
            }
        }

        ntHitDist->Fill(event.GetFileId(),
                        event.GetEventId(), 
                        // Total number of hits of each type that are in a PFO
                        nNeutrinoHits,
                        nCosmicHits,
                        nGhostHits,
                        // Total number of hits that are removed of each type that are in a PFO
                        nNeutrinoHitsRemoved,
                        nCosmicHitsRemoved,
                        nGhostHitsRemoved,
                        // Total number of hits of each type that are not in a PFO
                        nFreeNeutrinoHits,
                        nFreeCosmicHits,
                        nFreeGhostHits,
                        // Total number of hits that are removed of each type that are not in a PFO
                        nFreeNeutrinoHitsRemoved,
                        nFreeCosmicHitsRemoved,
                        nFreeGhostHitsRemoved);
        


        // ------------------------------------------------------------------
        // Run over each of the PFOs
        for (SimplePfo &pfo : event.GetPfoList()){
            int nPfoNeutrinoHits = 0;
            int nPfoGhostHits    = 0;
            int nPfoCosmicHits   = 0;

            int nPfoNeutrinoHitsRemoved = 0;
            int nPfoGhostHitsRemoved    = 0;
            int nPfoCosmicHitsRemoved   = 0;


            for (SimpleCaloHit &hit : event.GetCaloHitList()){
                if (hit.GetPfoId() == pfo.GetId()){
                    if (IsGhostHit(hit, event)){
                        nPfoGhostHits++;
                        if (hit.IsRemoved()) nPfoGhostHitsRemoved++;
                    }
                    else{
                        if (hit.IsNeutrinoInduced()){
                            nPfoNeutrinoHits++;
                            if (hit.IsRemoved()) nPfoNeutrinoHitsRemoved++;
                        }
                        else{
                            nPfoCosmicHits++;
                            if (hit.IsRemoved()) nPfoCosmicHitsRemoved++;
                        }
                    }
                }
            }
            
            ntPfoDist->Fill(event.GetFileId(),
                            event.GetEventId(),
                            pfo.GetId().GetUid(),
                            // Number of hits of each type in the pfo
                            nPfoNeutrinoHits,
                            nPfoCosmicHits,
                            nPfoGhostHits,
                            // Number of hits removed of each type
                            nPfoNeutrinoHitsRemoved,
                            nPfoCosmicHitsRemoved,
                            nPfoGhostHitsRemoved,
                            // Total number of hits of each type in the event
                            nNeutrinoHits,
                            nCosmicHits,
                            nGhostHits);
        }
    }


    // Write and close
    f->Write();
    f->Close();

}
