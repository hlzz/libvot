// Tianwei Shen, HKUST.
// extract vlfeat sift feature and save them in libvot's format

#include <iostream>
#include <vector>
#include <gflags/gflags.h>

// libvot includes
#include "libvot_config.h"
#include "utils/io_utils.h"
#include "utils/global_params.h"
#include "utils/data_types.h"
#include "feature/opencv_libvot_api.h"
#include "feature/vlfeat_libvot_api.h"

extern "C" {
#include <vl/generic.h>
#include <vl/sift.h>
}

#ifdef LIBVOT_USE_OPENCV
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/nonfree/features2d.hpp>
#else
#endif

using namespace std;

DEFINE_string(output_folder, "", "feature output folder, the same as the input folder if not specified");
DEFINE_int32(feature_type, 0, "feature type, 0 for opencv sift, 1 for vlfeat sift");
DEFINE_double(edge_thresh, 10, "edge threshold for vlfeat parameter");
DEFINE_double(peak_thresh, 2.5, "peak threshold for vlfeat parameter");

int main(int argc, char** argv)
{
	char c_program_usage[50], c_program_version[20];
	sprintf(c_program_usage, "Usage: %s <image_list>", argv[0]);
	sprintf(c_program_version, "%d.%d.%d", LIBVOT_VERSION_MAJOR, LIBVOT_VERSION_MINOR, LIBVOT_VERSION_PATCH);
	string program_usage(c_program_usage), program_version(c_program_version);
	gflags::SetVersionString(program_version);
	gflags::SetUsageMessage(program_usage);		// this has to appear before ParseCommandLineFlags
	gflags::ParseCommandLineFlags(&argc, &argv, true);

	if(argc != 2)
	{
		cout << "Input argument error, use '--help' to see the usage" << endl;
		cout << program_usage << endl;
		return -1;
	}

	const char *c_image_list = argv[1];
	vector<string> image_filenames, feat_filenames;
	tw::IO::ExtractLines(c_image_list, image_filenames);
	int num_images = image_filenames.size();
	cout << "[Extract Feature] " <<  num_images << " input images\n";
	feat_filenames.resize(num_images);

	// process output_folder flags
	if(FLAGS_output_folder != "")
	{
		cout << "[Extract Feature] Output folder: " << FLAGS_output_folder << endl;
		tw::IO::Mkdir(FLAGS_output_folder);
		for(int i = 0; i < num_images; i++)
		{
			feat_filenames[i] = tw::IO::SplitPath(image_filenames[i]).second;
			feat_filenames[i] = tw::IO::SplitPathExt(feat_filenames[i]).first + ".sift";
			feat_filenames[i] = tw::IO::JoinPath(FLAGS_output_folder, feat_filenames[i]);
		}
	}
	else
	{
		cout << "[Extract Feature] Output folder: output to the same folder as the input files.\n";
		for(int i = 0; i < num_images; i++)
			feat_filenames[i] = tw::IO::SplitPathExt(image_filenames[i]).first + ".sift";
	}

#ifdef LIBVOT_USE_OPENCV
	switch (FLAGS_feature_type)
	{
		case LIBVOT_FEATURE_TYPE::OPENCV_SIFT:
		{
			cout << "[Extract Feature] Compute SIFT features using opencv sift\n";
			cv::SiftDescriptorExtractor cv_sift_detector;
			for(int i = 0; i < num_images; i++)
			{
				// load the image in BGR format
				const cv::Mat input = cv::imread(image_filenames[i], CV_LOAD_IMAGE_COLOR);

				std::vector<cv::KeyPoint> cv_keypoints;
				cv::Mat sift_descriptors;
				cv_sift_detector.detect(input, cv_keypoints);
				cv_sift_detector.compute(input, cv_keypoints, sift_descriptors);
				tw::SiftData sift_data;
				tw::OpencvKeyPoints2libvotSift(cv_keypoints, sift_descriptors, sift_data);

				sift_data.SaveSiftFile(feat_filenames[i]);
				cout << "[Extract Feature] Save sift data (" << sift_data.getFeatureNum()
				     << " features) to " <<  feat_filenames[i] << "\n";
			}
			break;
		}
		case LIBVOT_FEATURE_TYPE::VLFEAT_SIFT:
		{
			cout << "[Extract Feature] Compute SIFT features using vlfeat sift\n";
			tw::VlFeatParam vlfeat_param;
			vlfeat_param.edge_thresh = FLAGS_edge_thresh;
			vlfeat_param.peak_thresh = FLAGS_peak_thresh;
			for(int i = 0; i < num_images; i++)
			{
				const cv::Mat input = cv::imread(image_filenames[i], CV_LOAD_IMAGE_COLOR);
				tw::SiftData sift_data;
				int num_features = tw::RunVlFeature(input.data, input.cols, input.rows, 3, sift_data, vlfeat_param);
				sift_data.SaveSiftFile(feat_filenames[i]);
				cout << "[Extract Feature] Save sift data (" << num_features
				     << " features) to " <<  feat_filenames[i] << "\n";
			}

			break;
		}
		default:
		{
			cout << "Feature type not supported\n";
			return -1;
		}
	}
#else
	cout << "[Extract Feature] Currently we use opencv to read images, no opencv support found.\n";
	return -1;
#endif

	gflags::ShutDownCommandLineFlags();
	return 0;
}
