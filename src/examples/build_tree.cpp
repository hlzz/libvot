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
#include <stdlib.h>

#include "vocab_tree.h"
#include "io_utils.h"
#include "data_types.h"

using namespace std;

int main(int argc, char **argv)
{
    if (argc != 7) 
    {
        printf("Usage: %s <sift_key_file_names.in> <sift_type> <depth> <branch_num> "
               "<restarts> <tree.out>\n", argv[0]);
        return 1;
    }

    //const char *sift_filenames = argv[1];
    int sift_type = atoi(argv[2]);
    int depth = atoi(argv[3]);
    int branch_num = atoi(argv[4]);
    int restarts = atoi(argv[5]);
    const char *output_filename = argv[6];

    // read sift filenames and get the total number of sift keys
    std::vector<std::string> sift_filenames;
    tw::IO::ExtractLines(argv[1], sift_filenames);
    int siftfile_num = sift_filenames.size();
    std::vector<tw::SiftData> sift_data;

    size_t total_keys = 0;
    for(int i = 0; i < sift_filenames.size(); i++)
    {
        tw::SiftData temp_sift;
        temp_sift.ReadSiftFile(sift_filenames[i]);

        sift_data.push_back(temp_sift);
        cout << "enter\n";
        total_keys += temp_sift.getFeatureNum();
    }

    cout << "Total sift keys: " << total_keys << endl;




    // allocate memory for sift keys


    // build a vocabulary tree using sift keys
    vot::VocabTree vt;
    vt.BuildTree(100, 128, depth, branch_num, NULL);
    return 0;
}
