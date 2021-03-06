#include <iomanip>

#include "Pandora/AlgorithmHeaders.h"

#include "larpandoracontent/LArValidation/PostRemovalAlgorithm.h"

#include "larpandoracontent/LArHelpers/LArMCParticleHelper.h"
#include "larpandoracontent/LArHelpers/LArMonitoringHelper.h"
#include "larpandoracontent/LArHelpers/LArPfoHelper.h"

using namespace pandora;

namespace lar_content
{

PostRemovalAlgorithm::PostRemovalAlgorithm(){
    m_eventNumber = 0;
    m_outputMCParticleTreeName = "HitDataMCParticle"; 
    m_outputHitTreeName = "HitDataHit"; 
}

PostRemovalAlgorithm::~PostRemovalAlgorithm(){
    PANDORA_MONITORING_API(SaveTree(this->GetPandora(), m_outputHitTreeName.c_str(), m_outputFileName.c_str(), "UPDATE"));
    PANDORA_MONITORING_API(SaveTree(this->GetPandora(), m_outputMCParticleTreeName.c_str(), m_outputFileName.c_str(), "UPDATE"));
    std::cout << "File written " << m_outputFileName << std::endl;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PostRemovalAlgorithm::Run()
{
    // Get the input collections 
    // ----------------------------------------------------------------------------------
    
    // Get the list of MCParticles
    const MCParticleList *pMCParticleList = nullptr;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetList(*this, m_mcParticleListName, pMCParticleList));

    // Get the list of CaloHits
    const CaloHitList *pCaloHitList = nullptr;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetList(*this, m_caloHitListName, pCaloHitList));
    
    // Run the Algorithm
    // ----------------------------------------------------------------------------------
   
    // Get the list of all CaloHits and assign them Uids
    std::map<size_t, const CaloHit *const> uidHitMap;
    size_t maxHitId = 0;
    for (const CaloHit *const &pCaloHit : *pCaloHitList){ 
        uidHitMap.insert( std::pair<size_t, const CaloHit *const>(maxHitId, pCaloHit) );
        maxHitId++;
    }

    // A map between hit and mcparticle uids
    std::map<size_t, size_t> hitParticleMap;

    // Get information on each hit
    std::map<size_t, const CaloHit *const>::iterator it;
    for (it = uidHitMap.begin(); it != uidHitMap.end(); ++it){
        // Get the Uid
        size_t uid = it->first;

        // View
        HitType view = it->second->GetHitType();
        int viewInt;
        switch (view){
            case TPC_VIEW_W:
                viewInt = 0;
                break;
            case TPC_VIEW_U:
                viewInt = 1;
                break;
            case TPC_VIEW_V:
                viewInt = 2;
                break;
            default:
                viewInt = -1;
        }

        // Position
        float x = it->second->GetPositionVector().GetX();
        float z = it->second->GetPositionVector().GetZ();

        // Find the associated MCParticle (if exists)
        const MCParticleWeightMap &hitMCParticleWeightMap(it->second->GetMCParticleWeightMap());
        const MCParticle* mcParticle = nullptr;

        // Find the MCParticle with the largest weight
        float bestWeight(0.f);
        for (const MCParticleWeightMap::value_type &mapEntry : hitMCParticleWeightMap) {
            if (mapEntry.second > bestWeight) {
                bestWeight = mapEntry.second;
                mcParticle = mapEntry.first;
            }
        }
        
        // Get the Uid for the associated particle
        size_t mcParticleUid = -1;
        if (mcParticle){
            // An associated MCParticle exists -> Not a ghost hit
            mcParticleUid = (size_t)(mcParticle->GetUid());
        }
        hitParticleMap.insert(std::pair<size_t, size_t>(uid, mcParticleUid));

        // Output this information to a tree
        // --------------------------------------------------
        // Identifier
        PANDORA_MONITORING_API(SetTreeVariable(this->GetPandora(), m_outputHitTreeName.c_str(), "FileId"      , m_fileId));
        PANDORA_MONITORING_API(SetTreeVariable(this->GetPandora(), m_outputHitTreeName.c_str(), "EventId"     , m_eventNumber));
        PANDORA_MONITORING_API(SetTreeVariable(this->GetPandora(), m_outputHitTreeName.c_str(), "UniqueId"    , (int) uid));
        // Associated particle
        PANDORA_MONITORING_API(SetTreeVariable(this->GetPandora(), m_outputHitTreeName.c_str(), "MCParticleId", (int) mcParticleUid));
        // Positional info
        PANDORA_MONITORING_API(SetTreeVariable(this->GetPandora(), m_outputHitTreeName.c_str(), "View"        , viewInt));
        PANDORA_MONITORING_API(SetTreeVariable(this->GetPandora(), m_outputHitTreeName.c_str(), "X"           , x));
        PANDORA_MONITORING_API(SetTreeVariable(this->GetPandora(), m_outputHitTreeName.c_str(), "Z"           , z));

        PANDORA_MONITORING_API(FillTree(this->GetPandora(), m_outputHitTreeName.c_str()));
    
    }


    // Get a list of all of the MCParticles 
    for (const MCParticle *const &pMCParticle : *pMCParticleList){
        // Identification
        size_t uid = (size_t) (pMCParticle->GetUid());

        // Does this particle come from a neutrino?
        bool isNeutrinoInduced = false;
        if (LArMCParticleHelper::IsNeutrinoInduced(pMCParticle)){
            isNeutrinoInduced = true;
        }
        
        // PDG code
        int pdg = pMCParticle->GetParticleId();

        /*
        // Uid of the primary particle
        size_t primaryUid;
        if (LArMCParticleHelper::IsPrimary(pMCParticle)){
            primaryUid = uid;
        }
        else{
            primaryUid = (size_t) (LArMCParticleHelper::GetPrimaryMCParticle(pMCParticle)->GetUid());
        }
        */

        // List of associated Hits
        std::vector<int> *MCParticleHitList = new std::vector<int>;
        std::map<size_t, size_t>::iterator itMC;
        for (itMC = hitParticleMap.begin(); itMC != hitParticleMap.end(); ++itMC){
            if (itMC->second == uid){
                MCParticleHitList->push_back((int) (itMC->first));
            }
        }

        // Start position
        const CartesianVector StartVtx = pMCParticle->GetVertex();
        float startX = StartVtx.GetX();
        float startY = StartVtx.GetY();
        float startZ = StartVtx.GetZ();

        // End position
        const CartesianVector EndVtx = pMCParticle->GetEndpoint();
        float endX = EndVtx.GetX();
        float endY = EndVtx.GetY();
        float endZ = EndVtx.GetZ();

        // Output this information to a tree
        // --------------------------------------------------
        // Identifier
        PANDORA_MONITORING_API(SetTreeVariable(this->GetPandora(), m_outputMCParticleTreeName.c_str(), "FileId"           , m_fileId));
        PANDORA_MONITORING_API(SetTreeVariable(this->GetPandora(), m_outputMCParticleTreeName.c_str(), "EventId"          , m_eventNumber));
        PANDORA_MONITORING_API(SetTreeVariable(this->GetPandora(), m_outputMCParticleTreeName.c_str(), "UniqueId"         , (int) uid));
        // Metadata
        PANDORA_MONITORING_API(SetTreeVariable(this->GetPandora(), m_outputMCParticleTreeName.c_str(), "PdgCode"          , pdg));
        PANDORA_MONITORING_API(SetTreeVariable(this->GetPandora(), m_outputMCParticleTreeName.c_str(), "IsNeutrinoInduced", (isNeutrinoInduced ? 1 : 0)));
        // Associated Primary Uid
        // PANDORA_MONITORING_API(SetTreeVariable(this->GetPandora(), m_outputMCParticleTreeName.c_str(), "PrimaryUid"       , (int) primaryUid));
        // Associated Hit List
        PANDORA_MONITORING_API(SetTreeVariable(this->GetPandora(), m_outputMCParticleTreeName.c_str(), "HitUidList"       , MCParticleHitList));
        // Positional information
        PANDORA_MONITORING_API(SetTreeVariable(this->GetPandora(), m_outputMCParticleTreeName.c_str(), "StartX"           , startX));
        PANDORA_MONITORING_API(SetTreeVariable(this->GetPandora(), m_outputMCParticleTreeName.c_str(), "StartY"           , startY));
        PANDORA_MONITORING_API(SetTreeVariable(this->GetPandora(), m_outputMCParticleTreeName.c_str(), "StartZ"           , startZ));
        PANDORA_MONITORING_API(SetTreeVariable(this->GetPandora(), m_outputMCParticleTreeName.c_str(), "EndX"             , endX));
        PANDORA_MONITORING_API(SetTreeVariable(this->GetPandora(), m_outputMCParticleTreeName.c_str(), "EndY"             , endY));
        PANDORA_MONITORING_API(SetTreeVariable(this->GetPandora(), m_outputMCParticleTreeName.c_str(), "EndZ"             , endZ));

        PANDORA_MONITORING_API(FillTree(this->GetPandora(), m_outputMCParticleTreeName.c_str()));

        delete MCParticleHitList;
    }

    m_eventNumber++;
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------
StatusCode PostRemovalAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    // Read settings from xml file here
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "MCParticleListName", m_mcParticleListName ));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "CaloHitListName"   , m_caloHitListName    ));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "FileId"            , m_fileId             ));

    m_outputFileName = "HitData_POST_" + std::to_string(m_fileId) + ".root";

    return STATUS_CODE_SUCCESS;
}

}// namespace lar_content
