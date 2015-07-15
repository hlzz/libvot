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
    int num_matches = 50;

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
