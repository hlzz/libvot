#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cassert>
#include <stdlib.h>

#include "data_types.h"
#include "io_utils.h"
#include "vocab_tree.h"

using namespace std;

int main(int argc, char **argv)
{
    if(argc < 4) 
    {
        printf("Usage: %s <sift_list> <tree.out> <db.out> [sift_type]\n", argv[0]);
        return 1;
    }

    const char *sift_input_file = argv[1];
    const char *input_tree = argv[2];
    const char *output_filename = argv[3];
    int sift_type = 0;
    int thread_num = 1;
    int start_id = 0;

    if(argc > 4)
        sift_type = atoi(argv[4]);

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
        cout << "[BuildDB] Total sift keys (Type SIFT5.0): " << total_keys << endl;
    }
    else //if(sift_type == 1)
    {
        cout << "[BuildDB] Sift type is wrong (should be 0). Exit...\n";
        exit(-1);
    }

    // TODO(tianwei): a filter key step
    // add image into the database using the inverted list
    vot::VocabTree tree;
    tree.ReadTree(input_tree);
    std::cout << "[BuildDB] Successfully read vocabulary tree file " << input_tree << std::endl;
    tree.Show();
    tree.SetConstantWeight();
    for(int i = 0; i < siftfile_num; i++)
    {
        double mag = tree.AddImage2Tree(start_id + i, sift_data[i], thread_num);
        cout << "[BuildDB] Add image #" << start_id + i << " to database, image vector magnitude " << mag << endl;
    } 
    tree.ComputeTFIDFWeight(siftfile_num);
    tree.NormalizeDatabase(start_id, siftfile_num);

    std::cout << "[BuildDB] Write vocabulary tree (with image database) to " << output_filename << std::endl;
    tree.WriteTree(output_filename);
    tree.ClearTree();

	return 0;
}
