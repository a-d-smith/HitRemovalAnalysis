# HitRemovalAnalysis - Step 3

In this step we take the merged data root files from step 2, and count useful numbers from which we can make figures!

-----------------------------------------------------------------------------------

## Running the code by hand

```
root -l
.L LoadMergedData.C+
.x makeNtuples.C("my_data_files_*.root", <outputFileId>)
```
