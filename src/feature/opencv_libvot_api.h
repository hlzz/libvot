// Some utility functions for the use of opencv in libvot
#ifndef OPENCV_LIBVOT_API_H
#define OPENCV_LIBVOT_API_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/nonfree/features2d.hpp>

namespace tw  {
class SiftData;

bool OpencvKeyPoints2libvotSift(std::vector<cv::KeyPoint> &key_points,
                                tw::SiftData &sift_data);

}	// end of namespace tw

#endif
