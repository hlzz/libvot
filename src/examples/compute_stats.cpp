/*
This program is use to compare the image search result with the ground truth match
Tianwei <shentianweipku@gmail.com>
*/

#include <iostream>
#include <stdio.h>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <unordered_set>
#include <cmath>
#include <cassert>
#include <cstdlib>

#include <Eigen/Dense>
#include <Eigen/SVD>

#include "io_utils.h"

using namespace std;

float Fnorm(Eigen::MatrixXf &f)
{
	float norm = 0;
	for(int i = 0; i < f.rows(); i++)
	{
		for(int j = 0; j < f.cols(); j++)
		{
			norm += f(i, j) * f(i, j);
		}
	}
	return sqrt(norm);
}

int main(int argc, char **argv)
{
	if(argc != 4)
	{
		printf("Usage: %s <sift_file> <groud_truth_match> <match_file>\n", argv[0]);
		printf("Each line of the ground_truth_match file consists of a 5-tuple of the form <pmatch, fmatch, hmatch, index1, index2>\n");
		printf("Each line of the match_file conssits of a 2-tuple of the form <index1, index2>\n");
		return -1;
	}

	const char *sift_file = argv[1];
	const char *ground_truth_match = argv[2];
	const char *match_file = argv[3];
	const int inlier_thresh = 20;

	vector<string> sift_filenames;
	tw::IO::ExtractLines(sift_file, sift_filenames);
	size_t image_num = sift_filenames.size();
	vector<unordered_set<size_t> > true_matches;
	true_matches.resize(image_num);

	// read ground truth match file
	ifstream fin(ground_truth_match);
	if(!fin.is_open())
	{
		cout << "Failed to open the ground truth match\n";
		return -1;
	}

	string line;
	size_t index1, index2;
	int nmatch, finlier, hinlier;
	size_t ground_truth_count = 0;
	while(!fin.eof())
	{
		std::getline(fin, line);
		if(line == "")
			continue;
		std::stringstream ss;
		ss << line;
		ss >> nmatch >> finlier >> hinlier >> index1 >> index2;
		if(index1 > image_num || index2 > image_num)
		{
			cout << "Error: invalid index pair, exit...\n";
			return -1;
		}
		if(finlier > inlier_thresh && index1 < index2)
		{
			// note that index1 < index2
			true_matches[index1].insert(index2);
			ground_truth_count++;
		}
	}
	fin.close();

	// read match file
	std::vector<std::unordered_set<int> > match_log;
	match_log.resize(image_num);
	size_t match_count = 0;
	size_t hit_count = 0;
	ifstream fin1(match_file);
	if(!fin1.is_open())
	{
		cout << "Failed to open the match file\n";
		return -1;
	}
	while(!fin1.eof())
	{
		std::getline(fin1, line);
		if(line == "")
			continue;
		std::stringstream ss;
		ss << line;
		ss >> index1 >> index2;

		if(index1 < index2)
		{
			match_count++;
			match_log[index1].insert(index2);
			std::unordered_set<size_t>::iterator it = true_matches[index1].find(index2);
			if(it != true_matches[index1].end())
			{
				hit_count++;
			}
		}
		else 			// index1 > index2
		{
			std::unordered_set<int>::iterator it = match_log[index2].find(index1);
			if(it == match_log[index2].end())
			{
				match_count++;
				std::unordered_set<size_t>::iterator it1 = true_matches[index2].find(index1);
				if(it1 != true_matches[index2].end())
				{
					hit_count++;
				}
			}
		}
	}
	fin1.close();

	// output the result
	double precision = (double) hit_count / match_count;
	double recall = (double) hit_count / ground_truth_count;
	cout << "hit / match / ground_truth: " << hit_count << " " << match_count << " " << ground_truth_count << "\n";
	cout << "precision / recall: " << precision << " " << recall << "\n";


    //////////////////////////////////////////////////////////////////////////////
    //                                                                          //
    //                     Singular Value Thresholding							//
    //                                                                          //
    //////////////////////////////////////////////////////////////////////////////

	// Eigen::MatrixXf gt_mat = Eigen::MatrixXf::Constant(image_num, image_num, -1);
	// for(int i = 0; i < image_num; i++)
	// {
	// 	for(int j = 0; j < true_matches[i].size(); j++)
	// 	{
	// 		gt_mat(i, true_matches[i][j]) = 1;
	// 		gt_mat(true_matches[i][j], i) = 1;
	// 	}
	// }

	// // random sampling ground truth and form the sample matrix M
	// Eigen::MatrixXf sample_mt = Eigen::MatrixXf::Constant(image_num, image_num, 0);

	// const float sample_ratio = 0.02;
	// const int sample_size = image_num * sample_ratio;
	// vector<vector<int> > sample_set;
	// sample_set.resize(image_num);

	// // sample without replacement
	// for(int i = 0; i < image_num; i++)
	// {
	// 	for(int j = 0; j < sample_size; j++)
	// 	{
	// 		int curr_idx = rand() % image_num;
	// 		int k;
	// 		for(k = 0; k < j; k++)
	// 		{
	// 			if(curr_idx == sample_set[i][k]) 
	// 			{
	// 				j--; k = -1;
	// 				break;
	// 			}
	// 		}
	// 		if(k != -1)
	// 			sample_set[i].push_back(curr_idx);
	// 	}
	// 	assert(sample_set[i] == sample_size);
	// }

	// int count = 0;
	// for(int i = 0; i < image_num; i++)
	// {
	// 	for(int j = 0; j < sample_size; j++)
	// 	{
	// 		sample_mt(i, sample_set[i][j]) = gt_mat(i, sample_set[i][j]);
	// 		sample_mt(sample_set[i][j], i) = gt_mat(i, sample_set[i][j]); 
	// 		if(gt_mat(i, sample_set[i][j]) == 1)
	// 		{
	// 			count++;
	// 		}
	// 	}
	// }
	// float step_size = 1.2 / sample_ratio;
	// float tolerance = 1e-4;	
	// float tau = 100000;
	// int max_iter = 30;

	// int k0 = tau / (step_size * Fnorm(sample_mt));
	// //cout << tau << " " << step_size << " " << Fnorm(sample_mt) << " " << k0 <<endl;
	// //Eigen::MatrixXf Y = 
	// Eigen::MatrixXf X = Eigen::MatrixXf::Constant(image_num, image_num, 0);
	// Eigen::MatrixXf Y = k0 * step_size * sample_mt;
	// int r0 = 0;
	// for(int k = 0; k < max_iter; k++)
	// {
	// 	cout << "iteration " << k  << ": " << endl;
	// 	Eigen::JacobiSVD<Eigen::MatrixXf> svd(sample_mt, Eigen::ComputeThinU | Eigen::ComputeThinV);
	// 	// Eigen::MatrixXf U = svd.matrixU();
	// 	// Eigen::MatrixXf V = svd.matrixV();
	// 	// cout << svd.singularValues() << endl;
	// }


	// Eigen::FullPivLU<Eigen::MatrixXf> lu_decomp(gt_mat);
	// lu_decomp.setThreshold(1e-5);
	// cout << lu_decomp.rank() << endl;

	// Eigen::JacobiSVD<Eigen::MatrixXf> svd(gt_mat, Eigen::ComputeThinU | Eigen::ComputeThinV);
	// Eigen::MatrixXf U = svd.matrixU();
	// Eigen::MatrixXf V = svd.matrixV();
	// cout << svd.singularValues() << endl;
	// float umax = -1, vmax = -1;
	// int usize = gt_mat.rows();
	// int vsize = gt_mat.cols();
	// for(int i = 0; i < usize; i++)
	// {
	// 	for(int j = 0; j < usize; j++)
	// 	{
	// 		if(umax < U(i, j))
	// 			umax = U(i, j);
	// 	}
	// }

	// for(int i = 0; i < vsize; i++)
	// {
	// 	for(int j = 0; j < vsize; j++)
	// 	{
	// 		if(vmax < V(i, j))
	// 			vmax = V(i, j);
	// 	}
	// }

	// cout << "umax: " << umax << endl;
	// cout << "vmax: " << vmax << endl;
	return 0;
}