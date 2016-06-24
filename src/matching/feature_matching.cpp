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

	SiftMatcher matcher(max_feature);

	// geometric verification

	return true;
}
}	// end of namespace vot
