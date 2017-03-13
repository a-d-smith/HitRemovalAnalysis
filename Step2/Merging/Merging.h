#ifndef MERGING_H
#define MERGING_H 1

#include <limits>

/**
 *  @brief  Merge - Main entry point for analysis
 * 
 *  @param  preRegex the regular expression identifying the input root files for pre hit removal data
 *  @param  postRegex the regular expression identifying the input root files for post hit removal data
 *  @param  outputFileName root file name to which to write output
 *  @param  nGoodEvents the number of good events to process
 */
void Merge(const std::string &preRegex, const std::string &postRegex, const std::string outputFileName, int nGoodEvents = std::numeric_limits<int>::max());

#endif // #ifndef MERGING_H
