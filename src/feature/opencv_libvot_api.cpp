#include "opencv_libvot_api.h"

#include "utils/global_params.h"
#include "utils/data_types.h"

namespace tw {
bool OpencvKeyPoints2libvotSift(std::vector<cv::KeyPoint> &key_points,
                                cv::Mat &descriptors,
                                tw::SiftData &sift_data)
{
	int num_features = key_points.size();
	sift_data.setFeatureNum(num_features);
	DTYPE *&dp = sift_data.getDesPointer();
	LTYPE *&lp = sift_data.getLocPointer();

	if(dp != NULL)
		delete [] dp;
	if(lp != NULL)
		delete [] lp;

	int des_dim = sift_data.getDesDim();
	int loc_dim = sift_data.getLocDim();
	dp = new DTYPE [num_features * des_dim];
	lp = new LTYPE [num_features * loc_dim];

	for(int i = 0; i < num_features; i++)
	{
		cv::KeyPoint &kp = key_points[i];
		lp[i*loc_dim + 0] = kp.pt.x;	// x coordinate
		lp[i*loc_dim + 1] = kp.pt.y;	// y coordinate
		lp[i*loc_dim + 2] = 0;			// color (not available)
		lp[i*loc_dim + 3] = kp.size;	// scale
		lp[i*loc_dim + 4] = kp.angle;	// orientation

		// save descriptors
		DTYPE *desc = descriptors.data;
		for(int j = 0; j < des_dim; j++)
		{
			dp[i * des_dim + j] = static_cast<DTYPE>(desc[i * des_dim + j]);
		}
	}

	return true;
}

} 	// end of namespace tw
