/** \file opencv_matching_api.cpp
 *	\brief libvot-side APIs of opencv matching utilities
 *
 * 	It contains some matching functions for the use of opencv in libvot
 */
// Author: Tianwei Shen <shentianweipku@gmail.com>

#include "opencv_matching_api.h"

#include "utils/global_params.h"
#include "utils/data_types.h"

namespace vot {
// ====================================================
// ---------------- SiftMatcherOpencv class ---------------
// ====================================================
SiftMatcherOpencv::SiftMatcherOpencv(int max_sift): SiftMatcher(max_sift) {}

bool SiftMatcherOpencv::SetDescriptors(int index, int num, const unsigned char *descriptors)
{
	if(index != 0 && index != 1)
	{
		std::cerr << "[SiftMatcherOpencv] SetDescriptors get index other than 0/1\n";
		return false;
	}
	if(num > max_sift_)
		num = max_sift_;
	num_sift_[index] = num;

	// fill des_mat_
	des_mat_[index].create(num, feature_dim_, CV_32FC1);
	for(int i = 0; i < num; i++)
	{
		for(int j = 0; j < feature_dim_; j++)
		{
			des_mat_[index].at<float>(i,j) = static_cast<float>(descriptors[i*feature_dim_+ j]);
		}
	}

	return true;
}

bool SiftMatcherOpencv::SetDescriptors(int index, int num, const float *descriptors)
{
	if(index != 0 || index != 1)
	{
		std::cerr << "[SiftMatcherCPU] SetDescriptors get index other than 0/1\n";
		return false;
	}
	if(num > max_sift_)
		num = max_sift_;
	num_sift_[index] = num;

	// fill des_mat_ and convert float
	des_mat_[index].create(num, feature_dim_, CV_32FC1);
	for(int i = 0; i < num; i++)
	{
		for(int j = 0; j < feature_dim_; j++)
		{
			des_mat_[index].at<float>(i,j) = static_cast<float>(512 * descriptors[i*feature_dim_+ j] + 0.5);
		}
	}

	return true;
}

// flann-based opencv matcher
int SiftMatcherOpencv::GetSiftMatch(int max_match,
                                    int match_buffer[][2],
								    int mutual_best_match)
{
	cv::FlannBasedMatcher matcher;
	std::vector<cv::DMatch> matches;
	matcher.match(des_mat_[0], des_mat_[1], matches);

	double max_dist = 0; double min_dist = 100;

	//-- Quick calculation of max and min distances between keypoints
	for (int i = 0; i < des_mat_[0].rows; i++)
	{
		double dist = matches[i].distance;
		if( dist < min_dist ) min_dist = dist;
		if( dist > max_dist ) max_dist = dist;
	}

	printf("-- Max dist : %f \n", max_dist);
	printf("-- Min dist : %f \n", min_dist);
	std::cout << "Matches size: " << matches.size() << std::endl;

	//-- Draw only "good" matches (i.e. whose distance is less than 5*min_dist,
	//-- or a small arbitary value ( 0.02 ) in the event that min_dist is very
	//-- small)

	int match_num = 0;
	for(int i = 0; i < des_mat_[0].rows && match_num < max_match; i++ )
	{
		if(matches[i].distance <= std::max(5*min_dist, 0.02) )
		{
			match_buffer[match_num][0] = matches[i].queryIdx;
			match_buffer[match_num][1] = matches[i].trainIdx;
			match_num++;
		}
	}

	return match_num;
}

} 	// end of namespace vot
