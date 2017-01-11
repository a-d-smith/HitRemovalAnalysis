# HitRemovalAnalysis

Repository of code for analysing the cosmic hit removal code in LArSoft using Pandora

--------------------------------------------------------------------------------------

This code broadly works in three steps

## Step 1 - Extract event information pre and post hit removal
For this you will need two `.pndr` files of exactly the same events.
- The first (pre file) should contain the `gaushit` collection, which is the hit collection **before** any cosmic hit removal has been done
- The second (post file) should contain the `PandoraCosmicKHitRemoval` collection, which is the hit collection **after** the cosmic hit removal has been done

On the pre file, you should run the **same** Pandora cosmic reconstruction pass that was done post file during its production. Then run the `PreRemovalAlgorithm` in Pandora to extract the relevant information from the file (MCParticles, CaloHits and PFOs)

On the post file, just run the `PostRemovalAlgorithm` in Pandora to extract the relevant information from the file (MCParticles, CaloHits)

## Step 2 - Merge the information from the pre and post files from Step 1
Run the pre and post ROOT files from Step 1, through the merging code.
This code looks at the post hit collection and attempts to match each hit to one in the pre hit collection.
In this way it can determine if a hit gets removed or not. 

## Step 3 - Count interesting quantities
Using the output from Step 2, in this step interesting quantities such as "number of neutrino hits in a given PFO" is counted, so plots can be made quickly


--------------------------------------------------------------------------------------
