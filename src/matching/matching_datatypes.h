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
			case OUTLIER: std::cout << "OUTLIER\n";break;
			case HINLIER: std::cout << "HINLIER\n";break;
			case FINLIER: std::cout << "FINLIER\n";break;
			case INLIER: std::cout << "INLIER\n";break;
			default: std::cerr << "ERROR\n";break;
		}
	}
};

class SiftMatchPair
{
private:
	FeatureMatchPair *match_pairs_;
	std::string filename1_;
	std::string filename2_;
	int nmatch_;
	int homography_inlier_num_;
	int fundamental_inlier_num_;
	Eigen::Matrix3d homography_;
	Eigen::Matrix3d fundamental_matrix_;

public:
	SiftMatchPair(std::string filename1)
	{
		filename1_ = filename1;
		match_pairs_ = NULL;
	}

	SiftMatchPair(int match_buf[][2], const int nmatch,
	const std::vector<bool>& homography_inlier_flag, const std::vector<bool>& fundamental_inlier_flag,
	std::string filename1, std::string filename2,
	const Eigen::Matrix3d &homography, const Eigen::Matrix3d &fundamental_matrix,
	int homography_inlier_num, int fundamental_inlier_num)
	{
		match_pairs_ = new FeatureMatchPair[nmatch];
		nmatch_ = nmatch;
		filename1_ = filename1;
		filename2_ = filename2;
		homography_ = homography;
		fundamental_matrix_ = fundamental_matrix;
		homography_inlier_num_ = homography_inlier_num;
		fundamental_inlier_num_ = fundamental_inlier_num;

		for(size_t i = 0; i < (size_t)(nmatch_); i++)
		{
			match_pairs_[i].first = match_buf[i][0];
			match_pairs_[i].second = match_buf[i][1];
			match_pairs_[i].flag = (FeatureMatchPair::OUTLIER | homography_inlier_flag[i] | (fundamental_inlier_flag[i] << 1));
		}
	}

	// copy constructor
	SiftMatchPair(const SiftMatchPair &obj):
	    filename1_(obj.filename1_), filename2_(obj.filename2_),
	    nmatch_(obj.nmatch_), homography_inlier_num_(obj.homography_inlier_num_),
	    fundamental_inlier_num_(obj.fundamental_inlier_num_),
	    homography_(obj.homography_), fundamental_matrix_(obj.fundamental_matrix_)
	{
		match_pairs_ = new FeatureMatchPair [obj.nmatch_];
		for(int i = 0; i < obj.nmatch_; i++)
		{
			match_pairs_[i] = obj.match_pairs_[i];
		}
	}

	bool operator==(const SiftMatchPair &rhs)
	{   //NOTICE: this is a incomplete comparison
		if(this->filename1_ == rhs.filename1_ && this->filename2_ == rhs.filename2_ &&
		   this->nmatch_ == rhs.nmatch_ &&
		   this->fundamental_inlier_num_ == rhs.fundamental_inlier_num_ &&
		   this->homography_inlier_num_ == rhs.homography_inlier_num_) { return true; }
		else { return false; }
	}

	// copy assignment operator
	SiftMatchPair & operator=(const SiftMatchPair & rhs)
	{
		if(*this == rhs)return *this;
		filename1_ = rhs.filename1_;
		filename2_ = rhs.filename2_;
		nmatch_ = rhs.nmatch_;
		homography_inlier_num_ = rhs.homography_inlier_num_;
		fundamental_inlier_num_ = rhs.fundamental_inlier_num_;
		homography_ = rhs.homography_;
		fundamental_matrix_ = rhs.fundamental_matrix_;
		FeatureMatchPair *originMatchPairs = match_pairs_;
		if (rhs.match_pairs_ != NULL)
		{
			match_pairs_ = new FeatureMatchPair [rhs.nmatch_];
			for(int i = 0; i < rhs.nmatch_; i++)
			{
				match_pairs_[i] = rhs.match_pairs_[i];
			}
		}
		else
			match_pairs_ = NULL;
		if(originMatchPairs != NULL)
			delete [] originMatchPairs;
		return *this;
	}

	// destructor
	~SiftMatchPair()
	{
		if(match_pairs_ != NULL)
		{
			delete [] match_pairs_;
		}
	}

	// Helper functions
	inline const std::string & fileName1() const { return filename1_; }
	inline std::string & fileName1() { return filename1_; }
	inline const std::string & fileNmae2() const { return filename2_; }
	inline std::string & fileNmae2() { return filename2_; }
	inline const int & numMatches() const { return nmatch_; }
	inline int & numMatches() { return nmatch_; }
	inline const int & hInlierNumMatches() const { return homography_inlier_num_; }		// Homography inlier number of matches
	inline int & hInlierNumMatches() { return homography_inlier_num_; }
	inline const int & fInlierNumMatches() const { return fundamental_inlier_num_; }	// Fundamental matrix_ inlier number of matches
	inline int & fInlierNumMatches() { return fundamental_inlier_num_; }

	const FeatureMatchPair * matchPairs() const { return match_pairs_; }
	FeatureMatchPair * matchPairs() { return match_pairs_; }

	/// @brief WriteSiftMatchPair
	/// @param file is open with append mode
	/// @return
	bool WriteSiftMatchPair(FILE *file) const
	{
		// write information
		int filename2_length = filename2_.size();
		fwrite((void*)&filename2_length, sizeof(int), 1, file);
		fwrite((void*)filename2_.c_str(), sizeof(char), filename2_length, file);
		fwrite((void*)&nmatch_, sizeof(int), 1, file);
		fwrite((void*)&homography_inlier_num_, sizeof(int), 1, file);
		fwrite((void*)&fundamental_inlier_num_, sizeof(int), 1, file);

		fwrite((void*)homography_.data(), sizeof(double), 9, file);
		fwrite((void*)fundamental_matrix_.data(), sizeof(double), 9, file);

		// write match correspondence (int, int, char)
		fwrite((void*)match_pairs_, sizeof(FeatureMatchPair), nmatch_, file);

		return true;
	}

	/// @brief ReadSiftMatchPair reads from a file formatted with WriteSiftMatchPair, and then
	///        initialize the SiftMatchPair class with the information extracted from the file
	///
	/// @param file is open with append mode
	/// @return
	bool ReadSiftMatchPair(FILE *file);

	void showInfo() const
	{
		std::cout << "filename1: " << filename1_ << '\n';
		std::cout << "filename2: " << filename2_ << '\n';
		std::cout << "nmatch: " << nmatch_ << '\n';
		std::cout << "homography_inlier_num: " << homography_inlier_num_ << '\n';
		std::cout << "fundamental_inlier_num: " << fundamental_inlier_num_ << '\n';
		std::cout << "homography: " << homography_ << '\n';
		std::cout << "fundamental_matrix: " << fundamental_matrix_ << '\n';

		// random shuffle
		//        std::srand(unsigned (std::time(0)));
		std::vector<int> index_array(nmatch_);
		//        std::iota(index_array.begin(), index_array.end(), 0);
		for (size_t i = 0; i < index_array.size(); ++i) index_array[i] = static_cast<int>(i);
		std::random_shuffle(index_array.begin(), index_array.end());

		for(size_t i = 0; i < 10; i++)
		{
			int index = index_array[i];
			match_pairs_[index].showInfo();
		}
	}
};

class SiftMatchFile
{
private:
	int image_num_;      // the number of match pairs
	std::string mat_filename_;   // the name of the .mat file
	std::vector<SiftMatchPair> match_pairs_;

public:
	SiftMatchFile()     // anonymous .mat file
	{
		image_num_ = 0;
	}
	SiftMatchFile(std::string mat_path)
	{
		mat_filename_ = mat_path;
		image_num_ = 0;
	}

	///
	/// \brief ReadMatchFile: read a .mat file and save the matching infomation in a SiftMatchFile instance
	/// \param mat_path: matching file path
	/// \return
	///
	bool ReadMatchFile(std::string mat_path)
	{
		FILE *fd = fopen(mat_path.c_str(), "rb");
		if(fd == NULL)
		{
			std::cerr << "Can't read .mat file. Exit...\n";
			return false;
		}
		std::string sift_path = tw::IO::SplitPath(tw::IO::SplitPathExt(mat_path).first + ".sift").second;
		SiftMatchPair temp_mp = SiftMatchPair(sift_path);
		while(temp_mp.ReadSiftMatchPair(fd))
		{
			match_pairs_.push_back(temp_mp);
		}
		image_num_ = match_pairs_.size();
		fclose(fd);
		return true;
	}

	const int & getMatchNum() const { return image_num_; }
	const std::string & getMatFilename() const { return mat_filename_; }
	const std::vector<SiftMatchPair> & getSiftMatchPairs() const { return match_pairs_; }
};

}	// end of namespace vot

#endif	// MATCHING_DATATYPES_H
