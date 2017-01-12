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


