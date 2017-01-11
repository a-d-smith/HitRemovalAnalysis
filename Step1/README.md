# HitRemovalAnalysis - Step 1

## Setting up
You will need to obtain a version of Pandora from github, see the build instructions [here](https://github.com/PandoraPFA/PandoraPFA/blob/master/README.md). Make sure you get all of the repositories.

Inside `LArContent/larpandoracontent`, you should add the directory `LArValidation` which is included in *this* directory. You will need to make some modifications to:
- `MakeFile` to include the `LArValidation` source code
```bash
SOURCES += $(wildcard $(PROJECT_DIR)/larpandoracontent/LArValidation/*.cc)
```
- `larpandoracontent/CMakeLists.txt` to add the `LArValidation` subdirectory
```
LArValidation
```
- `larpandoracontent/LArContent.h` to include the header files from `LArValidation`
```c++
#include "larpandoracontent/LArValidation/PreRemovalAlgorithm.h"
#include "larpandoracontent/LArValidation/PostRemovalAlgorithm.h"
```
```c++
d("LArPreRemoval",                          lar_content::PreRemovalAlgorithm::Factory)                                  \
d("LArPostRemoval",                         lar_content::PostRemovalAlgorithm::Factory)                                 \
```

Don't forget to rebuild after making these changes!

------------------------------------------------------------------------------------------------------------

## Getting Samples

Before you can run this code, you will need two types of `.pndr` files.
1. To make the first type of file generate samples up to the signal processing step to obtain a `gaushit` collection.
   Then run this through PandoraWriter to get a pndr file.
   - We will call this a "pre" file
2. To make the second type of file run the **same** samples used in the first file through PandoraCosmic (@sequence::microboone_reco_mcc7_pandoraCosmic), 
   and then again through PandoraWriter to get a pndr file.
   - We will call this a "post" file

------------------------------------------------------------------------------------------------------------

## Running this code by hand

To run the code you will need to give Pandora a settings file.

### LArPreRemoval settings
```xml
<algorithm type = "LArPreRemoval">
    <MCParticleListName>MCParticleList3D</MCParticleListName>
    <CaloHitListName>CaloHitList2D</CaloHitListName>
    <PfoListName>Particles3D</PfoListName>
    <FileId>FILE_ID</FileId>
</algorithm>
```

### LArPreRemoval settings
```xml
<algorithm type = "LArPostRemoval">
    <MCParticleListName>MCParticleList3D</MCParticleListName>
    <CaloHitListName>CaloHitList2D</CaloHitListName>
    <FileId>FILE_ID</FileId>
</algorithm>
```

`FILE_ID` is just a placeholder for a positive integer that you have to insert. You **must** use the same `FileId` for the pre and post files which contain the same events.

Example settings files can be found in the `Settings` directory, but you must ensure that the settings you use before LArPreRemoval match what you used for the PandoraCosmic pass inside LArSoft. The settings given in this repository match PandoraCosmic v03_00_00 (See the Pandora github for more details on which settings to use)

------------------------------------------------------------------------------------------------------------

## Running this code automatically

If you use the same file structure as I do for storing my `.pndr` files, then you can use scripts which automate the running of this code for many files. 
