/** \file matching_datatypes.cpp
 *	\brief matching data structures implementation
 *
 */
// Author: Tianwei Shen <shentianweipku@gmail.com>

#include "matching_datatypes.h"

#include <iostream>
#include "utils/data_types.h"
#include "utils/io_utils.h"

namespace vot {

// ====================================================
// ---------------- SiftMatchPair class ---------------
// ====================================================
SiftMatchPair::SiftMatchPair(std::string filename1)
{
	filename1_ = filename1;
	match_pairs_ = NULL;
}

SiftMatchPair::SiftMatchPair(int match_buf[][2], const int nmatch,
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
SiftMatchPair::SiftMatchPair(const SiftMatchPair &obj):
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

bool SiftMatchPair::operator==(const SiftMatchPair &rhs)
{
	//NOTE(tianwei): this is a incomplete comparison
	if(this->filename1_ == rhs.filename1_ && this->filename2_ == rhs.filename2_ &&
	   this->nmatch_ == rhs.nmatch_ &&
	   this->fundamental_inlier_num_ == rhs.fundamental_inlier_num_ &&
	   this->homography_inlier_num_ == rhs.homography_inlier_num_)
		return true;
	else
		return false;
}

// copy assignment operator
SiftMatchPair & SiftMatchPair::operator=(const SiftMatchPair & rhs)
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
			match_pairs_[i] = rhs.match_pairs_[i];
	}
	else
		match_pairs_ = NULL;
	if(originMatchPairs != NULL)
		delete [] originMatchPairs;
	return *this;
}

// destructor
SiftMatchPair::~SiftMatchPair()
{
	if(match_pairs_ != NULL)
		delete [] match_pairs_;
}

bool SiftMatchPair::WriteSiftMatchPair(FILE *file) const
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

bool SiftMatchPair::ReadSiftMatchPair(FILE *file)
{
	char filename2_temp[256];
	int filename2_length;

	int check = fread((void*)&filename2_length, sizeof(int), 1, file);
	if(check != 1)  return false;

	fread((void*)filename2_temp, sizeof(char), filename2_length, file);
	filename2_temp[filename2_length] = '\0';
	filename2_ = std::string(filename2_temp);

	fread((void*)&nmatch_, sizeof(int), 1, file);
	fread((void*)&homography_inlier_num_, sizeof(int), 1, file);
	fread((void*)&fundamental_inlier_num_, sizeof(int), 1, file);

	fread((void*)homography_.data(), sizeof(double), 9, file);
	fread((void*)fundamental_matrix_.data(), sizeof(double), 9, file);

	// read match correspondence (int, int, char)
	if(match_pairs_ != NULL)
	{
		delete [] match_pairs_;
		match_pairs_ = NULL;
	}
	match_pairs_ = new FeatureMatchPair [nmatch_];
	fread((void*)match_pairs_, nmatch_, sizeof(FeatureMatchPair), file);
	return true;
}

void SiftMatchPair::showInfo() const
{
	std::cout << "filename1: " << filename1_ << '\n';
	std::cout << "filename2: " << filename2_ << '\n';
	std::cout << "nmatch: " << nmatch_ << '\n';
	std::cout << "homography_inlier_num: " << homography_inlier_num_ << '\n';
	std::cout << "fundamental_inlier_num: " << fundamental_inlier_num_ << '\n';
	std::cout << "homography: " << homography_ << '\n';
	std::cout << "fundamental_matrix: " << fundamental_matrix_ << '\n';

	// random shuffle
	std::vector<int> index_array(nmatch_);
	for (size_t i = 0; i < index_array.size(); ++i) index_array[i] = static_cast<int>(i);
	std::random_shuffle(index_array.begin(), index_array.end());

	for(size_t i = 0; i < 10; i++)
	{
		int index = index_array[i];
		match_pairs_[index].showInfo();
	}
}

// Helper functions
inline const std::string & SiftMatchPair::fileName1() const { return filename1_; }
inline std::string & SiftMatchPair::fileName1() { return filename1_; }
inline const std::string & SiftMatchPair::fileNmae2() const { return filename2_; }
inline std::string & SiftMatchPair::fileNmae2() { return filename2_; }
inline const int & SiftMatchPair::numMatches() const { return nmatch_; }
inline int & SiftMatchPair::numMatches() { return nmatch_; }

inline const int & SiftMatchPair::hInlierNumMatches() const { return homography_inlier_num_; } //!< Homography inlier number of matches
inline int & SiftMatchPair::hInlierNumMatches() { return homography_inlier_num_; }
inline const int & SiftMatchPair::fInlierNumMatches() const { return fundamental_inlier_num_; }//!< Fundamental matrix_ inlier number of matches
inline int & SiftMatchPair::fInlierNumMatches() { return fundamental_inlier_num_; }

const FeatureMatchPair* SiftMatchPair::matchPairs() const { return match_pairs_; }
FeatureMatchPair* SiftMatchPair::matchPairs() { return match_pairs_; }


// ====================================================
// ---------------- SiftMatchFile class ---------------
// ====================================================
SiftMatchFile::SiftMatchFile()
{
	image_num_ = 0;
}

SiftMatchFile::SiftMatchFile(std::string mat_path)
{
	mat_filename_ = mat_path;
	image_num_ = 0;
}

bool SiftMatchFile::ReadMatchFile(std::string mat_path)
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

const int & SiftMatchFile::getMatchNum() const { return image_num_; }
const std::string & SiftMatchFile::getMatFilename() const { return mat_filename_; }
const std::vector<SiftMatchPair> & SiftMatchFile::getSiftMatchPairs() const { return match_pairs_; }


// ====================================================
// ---------------- SiftMatcher class ---------------
// ====================================================
SiftMatcher::SiftMatcher(int max_sift) { max_sift_ = max_sift; }

void SiftMatcher::setMaxSift(int max_sift) { max_sift_ = max_sift; }

const int SiftMatcher::getMaxSift() const { return (const int) max_sift_; }

}	// end of namespace vot
