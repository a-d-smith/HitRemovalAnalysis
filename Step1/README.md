# HitRemovalAnalysis - Step 1

You will need to obtain a version of Pandora from github, see the build instructions [here](https://github.com/PandoraPFA/PandoraPFA/blob/master/README.md). Make sure you get all of the repositories.

Inside `LArContent/larpandoracontent`, you should add the directory `LArValidation` which is included in *this* directory. You will need to make some modifications to:
- `MakeFile` to include the `LArValidation` source code
- `larpandoracontent/CMakeLists.txt` to add the `LArValidation` subdirectory
- `larpandoracontent/LArContent.h` to include the header files from `LArValidation`
