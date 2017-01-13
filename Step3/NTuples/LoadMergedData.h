#ifndef LOAD_MERGED_DATA_H
#define LOAD_MERGED_DATA_H 1

#include <cmath>
#include <iostream>
#include <fstream>
#include <limits>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "../../SimpleObjects/SimpleObjects.h"

int min(int a, int b){
    return ((a < b) ? a : b);
}

void showLoadingBar(int i, int tot, int width){
    int N = min(width, std::floor(width * (((double) i) / ((double) tot))));
    std::cout << "\r  |" << std::string(N, '=') << "|";
    std::fflush(stdout);
}

/**
 *  @brief  Read a hit from the chain
 * 
 *  @param  pTChain the address of the chain
 *  @param  iEntry the first chain entry to read
 *  @param  mcEventList the event list to be populated
 */
void ReadNextHit(TChain *const pTChain, const unsigned int iEntry, std::vector<SimpleMCEvent> & mcEventList){
    // Get the hit information
    int fileId, eventId, uniqueId, mcParticleId, pfoId;
    bool isRemoved, isNeutrinoInduced;
    char view;
    double x, z;
    pTChain->SetBranchAddress("FileId"           , &fileId);
    pTChain->SetBranchAddress("EventId"          , &eventId);
    pTChain->SetBranchAddress("UniqueId"         , &uniqueId);
    pTChain->SetBranchAddress("IsRemoved"        , &isRemoved);
    pTChain->SetBranchAddress("IsNeutrinoInduced", &isNeutrinoInduced);
    pTChain->SetBranchAddress("MCParticleUid"    , &mcParticleId);
    pTChain->SetBranchAddress("PfoUid"           , &pfoId);
    pTChain->SetBranchAddress("View"             , &view);
    pTChain->SetBranchAddress("X"                , &x);
    pTChain->SetBranchAddress("Z"                , &z);

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

    // Make the hit and add it to the event
    SimpleCaloHit thisHit(Identifier(fileId, eventId, uniqueId), view, x, z, isRemoved, Identifier(fileId, eventId, mcParticleId), Identifier(fileId, eventId, pfoId), isNeutrinoInduced);
    thisEvent.AddCaloHit(thisHit);
}



/**
 *  @brief  Read a pfo from the chain
 * 
 *  @param  pTChain the address of the chain
 *  @param  iEntry the first chain entry to read
 *  @param  mcEventList the event list to be populated
 */
void ReadNextPfo(TChain *const pTChain, const unsigned int iEntry, std::vector<SimpleMCEvent> & mcEventList){
    // Get the hit information
    int fileId, eventId, uniqueId;
    std::vector<int> *hitUidList(NULL);
    bool isRemoved;
    double fractionRemoved;
    pTChain->SetBranchAddress("FileId"         , &fileId);
    pTChain->SetBranchAddress("EventId"        , &eventId);
    pTChain->SetBranchAddress("UniqueId"       , &uniqueId);
    pTChain->SetBranchAddress("IsRemoved"      , &isRemoved);
    pTChain->SetBranchAddress("FractionRemoved", &fractionRemoved);
    pTChain->SetBranchAddress("HitList"        , &hitUidList);

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

    // Make the hit and add it to the event
    SimplePfo thisPfo(Identifier(fileId, eventId, uniqueId), hitList, isRemoved, fractionRemoved);
    thisEvent.AddPfo(thisPfo);

}


/**
 *  @brief  Read an MCParticle from the chain
 * 
 *  @param  pTChain the address of the chain
 *  @param  iEntry the first chain entry to read
 *  @param  mcEventList the event list to be populated
 */
void ReadNextMCParticle(TChain *const pTChain, const unsigned int iEntry, std::vector<SimpleMCEvent> & mcEventList){
    // Get the hit information
    int fileId, eventId, uniqueId, pdg;
    bool isNeutrinoInduced;
    std::vector<int> *hitUidList(NULL);
    pTChain->SetBranchAddress("FileId"           , &fileId);
    pTChain->SetBranchAddress("EventId"          , &eventId);
    pTChain->SetBranchAddress("UniqueId"         , &uniqueId);
    pTChain->SetBranchAddress("PdgCode"          , &pdg);
    pTChain->SetBranchAddress("IsNeutrinoInduced", &isNeutrinoInduced);
    pTChain->SetBranchAddress("HitList"          , &hitUidList);

    double startX, startY, startZ;
    double endX, endY, endZ;
    pTChain->SetBranchAddress("StartX", &startX);
    pTChain->SetBranchAddress("StartY", &startY);
    pTChain->SetBranchAddress("StartZ", &startZ);
    pTChain->SetBranchAddress("EndX", &endX);
    pTChain->SetBranchAddress("EndY", &endY);
    pTChain->SetBranchAddress("EndZ", &endZ);

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

    // Make the hit and add it to the event
    SimpleMCParticle thisMCParticle(Identifier(fileId, eventId, uniqueId), pdg, isNeutrinoInduced, hitList, startX, startY, startZ, endX, endY, endZ);
    thisEvent.AddMCParticle(thisMCParticle);

}

#endif // #ifndef LOAD_MERGED_DATA_H
