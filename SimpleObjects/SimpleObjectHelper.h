#include "SimpleObjects.h"

bool IsGhostHit(SimpleCaloHit &hit, SimpleMCEvent &event){
    return (hit.GetMCParticleId().GetUid() == -1);
}

int GetIntView(char view){
    switch (view){
        case 'W':
            return 0;
            break;
        case 'U':
            return 1;
            break;
        case 'V':
            return 2;
            break;
        default:
            return -1;
    }
}
