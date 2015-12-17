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

// web_search: an image search interface specifically used for online viewer

#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#include "vocab_tree.h"
#include "io_utils.h"
#include "data_structures.h"

using namespace std;

int main(int argc, char **argv)
{
    if (argc < 5) 
    {
        printf("Usage: %s <sift_filename> <db_image_list> <db_path> <output_path> [num_matches]\n", argv[0]);
        return -1;
    }

    const char *sift_filename = argv[1];
    const char *db_image_list = argv[2];
    const char *image_db = argv[3];
    const char *output_path = argv[4];

    int num_matches = 5;
    if(argc > 5) num_matches = atoi(argv[5]);

    // get db image filenames
    std::vector<std::string> db_image_filenames;
    tw::IO::ExtractLines(db_image_list, db_image_filenames);

    vot::VocabTree *tree = new vot::VocabTree();
    tree->ReadTree(image_db);
    std::cout << "[VocabMatch] Successfully read vocabulary tree (with image database) file " << image_db << std::endl;
    tree->Show();

    tw::SiftData sift;
    std::string sift_str(sift_filename);
    sift.ReadSiftFile(sift_str);

    FILE *match_file = fopen(output_path, "w");
    if(match_file == NULL)
    {
        std::cout << "[VocabMatch] Fail to open the match file.\n";
    }
    int db_image_num = tree->database_image_num;
    float *scores = new float [db_image_num];
    tw::IndexedFloat *indexed_scores = new tw::IndexedFloat [db_image_num];
    memset(scores, 0.0, sizeof(float) * db_image_num);
    tree->Query(sift, scores);

    for(size_t j = 0; j < db_image_num; j++)
    {
        indexed_scores[j].value = scores[j];
        indexed_scores[j].index = j;
    }
    qsort(indexed_scores, db_image_num, sizeof(tw::IndexedFloat), tw::CompareIndexedFloat);

    int top = num_matches > db_image_num ? db_image_num : num_matches;
    for(size_t j = 0; j < top; j++)
    {
        fprintf(match_file, "%s\n", db_image_filenames[indexed_scores[j].index].c_str());
    }

    delete [] scores;
    delete [] indexed_scores;
    return 0;
}
