#ifndef MERGING_H
#define MERGING_H 1

/**
 *  @brief  Merging - Main entry point for analysis
 * 
 *  @param  preRegex the regular expression identifying the input root files for pre hit removal data
 *  @param  postRegex the regular expression identifying the input root files for post hit removal data
 *  @param  skipEvents the number of events to skip
 *  @param  nEventsToProcess the number of events to process
 *  @param  outputFileName root file name to which to write output
 */
void Validation(const std::string &preRegex, const std::string &postRegex, const std::string outputFileName);

#endif // #ifndef MERGING_H
