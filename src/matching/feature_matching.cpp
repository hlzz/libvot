/** \file feature_matching.cpp
 *	\brief matching functions implementation
 *
 */
// Author: Tianwei Shen <shentianweipku@gmail.com>

#include "feature_matching.h"

#include <iostream>
#include "matching_datatypes.h"
#include "utils/data_types.h"
#include "utils/io_utils.h"

namespace vot {
bool PairwiseSiftMatching(SiftData &sift1, SiftData &sift2, SiftMatchPair &match_pair, MatchParam &match_param)
{
	// get putative match
	int num_feature1 = sift1.getFeatureNum();
	int num_feature2 = sift2.getFeatureNum();
	int max_feature = std::max(num_feature1, num_feature2) * 2;

	// using sift matcher is easy, just new an matcher object
	// set matching device and initialize the context
	SiftMatcher matcher(max_feature);
	matcher.SetMatchDevice(SiftMatcher::SIFT_MATCH_CPU);		// use cpu match
	if(!matcher.Init())				// initialize context
	{
		std::cerr << "[PairwiseSiftMatching] Initialize SiftMatcher context failed\n";
		return false;
	}
	if(!matcher.SetDescriptors(0, sift1.getFeatureNum(), sift1.getDesPointer()) ||
	   !matcher.SetDescriptors(1, sift2.getFeatureNum(), sift2.getDesPointer()))
	{
		std::cerr << "[PairwiseSiftMatching] Matcher set Descriptors failed\n";
		return false;
	}
	int match_buf[max_feature][2];
	int nmatch = matcher.GetSiftMatch(max_feature, match_buf);
	std::cout << "[PairwiseSiftMatching] Get " << nmatch << " matches\n";
	for(int i = 0; i < nmatch; i++)
		std::cout << match_buf[i][0] << " " << match_buf[i][1] << "\n";

	// geometric verification

	return true;
}
}	// end of namespace vot
