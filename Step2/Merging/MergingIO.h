#ifndef MERGING_IO_H
#define MERGING_IO_H 1

#include <cmath>
#include <iostream>
#include <fstream>
#include <limits>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "../../../SimpleObjects/SimpleObjects.h"

int min(int a, int b){
    return ((a < b) ? a : b);
}

void showLoadingBar(int i, int tot, int width){
    int N = min(width, (int) std::floor(width * (((double) i) / ((double) tot))));
    std::cout << "\r  |" << std::string(N, '=') << "|";
    std::fflush(stdout);
}

/**
 *  @brief  Read a hit and if it is neutrino induced then add its event to the good event list
 * 
 *  @param  pTChain the address of the chain
 *  @param  iEntry the first chain entry to read
 *  @param  goodEventList list of identifiers for the good events
 */
bool GetGoodEvent(TChain *const pTChain, const unsigned int iEntry, std::vector<SimpleMCEvent> & goodEventList) {
    // Sometimes we find events that have no neutrino hits, and so this functions allows us to cut such events
    int fileId, eventId, isNeutrinoInduced;
    pTChain->SetBranchAddress("FileId"           , &fileId);
    pTChain->SetBranchAddress("EventId"          , &eventId);
    pTChain->SetBranchAddress("IsNeutrinoInduced", &isNeutrinoInduced);
    pTChain->GetEntry(iEntry);
    pTChain->ResetBranchAddresses();

    if (isNeutrinoInduced == 1){
        // Check if this event already exists
        bool eventExists = false;
        for (SimpleMCEvent &event : goodEventList){
            if (event.GetFileId() == fileId && event.GetEventId() == eventId) {
                eventExists = true;
                break;
            }
        }
        if (!eventExists){
            SimpleMCEvent newEvent(fileId, eventId);
            goodEventList.push_back(newEvent);
            return true;
        }
    }

    return false;
}


/**
 *  @brief  Read a hit from the chain
 * 
 *  @param  pTChain the address of the chain
 *  @param  iEntry the first chain entry to read
 *  @param  mcEventList the event list to be populated
 *  @param  isPreHitRemoval
 */
void ReadNextHit(TChain *const pTChain, const unsigned int iEntry, std::vector<SimpleMCEvent> & mcEventList, bool isPreHitRemoval, std::vector<SimpleMCEvent> & goodEventList){
    // Check if the event is good
    int fileIdTest, eventIdTest;
    pTChain->SetBranchAddress("FileId"           , &fileIdTest);
    pTChain->SetBranchAddress("EventId"          , &eventIdTest);
    pTChain->GetEntry(iEntry);
    pTChain->ResetBranchAddresses();
    bool isGoodEvent = false;
    for (SimpleMCEvent &event : goodEventList){
        if (event.GetFileId() == fileIdTest && event.GetEventId() == eventIdTest) {
            isGoodEvent = true;
            break;
        }
    }
    if (!isGoodEvent) return;
    // ------------------------------------

    // Get the hit information
    int fileId, eventId, uniqueId, mcParticleId, pfoId, isNeutrinoInduced;
    pTChain->SetBranchAddress("FileId"      , &fileId);
    pTChain->SetBranchAddress("EventId"     , &eventId);
    pTChain->SetBranchAddress("UniqueId"    , &uniqueId);
    pTChain->SetBranchAddress("MCParticleId", &mcParticleId);

    if (isPreHitRemoval){
        pTChain->SetBranchAddress("PfoId"            , &pfoId);
        pTChain->SetBranchAddress("IsNeutrinoInduced", &isNeutrinoInduced);
    }
    else{
        pfoId = -1;
        isNeutrinoInduced = -1;
    }

    int viewInt;
    pTChain->SetBranchAddress("View", &viewInt);

    float x, z;
    pTChain->SetBranchAddress("X", &x);
    pTChain->SetBranchAddress("Z", &z);

    // Read the information
    pTChain->GetEntry(iEntry);
    pTChain->ResetBranchAddresses();

    char view;
    switch (viewInt){
        case 0:
            view = 'W';
            break;
        case 1:
            view = 'U';
            break;
        case 2:
            view = 'V';
            break;
        default:
            view = 'X';
    }

    // Check if this event already exists
    bool eventExists = false;
    int  eventIndex;
    for (unsigned int i=0; i<mcEventList.size(); i++){
        SimpleMCEvent &event = mcEventList[i];
        if (event.GetFileId() == fileId && event.GetEventId() == eventId) {
            eventExists = true;
            eventIndex  = i;
            break;
        }
    }
    if (!eventExists){
        SimpleMCEvent newEvent(fileId, eventId);
        mcEventList.push_back(newEvent);
        eventIndex = mcEventList.size() - 1;
    }
    SimpleMCEvent &thisEvent = mcEventList[eventIndex];

    // Make the hit and add it to the event
    SimpleCaloHit thisHit(Identifier(fileId, eventId, uniqueId), view, x, z, false, Identifier(fileId, eventId, mcParticleId), Identifier(fileId, eventId, pfoId), (isNeutrinoInduced == 1));
    thisEvent.AddCaloHit(thisHit);
}



/**
 *  @brief  Read a pfo from the chain
 * 
 *  @param  pTChain the address of the chain
 *  @param  iEntry the first chain entry to read
 *  @param  mcEventList the event list to be populated
 */
void ReadNextPfo(TChain *const pTChain, const unsigned int iEntry, std::vector<SimpleMCEvent> & mcEventList, std::vector<SimpleMCEvent> & goodEventList){
    // Check if the event is good
    int fileIdTest, eventIdTest;
    pTChain->SetBranchAddress("FileId"           , &fileIdTest);
    pTChain->SetBranchAddress("EventId"          , &eventIdTest);
    pTChain->GetEntry(iEntry);
    pTChain->ResetBranchAddresses();
    bool isGoodEvent = false;
    for (SimpleMCEvent &event : goodEventList){
        if (event.GetFileId() == fileIdTest && event.GetEventId() == eventIdTest) {
            isGoodEvent = true;
            break;
        }
    }
    if (!isGoodEvent) return;
    // ------------------------------------
    
    // Get the pfo information
    int fileId, eventId, uniqueId;
    std::vector<int> *hitUidList(NULL);
    pTChain->SetBranchAddress("FileId"      , &fileId);
    pTChain->SetBranchAddress("EventId"     , &eventId);
    pTChain->SetBranchAddress("UniqueId"    , &uniqueId);
    pTChain->SetBranchAddress("HitUidList"  , &hitUidList);

    // Read the information
    pTChain->GetEntry(iEntry);
    pTChain->ResetBranchAddresses();
    

    // Check if this event already exists
    bool eventExists = false;
    int  eventIndex;
    for (unsigned int i=0; i<mcEventList.size(); i++){
        SimpleMCEvent &event = mcEventList[i];
        if (event.GetFileId() == fileId && event.GetEventId() == eventId) {
            eventExists = true;
            eventIndex  = i;
            break;
        }
    }
    if (!eventExists){
        SimpleMCEvent newEvent(fileId, eventId);
        mcEventList.push_back(newEvent);
        eventIndex = mcEventList.size() - 1;
    }
    SimpleMCEvent &thisEvent = mcEventList[eventIndex];

    std::vector<Identifier> hitList;
    for (unsigned int i=0; i<hitUidList->size(); i++){
        hitList.push_back(Identifier(fileId, eventId, hitUidList->at(i)));
    } 

    // Make the particle and add it to the event
    SimplePfo thisPfo(Identifier(fileId, eventId, uniqueId), hitList, false, 0);
    thisEvent.AddPfo(thisPfo);

}


/**
 *  @brief  Read an MCParticle from the chain
 * 
 *  @param  pTChain the address of the chain
 *  @param  iEntry the first chain entry to read
 *  @param  mcEventList the event list to be populated
 */
void ReadNextMCParticle(TChain *const pTChain, const unsigned int iEntry, std::vector<SimpleMCEvent> & mcEventList, std::vector<SimpleMCEvent> & goodEventList){
    // Check if the event is good
    int fileIdTest, eventIdTest;
    pTChain->SetBranchAddress("FileId"           , &fileIdTest);
    pTChain->SetBranchAddress("EventId"          , &eventIdTest);
    pTChain->GetEntry(iEntry);
    pTChain->ResetBranchAddresses();
    bool isGoodEvent = false;
    for (SimpleMCEvent &event : goodEventList){
        if (event.GetFileId() == fileIdTest && event.GetEventId() == eventIdTest) {
            isGoodEvent = true;
            break;
        }
    }
    if (!isGoodEvent) return;
    // ------------------------------------
    
    // Get the MCParticle information
    int fileId, eventId, uniqueId, pdg, neutrinoInduced;
    std::vector<int> *hitUidList(NULL);
    pTChain->SetBranchAddress("FileId"           , &fileId);
    pTChain->SetBranchAddress("EventId"          , &eventId);
    pTChain->SetBranchAddress("UniqueId"         , &uniqueId);
    pTChain->SetBranchAddress("PdgCode"          , &pdg);
    pTChain->SetBranchAddress("IsNeutrinoInduced", &neutrinoInduced);
    pTChain->SetBranchAddress("HitUidList"       , &hitUidList);

    float startX, startY, startZ;
    float endX, endY, endZ;
    pTChain->SetBranchAddress("StartX", &startX);
    pTChain->SetBranchAddress("StartY", &startY);
    pTChain->SetBranchAddress("StartZ", &startZ);
    pTChain->SetBranchAddress("EndX", &endX);
    pTChain->SetBranchAddress("EndY", &endY);
    pTChain->SetBranchAddress("EndZ", &endZ);

    // Read the information
    pTChain->GetEntry(iEntry);
    pTChain->ResetBranchAddresses();

    bool isNeutrinoInduced = (neutrinoInduced == 1);

    // Check if this event already exists
    bool eventExists = false;
    int  eventIndex;
    for (unsigned int i=0; i<mcEventList.size(); i++){
        SimpleMCEvent &event = mcEventList[i];
        if (event.GetFileId() == fileId && event.GetEventId() == eventId) {
            eventExists = true;
            eventIndex  = i;
            break;
        }
    }
    if (!eventExists){
        SimpleMCEvent newEvent(fileId, eventId);
        mcEventList.push_back(newEvent);
        eventIndex = mcEventList.size() - 1;
    }
    SimpleMCEvent &thisEvent = mcEventList[eventIndex];

    std::vector<Identifier> hitList;
    for (unsigned int i=0; i<hitUidList->size(); i++){
        hitList.push_back(Identifier(fileId, eventId, hitUidList->at(i)));
    } 

    // Make the particle and add it to the event
    SimpleMCParticle thisMCParticle(Identifier(fileId, eventId, uniqueId), pdg, isNeutrinoInduced, hitList, startX, startY, startZ, endX, endY, endZ);
    thisEvent.AddMCParticle(thisMCParticle);

}




/**
 *  @brief  Associate pre and post hit information
 * 
 *  @param  preEventList event list pre cosmic hit removal
 *  @param  preEventList event list post cosmic hit removal
 *  @param  mergedEventList empty event list to be filled with merged data
 */
void MergeEvents(std::vector<SimpleMCEvent> &preEventList, std::vector<SimpleMCEvent> &postEventList, std::vector<SimpleMCEvent> &mergedEventList){
    // Do some sanity checks
    if (preEventList.size() != postEventList.size()){
        std::cerr << "Error: Then number of events do not match" << std::endl;
        std::cerr << "       Events pre cosmic hit removal  = " << preEventList.size() << std::endl;
        std::cerr << "       Events post cosmic hit removal = " << postEventList.size() << std::endl;
        exit(1);
    }

    unsigned int totalPreHits        = 0;
    unsigned int totalPostHits       = 0;
    unsigned int totalAssociatedHits = 0;
    unsigned int totalPfos           = 0;
    unsigned int totalAmbiguousPfos  = 0;
    double       summedPurity        = 0;

    // For each event
    for (unsigned int i=0; i<preEventList.size(); i++){
        SimpleMCEvent &preEvent  = preEventList[i];
        SimpleMCEvent postEvent  = postEventList[i];
        SimpleMCEvent mergedEvent(preEvent.GetFileId(), preEvent.GetEventId());
      
        std::vector<int> remainingHitId;
        // Associate hits pre and post hit removal
        int hitCount = 0;
        for (SimpleCaloHit &preHit : preEvent.GetCaloHitList()){
            bool hitIsRemoved = true;
            // Search for a matching hit in the post data
            for (SimpleCaloHit &postHit : postEvent.GetCaloHitList()){
                // If the hits match 
                if ((std::fabs(preHit.GetX()-postHit.GetX()) < std::numeric_limits<float>::min())  &&
                    (std::fabs(preHit.GetZ()-postHit.GetZ()) < std::numeric_limits<float>::min())  &&
                    (preHit.GetMCParticleId() == postHit.GetMCParticleId()) &&
                    (preHit.GetView()         == postHit.GetView())){

                    hitIsRemoved = false;
                    remainingHitId.push_back(preHit.GetId().GetUid());
                    break;
                }
            }
            // Add the hit to the merged hit data
            SimpleCaloHit thisHit(preHit.GetId(), preHit.GetView(), preHit.GetX(), preHit.GetZ(), hitIsRemoved, preHit.GetMCParticleId(), preHit.GetPfoId(), preHit.IsNeutrinoInduced());
            mergedEvent.AddCaloHit(thisHit);

            hitCount++;
        }

        totalPreHits        += preEvent.GetCaloHitList().size();
        totalPostHits       += postEvent.GetCaloHitList().size();
        totalAssociatedHits += remainingHitId.size();

        // Now check if the Pfos have been removed
        // Technically, either all or none of the pfo hits should be removed
        for (SimplePfo &pfo : preEvent.GetPfoList()){
            unsigned int nRemaining = 0;
            for (Identifier &hitId : pfo.GetHitList()){
                if ( std::find(remainingHitId.begin(), remainingHitId.end(), hitId.GetUid()) != remainingHitId.end() ){
                    // Hit remains
                    nRemaining++;
                }
            }
            bool pfoIsRemoved = false;
            if (nRemaining == 0){
                // Pfo has been removed
                pfoIsRemoved = true;
            }
            else if(nRemaining == pfo.GetHitList().size()){
                // All pfo hits remain
                pfoIsRemoved = false;
            }
            else{
                // Ambiguous if the pfo has been removed or not.
                // Since all pfo hits should be removed, if ANY remain, then the pfo remains.. 
                // So a semi-removal should be due to unassociated hits
                pfoIsRemoved = false;
                totalAmbiguousPfos++;
                summedPurity += ((double) nRemaining)/((double) pfo.GetHitList().size());
            }
        
            totalPfos++;

            double fractionRemoved = ((double) (pfo.GetHitList().size() - nRemaining)) / ((double) pfo.GetHitList().size());

            // Add the pfo to the merged data
            SimplePfo thisPfo(pfo.GetId(), pfo.GetHitList(), pfoIsRemoved, fractionRemoved);
            mergedEvent.AddPfo(thisPfo);
        }

        // Add the MCParticles
        for (SimpleMCParticle &part : preEvent.GetMCParticleList()){
            SimpleMCParticle thisMCParticle(part.GetId(), part.GetPdg(), part.IsNeutrinoInduced(), part.GetHitList(), part.GetStartX(), part.GetStartY(), part.GetStartZ(), part.GetEndX(), part.GetEndY(), part.GetEndZ());
            mergedEvent.AddMCParticle(thisMCParticle);
        } 

        // Add the merged event to the merged event list
        mergedEventList.push_back(mergedEvent);

        showLoadingBar(i+1, preEventList.size(), 30);
    }

    std::cout << std::endl;
    std::cout << "  Total pre hits       : " << totalPreHits  << std::endl;
    std::cout << "  Total post hits      : " << totalPostHits << std::endl;
    std::cout << "    Total unnasociated : " << (totalPostHits - totalAssociatedHits) << "  (" << ((100 * ((double) totalPostHits - (double) totalAssociatedHits)) / ((double) totalPostHits) )<< "%)" << std::endl;
    std::cout << "  Total pfos           : " << totalPfos     << std::endl;
    std::cout << "    Total ambigious    : " << totalAmbiguousPfos << "  (" << (100 * ((double) totalAmbiguousPfos / (double) totalPfos)) << "%)" << std::endl;
    std::cout << "    Average purity     : " << (100 * (summedPurity / (double) totalAmbiguousPfos)) << "%" << std::endl;

}

#endif // #ifndef MERGING_IO_H
