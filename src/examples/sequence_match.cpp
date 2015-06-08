#include <iostream>
#include <vector>
#include <string>
#include <stdio.h>

#include "io_utils.h"

using namespace std;

int main(int argc, char **argv)
{
	if(argc != 3)
	{
		printf("Usage: %s <sift_file_list> <output_file>\n", argv[0]);
		return -1;
	}
	const char *sift_input_file = argv[1];
	const char *output_file = argv[2];
	int window_size = 3;

    std::vector<std::string> sift_filenames;
    tw::IO::ExtractLines(sift_input_file, sift_filenames);
    int image_num = sift_filenames.size();

    FILE *fp;
    fp = fopen(output_file, "w");
    if(fp == NULL)
   	{
   		printf("Reading error\n");
   		exit(-1);
   	}

    for(int i = 0; i < image_num; i++)
    {
    	for(int j = 1; j < window_size+1 && i+j < image_num; j++)
    	{
    		fprintf(fp, "%s %s\n", sift_filenames[i].c_str(), sift_filenames[i+j].c_str());
    	}
    }

    fclose(fp);

	return 0;
}