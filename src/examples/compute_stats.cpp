/*
This program is use to compare the image search result with the ground truth match
Tianwei <shentianweipku@gmail.com>
*/
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <sstream>
#include <algorithm>

#include "io_utils.h"

using namespace std;

int main(int argc, char **argv)
{
	if(argc != 4)
	{
		printf("Usage: %s <sift_file> <groud_truth_match> <match_file>\n\n", argv[0]);
		printf("Each line of the ground_truth_match file consists of a 5-tuple of the form <pmatch, fmatch, hmatch, index1, index2>\n\n");
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
	vector<vector<size_t> > true_matches;
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
			true_matches[index1].push_back(index2);
			ground_truth_count++;
		//cout << index1 << " " << index2 << " " << ground_truth_count << endl;
		}
	}
	fin.close();

	// read match file
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
			vector<size_t>::iterator it;
			it = find(true_matches[index1].begin(), true_matches[index1].end(), index2);
			if(it != true_matches[index1].end())
			{
				hit_count++;
			}
		}
	}
	fin1.close();

	// output the result
	double precision = (double) hit_count / match_count;
	double recall = (double) hit_count / ground_truth_count;
	cout << "hit / ground_truth / match: " << hit_count << " " << ground_truth_count << " " << match_count << "\n";
	cout << "precision / recall: " << precision << " " << recall << "\n";
	return 0;
}