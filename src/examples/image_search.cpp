// build_tree.cpp: build a vocabulary tree of visual words

#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <unistd.h>

#include "vot_pipeline.h"

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
        printf("Usage: %s <sift_list> <tree.out> [depth] [branch_num] [sift_type] [num_matches] [thread_num]\n", argv[0]);
        return -1;
    }

    const char *sift_input_file = argv[1];
    const char *tree_output = argv[2];
    const char *db_output = "db.out";
    const char *match_output = "match.out";
    const char *filtered_output = "match";

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

    //vot::BuildVocabTree(sift_input_file, tree_output, depth, branch_num, sift_type, thread_num);
    //vot::BuildImageDatabase(sift_input_file, tree_output, db_output, sift_type, start_id, thread_num);
    //vot::QueryDatabase(db_output, sift_input_file, match_output, sift_type, thread_num);
    vot::FilterMatchList(sift_input_file, match_output, filtered_output, num_matches);

    return 0;
}
