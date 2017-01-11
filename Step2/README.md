# HitRemovalAnalysis - Step 2

In this step we will take in PRE and POST .root files from Step 1, and merge the data from both.
The macro Merging.C is the code that does this, it loads all of the CaloHits, PFOs and MCParticles from both files and then trys to associate post-hits to pre-hits. From this information it can determine if a hit has been removed or not! 
