#include <iostream>
#include <iomanip>
#include <numeric>
#include <utility>
#include <limits>
#include <map>

#include "../../SimpleObjects/SimpleObjects.h"
#include "../../SimpleObjects/SimpleObjectHelper.h"


void GetMCNeutrinoNHitMap(SimplePfo & pfo,  std::vector<SimpleCaloHit> & hitList, std::vector<SimpleMCParticle> & MCParticleList, std::map<int, int> & MCNeutrinoNHitMap);
void GetMCNeutrinoNHitTotalMap(std::vector<SimpleCaloHit> & hitList, std::vector<SimpleMCParticle> & MCParticleList, std::map<int, int> & MCNeutrinoNHitTotalMap);
void FindCaloHit( Identifier & hitID,  std::vector<SimpleCaloHit> & hitList, SimpleCaloHit & hitFound);
void FindMCParticle( const Identifier & partID,  std::vector<SimpleMCParticle> & MCParticleList, SimpleMCParticle & partFound);

void getMCParticleAssociations( char *filePath, int outputFileId){
    // Load the data
    std::vector<SimpleMCEvent> eventList;
    LoadMergedData(filePath, eventList);


    // Output file
    TFile *f = TFile::Open(("MergedData-nTuples-associations-" + std::to_string(outputFileId) + ".root").c_str(), "RECREATE");

    // Raw hit data 
    TNtuple *ntHitData = new TNtuple("HitData", "HitData", "fileId:eventId:uniqueId:view:x:z:isRemoved:type:inPfo:pfoId");
    // type = 0 -> Ghost
    // type = 1 -> Neutrino
    // type = 2 -> Cosmic

    // The number of hits of each type that are removed (or not) in and not in a pfo per event (that *DID* make sense)
    TNtuple *ntHitDist = new TNtuple("HitDist", "HitDist", "fileId:eventId:nNuPfo:nCoPfo:nGhPfo:nNuPfoRemoved:nCoPfoRemoved:nGhPfoRemoved:nNuFree:nCoFree:nGhFree:nNuFreeRemoved:nCoFreeRemoved:nGhFreeRemoved");

    // The of hits of each type that are removed (or not) on a pfo by pfo basis
    TNtuple *ntPfoDist = new TNtuple("PfoDist", "PfoDist", "fileId:eventId:uniqueId:nNu:nCo:nGh:nNuRemoved:nCoRemoved:nGhRemoved:nNuTot:nCoTot:nGhTot:nPrimary");

    // First collect information on all of the hits
    for (SimpleMCEvent &event : eventList){
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

        // ====================================================================================================================================================

        std::cout << "Event " << event.GetEventId() << std::endl;
        std::cout << "-----------------------------------------" << std::endl;

        // Get the list of all all primary visible neutrino MCParticle uids and the number of hits associated with them
        std::map<int, int> MCNeutrinoNHitTotalMap;
        GetMCNeutrinoNHitTotalMap(event.GetCaloHitList(), event.GetMCParticleList(), MCNeutrinoNHitTotalMap);
        
        // For each PFO
        for (SimplePfo & pfo : event.GetPfoList()){

            // Count the number of hits of each type in this pfo
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
            

            // Map between MC neutrino ID and the number of hits from that particle in this PFO
            std::map<int, int> MCNeutrinoNHitMap;
            GetMCNeutrinoNHitMap(pfo, event.GetCaloHitList(), event.GetMCParticleList(), MCNeutrinoNHitMap);

            // The number of primary visible neutrino MCParticle with which this PFO is assoaciated with (continuous)
            double nPrimary = 0;

            if (MCNeutrinoNHitMap.size() != 0){
                std::cout << "PFO " << pfo.GetId().GetUid() << " - number of neutrino primary MC matches " << MCNeutrinoNHitMap.size() << std::endl;
    
                // Calcualte the number of primary MCParticles this PFO is associate with
                for (std::map<int, int>::iterator it=MCNeutrinoNHitMap.begin(); it != MCNeutrinoNHitMap.end(); it++){
                    if (it->first != -1){
                        SimpleMCParticle particle;
                        FindMCParticle(Identifier(event.GetFileId(), event.GetEventId(), it->first), event.GetMCParticleList(), particle);
                        std::cout << " > " << it->first << " " << particle.GetPdg() << "  :  " << it->second << " / " << MCNeutrinoNHitTotalMap.at(it->first) << std::endl;
                        
                        nPrimary += ((double) it->second) / ( (double) MCNeutrinoNHitTotalMap.at(it->first) );
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
                            nGhostHits,
                            // The (continuous) number of primary visible neutrino MCParticles with which this PFO is associated
                            nPrimary);

        }

        std::cout << std::endl << std::endl;
    }

    // Write and close
    f->Write();
    f->Close();
}

// Function takes a SimplePfo, a list of MC neutrinos, the list of calo hits, and a map between MC neutrino ID and number of hits
// and populates it with the number of hits in the PFO for each MC neutrino
void GetMCNeutrinoNHitMap(SimplePfo & pfo,  std::vector<SimpleCaloHit> & hitList, std::vector<SimpleMCParticle> & MCParticleList, std::map<int, int> & MCNeutrinoNHitMap){

    // Loop over all of the hits in the PFO
    for (Identifier & hitID : pfo.GetHitList()){
    
        // Find the hit with this ID
        SimpleCaloHit thisHit;
        FindCaloHit(hitID, hitList, thisHit);

        if (!thisHit.IsNeutrinoInduced())
            continue;

        Identifier MCParticleId = thisHit.GetMCParticleId();
        SimpleMCParticle thisMCparticle;
        FindMCParticle(MCParticleId, MCParticleList, thisMCparticle);
        int neutrinoID = thisMCparticle.GetPrimaryVisibleNeutrinoId().GetUid();

        // Check if an entry for that ID exists if not make one!
        if ( MCNeutrinoNHitMap.find(neutrinoID) == MCNeutrinoNHitMap.end() ){
            MCNeutrinoNHitMap.insert(std::make_pair(neutrinoID, 1));
        }
        else{
            MCNeutrinoNHitMap.at(neutrinoID)++;
        }

    }
}

// Function which returns a map between all of the primary visible neutrino MC particle Uids to the number of hits associated with them
void GetMCNeutrinoNHitTotalMap(std::vector<SimpleCaloHit> & hitList, std::vector<SimpleMCParticle> & MCParticleList, std::map<int, int> & MCNeutrinoNHitTotalMap){
    
    // Loop over all of the hits in the event
    for (SimpleCaloHit & thisHit : hitList){
    
        if (!thisHit.IsNeutrinoInduced())
            continue;

        // Get the primary visible neutrino MC particle ID
        Identifier MCParticleId = thisHit.GetMCParticleId();
        SimpleMCParticle thisMCparticle;
        FindMCParticle(MCParticleId, MCParticleList, thisMCparticle);
        int neutrinoID = thisMCparticle.GetPrimaryVisibleNeutrinoId().GetUid();

        // Check if an entry for that ID exists if not make one!
        if ( MCNeutrinoNHitTotalMap.find(neutrinoID) == MCNeutrinoNHitTotalMap.end() ){
            MCNeutrinoNHitTotalMap.insert(std::make_pair(neutrinoID, 1));
        }
        else{
            MCNeutrinoNHitTotalMap.at(neutrinoID)++;
        }

    }
}

// Function to find a hit from its ID in a list of hits
void FindCaloHit( Identifier & hitID,  std::vector<SimpleCaloHit> & hitList, SimpleCaloHit & hitFound) {
    
    bool foundHit = false;
    for ( SimpleCaloHit & hit : hitList){
        if (hit.GetId() == hitID) {
            hitFound = hit;
            foundHit = true;
            break;
        }
    } 

    if (!foundHit){
        std::cerr << "Error: Can't find calo hit!" << std::endl;
        exit(1);
    }
}

// Function to find an MCParticle from its ID and a list of MCParticles
void FindMCParticle( const Identifier & partID,  std::vector<SimpleMCParticle> & MCParticleList, SimpleMCParticle & partFound){

    bool foundPart = false;
    for ( SimpleMCParticle & part : MCParticleList){
        if (part.GetId() == partID) {
            partFound = part;
            foundPart = true;
            break;
        }
    } 

    if (!foundPart){
        std::cerr << "Error: Can't find MCParticle!" << std::endl;
        exit(1);
    }
}

