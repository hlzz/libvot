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
	SiftMatcher(int max_sift);
	void setMaxSift(int max_sift);
	const int getMaxSift() const;

private:
	int max_sift_;
};

}	// end of namespace vot

#endif	// MATCHING_DATATYPES_H
