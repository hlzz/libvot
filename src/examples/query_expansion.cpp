/*
Copyright (c) 2015, Tianwei Shen, HKUST
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of libvot nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

// query_expansion.cpp: query expansion on the image graph

#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include <sstream>
#include <fstream>
#include <Eigen/Dense>
#include <Eigen/SVD>

#include "image_graph/image_graph.h"
#include "utils/io_utils.h"

using namespace std;

int main(int argc, char **argv)
{
    if(argc != 5)
    {
        printf("Usage: %s <sift_file> <groud_truth_match> <match_file> <query_expansion_level>\n", argv[0]);
        printf("Each line of the ground_truth_match file consists of a 5-tuple of the form <pmatch, fmatch, hmatch, index1, index2>\n");
        printf("Each line of the match_file conssits of a 2-tuple of the form <index1, index2>\n");
        return -1;
    }

    const char *sift_file = argv[1];
    const char *ground_truth_match = argv[2];
    const char *match_file = argv[3];
    const int query_level = atoi(argv[4]);

    const int inlier_thresh = 20;
    const int min_finlier = 5;
    const float C = 10000;

    vector<string> sift_filenames;
    tw::IO::ExtractLines(sift_file, sift_filenames);
    size_t image_num = sift_filenames.size();

    vector<vector<vot::LinkNode> > true_matches;
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

    float max_dist = log(min_finlier/C) * log(min_finlier/C); 
    Eigen::MatrixXf distance_matrix = Eigen::MatrixXf::Constant(image_num, image_num, max_dist);
    for(int i = 0; i < image_num; i++)
    {
        distance_matrix(i, i) = 0;
    }

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

        if(index1 < index2) // note that index1 < index2
        {
            // fill in the ground truth match matrix
            if(finlier > inlier_thresh)
            {
                vot::LinkNode temp(index1, index2, 0.0, nmatch, finlier);
                true_matches[index1].push_back(temp);
                ground_truth_count++;
            }
        }
    }
    fin.close();
    // Eigen::FullPivLU<Eigen::MatrixXf> lu_decomp(distance_matrix);
    // cout << "rank " << lu_decomp.rank() << endl;
    // cout << "image num " << image_num << endl;

    // read match file
    // we would like to complete the match_matrix
    //Eigen::MatrixXf match_matrix = Eigen::MatrixXf::Constant(image_num, image_num, 0.0);
    vot::ImageGraph image_graph(image_num);
    size_t match_count = 0;
    size_t hit_count = 0;

    bool **visit_mat;
    visit_mat = new bool *[image_num];
    for(int i = 0; i < image_num; i++)
    {
        visit_mat[i] = new bool [image_num];
        memset(visit_mat[i], false, sizeof(bool)*image_num);
    }

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
            visit_mat[index1][index2] = true;
            for(int i = 0; i < true_matches[index1].size(); i++)
            {
                assert(index1 == true_matches[index1][i].src);
                if(true_matches[index1][i].dst == index2)
                {
                    hit_count++;
                    image_graph.addEdge(true_matches[index1][i]);
                    //match_matrix(index1, index2) = true_matches[index1][i].g_match;
                    //match_matrix(index2, index1) = match_matrix(index1, index2);
                    break;
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

    // Eigen::JacobiSVD<Eigen::MatrixXf> svd(match_matrix, Eigen::ComputeThinU | Eigen::ComputeThinV);
    // Eigen::FullPivLU<Eigen::MatrixXf> lu_decomp(match_matrix);
    // cout << lu_decomp.rank() << endl;

    // query expansion
    vector<vector<vot::LinkNode> > expansion_lists;
    image_graph.QueryExpansion(expansion_lists, visit_mat, query_level);

    // recompute precision and recall after query expansion
    match_count = 0;
    hit_count = 0;
    for(int i = 0; i < image_num; i++)
    {
        for(int j = i+1; j < image_num; j++)
        {
            if(visit_mat[i][j])
            {
                match_count++;
                for(int k = 0; k < true_matches[i].size(); k++)
                {
                    if(true_matches[i][k].dst == j)
                    {
                        hit_count++;
                        break;
                    }
                }
            }
        }
    }
    // int count = 0;
    // for(int i = 0; i < expansion_lists.size(); i++)
    //     count += expansion_lists[i].size();
    // cout << "count " << count << endl;
    precision = (double) hit_count / match_count;
    recall = (double) hit_count / ground_truth_count;
    cout << "hit / match / ground_truth: " << hit_count << " " << match_count << " " << ground_truth_count << "\n";
    cout << "precision / recall: " << precision << " " << recall << "\n";

    // release the memory
    for(int i = 0; i < image_num; i++)
    {
        delete [] visit_mat[i];
    }
    delete [] visit_mat;

    return 0;
}
