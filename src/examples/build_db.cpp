#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cassert>
#include <stdlib.h>

#include "data_types.h"
#include "io_utils.h"

using namespace std;

int main(int argc, char **argv)
{
    if (argc != 4) 
    {
        printf("Usage: %s <sift_key_file_names.in> <sift_type> <tree.out>\n", argv[0]);
        return 1;
    }

    const char *sift_input_file = argv[1];
    int sift_type = atoi(argv[2]);
    //const char *output_filename = argv[5];

    // read sift filenames, get the total number of sift keys, and allocate memory
    std::vector<std::string> sift_filenames;
    tw::IO::ExtractLines(sift_input_file, sift_filenames);
    int siftfile_num = sift_filenames.size();
    size_t total_keys = 0;
    std::vector<tw::SiftData> sift_data;
    for(int i = 0; i < sift_filenames.size(); i++)
    {
    	cout << sift_filenames[i] << endl;
    }

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
	return 0;
}
