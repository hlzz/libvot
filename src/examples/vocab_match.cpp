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
#include "data_structures.h"

using namespace std;

int CompareIndexedFloat(const void *a, const void *b)
{
	tw::IndexedFloat *i1, *i2;
	i1 = (tw::IndexedFloat *)a;
	i2 = (tw::IndexedFloat *)b;
	if(i1->value < i2->value)
		return 1;
	else 
		return -1;
}

int main(int argc, char **argv)
{
    if(argc < 4) 
    {
        printf("Usage: %s <image_db> <query_sift_list> <match_output> [sift_type] [thread_num]\n", argv[0]);
        return 1;
    }
    const char *image_db = argv[1];
    const char *query_sift_list = argv[2];
    const char *match_output = argv[3];
    int sift_type = 0;
    int thread_num = 1;

    if(argc > 4)
    	sift_type = atoi(argv[4]);

    if(argc > 5)
    	thread_num = atoi(argv[5]);

    // read tree and image database
    vot::VocabTree *tree = new vot::VocabTree();
    tree->ReadTree(image_db);
    std::cout << "[VocabMatch] Successfully read vocabulary tree (with image database) file " << image_db << std::endl;
    tree->Show();

    // read query image sift data
    std::vector<std::string> sift_filenames;
    tw::IO::ExtractLines(query_sift_list, sift_filenames);
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
        cout << "[VocabMatch] Total sift keys (Type SIFT5.0): " << total_keys << endl;
    }
    else //if(sift_type == 1)
    {
        cout << "[VocabMatch] Sift type is wrong (should be 0). Exit...\n";
        exit(-1);
    }

    FILE *match_file = fopen(match_output, "w");
    if(match_file == NULL)
    {
        cout << "[VocabMatch] Fail to open the match file.\n";
    }
    size_t db_image_num = tree->database_image_num;
    if(thread_num == 1)
    {
    	float *scores = new float [db_image_num];
    	tw::IndexedFloat *indexed_scores = new tw::IndexedFloat [db_image_num];

        for(int i = 0; i < siftfile_num; i++)
        {
            cout << "[VocabMatch] Querying image #" << i << " to database\n";
            memset(scores, 0.0, sizeof(float) * db_image_num);
            tree->Query(sift_data[i], scores);
            for(int j = 0; j < db_image_num; j++)
            {
                indexed_scores[j].value = scores[j];
                indexed_scores[j].index = j;
            }
            qsort(indexed_scores, db_image_num, sizeof(tw::IndexedFloat), CompareIndexedFloat);
            for(int j = 0; j < db_image_num; j++)
            {
                fprintf(match_file, "%d %zd %0.4f\n", i, indexed_scores[j].index, indexed_scores[j].value);
            }
        }

        delete [] scores;
        delete [] indexed_scores;
    }
    else {}	// TODO(tianwei):multi-thread version
    fclose(match_file);

    // release memory
    tree->ClearTree();
    delete tree;

    return 0;
}