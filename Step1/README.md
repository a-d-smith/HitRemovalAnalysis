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

### Generating settings files using the examples

There is a helper script called `GetSettings.sh` which takes the following arguments

1. pre / post  - if this should be settings for a pre or post .pndr file
2. input file  - path to the input .pndr file
3. file id     - integer corresponding to the ID of that file
4. output file - a .xml file path for the generated settings file

### Running the code

There is a helper script called `SubmitJob.sh` which takes the following arguments

1. Settings xml files       - path to the settings file (see previous section)
2. Number of events         - number of events to process in the .pndr file
3. Output directory         - path to a directory to store the output (file name will be automatically generated based on ID in settings)
4. Setup script             - path to a script you use to set up before running Pandora
5. PandoraInterface binary  - path to the Pandora Interface binary (usually LArReco/bin/PandoraInterface)


------------------------------------------------------------------------------------------------------------

## Running this code automatically

There is a helper script called `SubmitBatch.sh` which will use condor to submit multiple jobs simultaneously, it takes the following arguments

1. pre file event list      - path to a list of pre .pndr files and the number of events to process in each.
2. post file event list     -
    - The files indicated in arguments 1 & 2 should have a line per file, and each line should have the following format (space delimited two column text file)
    ```
    <path to .pndr file> <number of events>
    ```
3. output directory         - path to a directory to store the output (file names will be automatically generated based on .pndr file name) 
4. setup script             - path to a script you use to set up before running Pandora
5. PandoraInterface binary  - path to the Pandora Interface binary (usually LArReco/bin/PandoraInterface)

### Imporant note
This script assumes that your .pndr files end with `-<ID>.pndr`, where `<ID>` is an integer, and will use this to scrape file IDs. 
So, you must ensure that your .pndr file event lists contain files of this format (where file IDs match 1:1 in the pre and post event list), AND they must have the same ordering of IDs.
