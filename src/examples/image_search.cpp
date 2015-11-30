/*
Copyright (c) 2015, Tianwei Shen
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

// build_tree.cpp: build a vocabulary tree of visual words

#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <unistd.h>

#include "vot_pipeline.h"
#include "io_utils.h"

//TODO(tianwei): figure out a way to do platform-dependent MAX_ARRAY_SIZE
//#define MAX_ARRAY_SIZE 8388608  // 2^23
using namespace std;

/* 
sift_type: 0 - our own sift data format
           1 - TODO(tianwei): vlfeat sift
           2 - TODO(tianwei): lowe's sift type
*/

int main(int argc, char **argv)
{
    if (argc < 3) 
    {
        printf("Usage: %s <sift_list> <output_dir> [depth] [branch_num] [sift_type] [num_matches] [thread_num]\n", argv[0]);
        return -1;
    }

    const char *sift_input_file = argv[1];
    const char *output_dir = argv[2];
    std::string output_directory = std::string(output_dir);
    std::string tree_output = tw::IO::JoinPath(output_directory, std::string("tree.out"));
    std::string db_output = tw::IO::JoinPath(output_directory, std::string("db.out"));
    std::string match_output = tw::IO::JoinPath(output_directory, std::string("match.out"));
    std::string filtered_output = tw::IO::JoinPath(output_directory, std::string("match_pairs"));

    // create folder
    tw::IO::Mkdir(output_directory);

    // optional parameters
    int depth = 6;
    int branch_num = 8;
    int sift_type = 0;
    int thread_num = sysconf(_SC_NPROCESSORS_ONLN);     // this works on unix and mac
    int start_id = 0;
    int num_matches = 100;

    if(argc > 3)
        depth = atoi(argv[3]);
    if(argc > 4)
        branch_num = atoi(argv[4]);
    if(argc > 5)
        sift_type = atoi(argv[5]);
    if(argc > 6)
        num_matches = atoi(argv[6]);
    if(argc > 7)
        thread_num = atoi(argv[7]);

    vot::BuildVocabTree(sift_input_file, tree_output.c_str(), depth, branch_num, sift_type, thread_num);
    vot::BuildImageDatabase(sift_input_file, tree_output.c_str(), db_output.c_str(), sift_type, start_id, thread_num);
    vot::QueryDatabase(db_output.c_str(), sift_input_file, match_output.c_str(), sift_type, thread_num);
    vot::FilterMatchList(sift_input_file, match_output.c_str(), filtered_output.c_str(), num_matches);

    return 0;
}
