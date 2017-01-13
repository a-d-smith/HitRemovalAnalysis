# HitRemovalAnalysis - Step 2

In this step we will take in PRE and POST .root files from Step 1, and merge the data from both.
The macro Merging.C is the code that does this, it loads all of the CaloHits, PFOs and MCParticles from both files and then trys to associate post-hits to pre-hits. From this information it can determine if a hit has been removed or not! 

----------------------------------------------------------------------------------------------------

## Running the code by hand

To run the code by hand start root, and compile `Merging.C` by typing
```
.L Merging.C+
```
Then still inside root run the code by calling the function `Merge`. This function has the following parameters
- `const string& preRegex` A regular expression for your PRE files produced in Stage 1
- `const string& postRegex` A regular expression for you POST files produced in Stage 2 (done so that the file numbers line up with the PRE regex)
- `const string outputFileName` A file name for your output (.root)
- `int nGoodEvents` Number of events to process (a good events is one that contains at least one neutrino hit)

### Using a helper script
There is a helper script called `RunJob.sh` that takes the following arguments
1. Pre file  - path to the output pre .root file from stage 1
2. Post file - path to the output post .root file from stage 1
3. Output file (include .root) - path of the merged output
4. Root setup script - path to a script which enables the use of ROOT
5. Number of good events (at least one neutrino hit) - Optional

----------------------------------------------------------------------------------------------------

## Running the code using condor

There is a helper script called `RunBatch.sh` that takes the following arguments
1. Pre file regex (use quotes) - a regular expression (in " ") for the pre files from stage 1
2. Post file regex (use quotes) - a regular expression (in " ") for the pre files from stage 2
3. Results directory - path of the merged ouput directory
4. ROOT setup script - path to a script which enables the use of ROOT


