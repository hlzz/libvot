// Tianwei Shen, HKUST.
// extract vlfeat sift feature and save them in libvot's format

/** \file libvot_feature.cpp
 *	\brief libvot feature generater (exe)
 */

#include <iostream>
#include <vector>
#include <thread>
#include <mutex>

#include "gflags/gflags.h"

// libvot includes
#include "libvot_config.h"
#include "utils/io_utils.h"
#include "utils/global_params.h"
#include "utils/data_types.h"
#ifdef LIBVOT_USE_OPENCV
#include "feature/opencv_libvot_api.h"
#endif //LIBVOT_USE_OPENCV
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
DEFINE_int32(thread_num, -1, "thread num");
DEFINE_double(edge_thresh, 10, "edge threshold for vlfeat parameter");
DEFINE_double(peak_thresh, 2.5, "peak threshold for vlfeat parameter");

void MultiVlfeatSiftExtract(std::vector<std::string> *image_filenames,
                            std::vector<std::string> *feat_filenames,
                            vot::VlFeatParam *vlfeat_param,
                            int first_index,
                            int num_images,
                            std::mutex *cout_mutex)
{
#ifdef LIBVOT_USE_OPENCV
	for (size_t i = first_index; i < first_index + num_images; i++) {
		const cv::Mat input = cv::imread((*image_filenames)[i], CV_LOAD_IMAGE_COLOR);
		vot::SiftData sift_data;
		int num_features = vot::RunVlFeature(input.data, input.cols, input.rows, 3, sift_data, *vlfeat_param);
		if (!sift_data.SaveSiftFile((*feat_filenames)[i])) {
			cout_mutex->lock();
			cerr << "[Extract Feature] sift_data.SaveSiftFile error.\n";
			cout_mutex->unlock();
		}

		cout_mutex->lock();
		cout << "[Extract Feature] Save sift data (" << num_features
		     << " features) to " <<  (*feat_filenames)[i] << " (" << i << "/" << image_filenames->size() << ")\n";
		cout_mutex->unlock();
	}
#else
	cout << "[MultiVlfeatSiftExtract] Currently we use opencv to read images, no opencv support found.\n";
#endif
}

int main(int argc, char** argv)
{
	char c_program_usage[50], c_program_version[20];
	snprintf(c_program_usage, 50, "Usage: %s <image_list>", argv[0]);
	snprintf(c_program_version, 20, "%d.%d.%d", LIBVOT_VERSION_MAJOR, LIBVOT_VERSION_MINOR, LIBVOT_VERSION_PATCH);
	string program_usage(c_program_usage), program_version(c_program_version);
	gflags::SetVersionString(program_version);
	gflags::SetUsageMessage(program_usage);		// this has to appear before ParseCommandLineFlags
	gflags::ParseCommandLineFlags(&argc, &argv, true);

	if (argc != 2) {
		cout << "Input argument error, use '--help' to see the usage" << endl;
		cout << program_usage << endl;
		return -1;
	}

	const char *c_image_list = argv[1];
	// guard against single image input
	string image_list_ext = tw::IO::SplitPathExt(std::string(argv[1])).second;
	const int image_ext_num = 18;
	const string image_ext[image_ext_num] = {"JPG", "JPEG", "PNG", "TIFF", "BMP", "PPM", "PGM", "PNM", "GIF",
	                                         "jpg", "jpeg", "png", "tiff", "bmp", "ppm", "pgm", "pnm", "gif"};
	for (int i = 0; i < image_ext_num; i++) {
		if (image_list_ext == image_ext[i]) {
			cerr << "[Extract Feature] " << argv[0] << " takes a image filename list as input, convert image to list!\n";
			exit(-1);
		}
	}

	vector<string> image_filenames, feat_filenames;
	tw::IO::ExtractLines(c_image_list, image_filenames);
	int num_images = image_filenames.size();
	cout << "[Extract Feature] " <<  num_images << " input images\n";
	feat_filenames.resize(num_images);

	// process output_folder flags
	if (FLAGS_output_folder != "") {
		cout << "[Extract Feature] Output folder: " << FLAGS_output_folder << endl;
		tw::IO::Mkdir(FLAGS_output_folder);
		for (int i = 0; i < num_images; i++) {
			feat_filenames[i] = tw::IO::SplitPath(image_filenames[i]).second;
			feat_filenames[i] = tw::IO::SplitPathExt(feat_filenames[i]).first + ".sift";
			feat_filenames[i] = tw::IO::JoinPath(FLAGS_output_folder, feat_filenames[i]);
		}
	}
	else {
		cout << "[Extract Feature] Output folder: output to the same folder as the input files.\n";
		for (int i = 0; i < num_images; i++)
			feat_filenames[i] = tw::IO::SplitPathExt(image_filenames[i]).first + ".sift";
	}

	// process thread number
	if(FLAGS_thread_num == -1) {			// default, use multi-thread
		FLAGS_thread_num = std::thread::hardware_concurrency();		// one single thread would take about 3g memory to process sift
		size_t memory_size = tw::IO::GetAvailMem() / (1024*1024);	// convert to mb
		int available_thread_num = memory_size/(3 * 1024);
		FLAGS_thread_num = FLAGS_thread_num > available_thread_num ? available_thread_num : FLAGS_thread_num;
		FLAGS_thread_num = FLAGS_thread_num > 0 ? FLAGS_thread_num : 1;
		std::cout << "[Extract Feature] Available memory: " << memory_size <<
		             " / thread number: " << FLAGS_thread_num << "\n";
	}
	else {
		const int max_thread_num = std::thread::hardware_concurrency();
		const int min_thread_num = 1;
		if (FLAGS_thread_num < min_thread_num || FLAGS_thread_num > max_thread_num)
			FLAGS_thread_num = 1;
		std::cout << "[Extract Feature] Thread number " << FLAGS_thread_num << " specified by users\n";
	}

#ifdef LIBVOT_USE_OPENCV
	switch (FLAGS_feature_type) {
		case vot::LIBVOT_FEATURE_TYPE::OPENCV_SIFT:
		{
			cout << "[Extract Feature] Compute SIFT features using opencv sift\n";
			cv::SiftDescriptorExtractor cv_sift_detector;
			for (int i = 0; i < num_images; i++) {
				// load the image in BGR format
				const cv::Mat input = cv::imread(image_filenames[i], CV_LOAD_IMAGE_COLOR);

				std::vector<cv::KeyPoint> cv_keypoints;
				cv::Mat sift_descriptors;
				cv_sift_detector.detect(input, cv_keypoints);
				cv_sift_detector.compute(input, cv_keypoints, sift_descriptors);

				vot::SiftData sift_data;
				vot::OpencvKeyPoints2libvotSift(cv_keypoints, sift_descriptors, sift_data);

				if (!sift_data.SaveSiftFile(feat_filenames[i])) {
					cerr << "[Extract Feature] sift_data.SaveSiftFile error.\n";
					exit(-1);
				}
				cout << "[Extract Feature] Save sift data (" << sift_data.getFeatureNum()
				     << " features) to " <<  feat_filenames[i] << "\n";
			}
			break;
		}
		case vot::LIBVOT_FEATURE_TYPE::VLFEAT_SIFT:
		{
			if (FLAGS_thread_num == 1) {		// single thread version
				cout << "[Extract Feature] Compute SIFT features using vlfeat sift\n";
				vot::VlFeatParam vlfeat_param;
				vlfeat_param.edge_thresh = FLAGS_edge_thresh;
				vlfeat_param.peak_thresh = FLAGS_peak_thresh;
				for (int i = 0; i < num_images; i++) {
					const cv::Mat input = cv::imread(image_filenames[i], CV_LOAD_IMAGE_COLOR);
					vot::SiftData sift_data;
					int num_features = vot::RunVlFeature(input.data, input.cols, input.rows, 3, sift_data, vlfeat_param);
					if (!sift_data.SaveSiftFile(feat_filenames[i])) {
						cerr << "[Extract Feature] sift_data.SaveSiftFile error.\n";
						return -1;
					}
					cout << "[Extract Feature] Save sift data (" << num_features
					     << " features) to " <<  feat_filenames[i] << " (" << i << "/" << num_images << ")\n";
				}
			}
			else { 			// multi-thread
				cout << "[Extract Feature] Compute SIFT features using vlfeat sift (multi-thread)\n";
				std::vector<std::thread> threads;
				std::mutex cout_mutex;

				vot::VlFeatParam vlfeat_param;
				vlfeat_param.edge_thresh = FLAGS_edge_thresh;
				vlfeat_param.peak_thresh = FLAGS_peak_thresh;

				size_t off = 0;
				for (int i = 0; i < FLAGS_thread_num; i++) {
					size_t thread_image = num_images / FLAGS_thread_num;
					if (i == FLAGS_thread_num - 1)
						thread_image = num_images - (FLAGS_thread_num - 1) * thread_image;

					threads.push_back(std::thread(MultiVlfeatSiftExtract,
					                              &image_filenames, &feat_filenames,
					                              &vlfeat_param, off, thread_image, &cout_mutex));
					off += thread_image;
				}
				std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));
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
