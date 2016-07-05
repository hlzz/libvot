/** \file opencv_matching_api.h
 *	\brief libvot-side APIs of opencv matching utilities
 *
 * 	It contains some matching functions for the use of opencv in libvot
 */
// Author: Tianwei Shen <shentianweipku@gmail.com>

#ifndef VOT_OPENCV_MATCHING_API_H
#define VOT_OPENCV_MATCHING_API_H
#include "matching_datatypes.h"

#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/imgproc/imgproc.hpp>

namespace vot {
class SiftMatcherOpencv: public SiftMatcher
{
public:
	SiftMatcherOpencv(int max_sift);
	int GetSiftMatch(int max_match, int match_buffer[][2], int mutual_best_match);
	bool SetDescriptors(int index, int num, const unsigned char *descriptors);
	bool SetDescriptors(int index, int num, const float *descriptors);
private:
	cv::Mat des_mat_[2];
};

} // end of namespace vot

#endif	// VOT_OPENCV_MATCHING_API_H
