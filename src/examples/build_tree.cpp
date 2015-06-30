// build_tree.cpp: build a vocabulary tree of visual words

#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cassert>
#include <stdlib.h>

#include "vocab_tree.h"
#include "io_utils.h"
#include "data_types.h"
#include "global_params.h"

//TODO(tianwei): figure out a way to do platform-dependent MAX_ARRAY_SIZE
#define MAX_ARRAY_SIZE 8388608  // 2^23
using namespace std;

/* 
sift_type: 0 - our own sift data format
           1 - TODO(tianwei): vlfeat sift
           2 - TODO(tianwei): lowe's sift type
*/

int main(int argc, char **argv)
{
    if (argc < 5) 
    {
        printf("Usage: %s <sift_list> <depth> <branch_num> <tree.out> [sift_type]\n", argv[0]);
        return -1;
    }

    const char *sift_input_file = argv[1];
    int depth = atoi(argv[2]);
    int branch_num = atoi(argv[3]);
    const char *output_filename = argv[4];
    int sift_type = 0;

    if(argc > 5)
        sift_type = atoi(argv[5]);

    // read sift filenames, get the total number of sift keys, and allocate memory
    std::vector<std::string> sift_filenames;
    tw::IO::ExtractLines(sift_input_file, sift_filenames);
    int siftfile_num = sift_filenames.size();
    size_t total_keys = 0;
    std::vector<tw::SiftData> sift_data;

    if(sift_type == 0)
    {
        sift_data.resize(siftfile_num);
        for(int i = 0; i < sift_filenames.size(); i++)
        {
            sift_data[i].ReadSiftFile(sift_filenames[i]);
            total_keys += sift_data[i].getFeatureNum();
        }
        cout << "[Build Tree] Total sift keys (Type SIFT5.0): " << total_keys << endl;
    }
    else //if(sift_type == 1)
    {
        cout << "[Build Tree] Sift type is wrong (should be 0). Exit...\n";
        exit(-1);
    }

    size_t len = (size_t) total_keys * FDIM;
    int num_arrays = len / MAX_ARRAY_SIZE + ((len % MAX_ARRAY_SIZE) == 0 ? 0 : 1);

    // allocate a big chunk of memory to each array
    cout << "[Build Tree] Allocate " << len << " bytes memory into " << num_arrays << " arrays\n";
    DTYPE **mem = new DTYPE *[num_arrays];       
    size_t remain_length = len;
    for(int i = 0; i < num_arrays; i++)
    {
        size_t len_curr = remain_length > MAX_ARRAY_SIZE ? MAX_ARRAY_SIZE : remain_length;
        mem[i] = new DTYPE [len_curr];
        remain_length -= len_curr;
    }
    assert(remain_length == 0);

    // allocate a pointer array to sift data
    DTYPE **mem_pointer = new DTYPE *[total_keys];
    size_t off = 0;
    size_t curr_key = 0;
    int curr_array = 0;
    for(int i = 0; i < siftfile_num; i++)
    {
        int num_keys = sift_data[i].getFeatureNum();
        if(num_keys > 0)
        {
            DTYPE *dp = sift_data[i].getDesPointer();
            for(int j = 0; j < num_keys; j++)
            {
                for(int k = 0; k < FDIM; k++)
                {
                    mem[curr_array][off+k] = dp[j * FDIM + k];
                }
                mem_pointer[curr_key] = mem[curr_array] + off;
                curr_key++;
                off += FDIM;
                if(off == MAX_ARRAY_SIZE)
                {
                    off = 0;
                    curr_array++;
                }
            }
        }
    }

    // build a vocabulary tree using sift keys
    vot::VocabTree vt;
    if(vt.BuildTree(total_keys, FDIM, depth, branch_num, mem_pointer))
        vt.WriteTree(output_filename);

    vot::VocabTree vt1;
    vt1.ReadTree(output_filename);
    if(vt1.Compare(vt))
    {
        cout << "[VocabTree IO test] vt and vt1 are the same\n";
    }
    else
    {
        cout << "[vocabTree IO test] vt and vt1 are different\n";
    }

    vt.ClearTree();
    // free memory
    delete [] mem_pointer;
    for(int i = 0; i < num_arrays; i++)
    {
        delete [] mem[i];
    }
    delete [] mem;
    return 0;
}
