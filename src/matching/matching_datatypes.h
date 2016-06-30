/** \file matching_datatypes.h
 *	\brief matching data structures header
 *
 */
// Author: Tianwei Shen <shentianweipku@gmail.com>

#ifndef MATCHING_DATATYPES_H
#define MATCHING_DATATYPES_H

#include <iostream>
#include <Eigen/Dense>
#include "utils/global_params.h"
#include "utils/data_types.h"
#include "utils/io_utils.h"

namespace vot {
/**
 * @brief Match parameters
 */
struct MatchParam
{
	MatchParam()
	{
		cuda_device = -1;
		thread_num = 1;
		num_sequece_match = 0;
		max_sift = 32768;
		is_sequnce_match = false;
		is_optional_match = false;
		optional_match_path = "";

		min_num_inlier = 25;
		min_f_inlier = 15;
		min_h_inlier = 15;

		min_f_inlier_ratio = 0.1;
		min_h_inlier_ratio = 0.1;
		f_ransac_ratio = 0.5;
		h_ransac_ratio = 0.5;
		f_ransac_conf = 0.999999;
		h_ransac_conf = 0.999999;

		f_error = 4;
		h_error = 4;
	}

	// Match configuration param
	int cuda_device;
	size_t thread_num;
	size_t num_sequece_match;
	size_t max_sift;
	bool is_sequnce_match;
	bool is_optional_match;
	std::string optional_match_path;

	// Match error params
	size_t min_num_inlier;
	size_t min_f_inlier;
	size_t min_h_inlier;

	double min_f_inlier_ratio;
	double min_h_inlier_ratio;
	double f_ransac_ratio;
	double h_ransac_ratio;
	double f_ransac_conf;
	double h_ransac_conf;

	double f_error;
	double h_error;
};

struct FeatureMatchPair
{
public:
	enum FlagType { OUTLIER, HINLIER, FINLIER, INLIER};
	int first;
	int second;
	char flag;

	bool operator==(const FeatureMatchPair & rhs)
	{
		if(first == rhs.first && second == rhs.second && flag == rhs.flag) { return true; }
		else { return false; }
	}

	FeatureMatchPair & operator=(const FeatureMatchPair & rhs)
	{
		first = rhs.first;
		second = rhs.second;
		flag = rhs.flag;
		return *this;
	}

	void showInfo() const
	{
		std::cout << first << " " << second << " ";
		switch(flag)
		{
			case OUTLIER: std::cout << "OUTLIER\n"; break;
			case HINLIER: std::cout << "HINLIER\n"; break;
			case FINLIER: std::cout << "FINLIER\n"; break;
			case INLIER: std::cout << "INLIER\n"; break;
			default: std::cerr << "ERROR\n";
		}
	}
};

class SiftMatchPair
{
public:
	SiftMatchPair(std::string filename1);

	SiftMatchPair(int match_buf[][2], const int nmatch,
	const std::vector<bool>& homography_inlier_flag, const std::vector<bool>& fundamental_inlier_flag,
	std::string filename1, std::string filename2,
	const Eigen::Matrix3d &homography, const Eigen::Matrix3d &fundamental_matrix,
	int homography_inlier_num, int fundamental_inlier_num);

	SiftMatchPair(const SiftMatchPair &obj);	//!< copy constructor
	bool operator==(const SiftMatchPair &rhs);
	SiftMatchPair & operator=(const SiftMatchPair & rhs);	 //!< copy assignment operator
	~SiftMatchPair(); 	//!< destructor

	/**
	 * @brief WriteSiftMatchPair: write sift match pairs to file
	 * @param file: the file path
	 * @return true if success
	 */
	bool WriteSiftMatchPair(FILE *file) const;
	bool ReadSiftMatchPair(FILE *file);

	// Helper functions
	inline const std::string & fileName1() const;
	inline std::string & fileName1();
	inline const std::string & fileNmae2() const;
	inline std::string & fileNmae2();
	inline const int & numMatches() const;
	inline int & numMatches();
	inline const int & hInlierNumMatches() const;
	inline int & hInlierNumMatches();
	inline const int & fInlierNumMatches() const;
	inline int & fInlierNumMatches();
	const FeatureMatchPair* matchPairs() const;
	FeatureMatchPair* matchPairs();
	void showInfo() const;

private:
	FeatureMatchPair *match_pairs_;
	std::string filename1_;
	std::string filename2_;
	int nmatch_;
	int homography_inlier_num_;
	int fundamental_inlier_num_;
	Eigen::Matrix3d homography_;
	Eigen::Matrix3d fundamental_matrix_;
};

class SiftMatchFile
{
public:
	SiftMatchFile();     //!< anonymous .mat file
	SiftMatchFile(std::string mat_path);

	/**
	 * @brief ReadMatchFile: read a .mat file and save the matching infomation in a SiftMatchFile instance
	 * @param mat_path: matching file path
	 * @return true if success
	 */
	bool ReadMatchFile(std::string mat_path);
	const int & getMatchNum() const;
	const std::string & getMatFilename() const;
	const std::vector<SiftMatchPair> & getSiftMatchPairs() const;

private:
	int image_num_;      // the number of match pairs
	std::string mat_filename_;   // the name of the .mat file
	std::vector<SiftMatchPair> match_pairs_;
};

/**
 * @brief The SiftMatcher class: sift feature matcher, inspired by Changchang Wu's SiftGPU
 */
class SiftMatcher
{
public:
	enum MatcherDevice {
		SIFT_MATCH_CPU = 0,
		SIFT_MATCH_CUDA = 1,
		SIFT_MATCH_GLSL = 2,
	};

	SiftMatcher(int max_sift);
	virtual ~SiftMatcher();
	bool Init();		//!< initialize matcher context
	void SetMaxSift(int max_sift);	//!< set maximum buffer length
	const int GetMaxSift() const;	//!< get maximum buffer length

	/**
	 * @brief SetMatchDevice: set match device (cpu, cuda, glsl)
	 * @param device: 0 for cpu, 1 for cuda, 2 for glsl
	 * @return: false if the device is not available, which means using cpu matcher
	 */
	bool SetMatchDevice(int device);
	int GetMatchDevice(int device) const;	//!< get match device
	/**
	 * @brief SetDescriptors: set descriptors to match
	 * @param index: index = [0/1] for two features sets respectively
	 * @param num
	 * @param descriptors: descriptor pointer
	 * @param id
	 */
	virtual bool SetDescriptors(int index, int num, const float *descriptors);
	/**
	 * @brief SetDescriptors: descriptors are in unsigned char
	 */
	virtual bool SetDescriptors(int index, int num, const unsigned char *descriptors);

	/**
	 * @brief GetSiftMatch: match two sets of features, and returns the number of matches.
	 * Given two normalized descriptor d1,d2, the distance here is acos(d1 *d2);
	 * @param max_match: the length of the match_buffer.
	 * @param match_buffer: buffer to receive the matched feature indices
	 * @param distmax: maximum distance of sift descriptor
	 * @param ratiomax: maximum distance ratio
	 * @param mutual_best_match: mutual best match or one way
	 * @return the number of matches
	 */
	virtual int GetSiftMatch(int max_match,
	                         int match_buffer[][2],
							 float distmax = 0.7,
	 						 float ratiomax = 0.8,
							 int mutual_best_match = 1);

protected:
	int max_sift_;
	int num_sift_[2];
private:
	int match_device_;
	SiftMatcher *matcher_;
};

/**
 * @brief The SiftMatcherCPU class: matcher using cpu
 */
class SiftMatcherCPU: public SiftMatcher
{
public:
	SiftMatcherCPU(int max_sift);
	~SiftMatcherCPU();
	int GetSiftMatch(int max_match, int match_buffer[][2], float distmax, float ratiomax, int mutual_best_match);
	bool SetDescriptors(int index, int num, const unsigned char *descriptors);
	bool SetDescriptors(int index, int num, const float *descriptors);
private:
	std::vector<float> sift_buffer;
};

/**
 * @brief The SiftMatcherGL class: matcher using openGL
 */
class SiftMatcherGL: public SiftMatcher
{
public:
	SiftMatcherGL(int max_sift);
	~SiftMatcherGL();
	int GetSiftMatch(int max_match, int match_buffer[][2], float distmax, float ratiomax, int mutual_best_match);
private:
};

/**
 * @brief The SiftMatcherCUDA class: matcher using cuda
 */
class SiftMatcherCUDA: public SiftMatcher
{
public:
	SiftMatcherCUDA(int max_sift);
	~SiftMatcherCUDA();
	int GetSiftMatch(int max_match, int match_buffer[][2], float distmax, float ratiomax, int mutual_best_match);
private:
};
}	// end of namespace vot

#endif	// MATCHING_DATATYPES_H
