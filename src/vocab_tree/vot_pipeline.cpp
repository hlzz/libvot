#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cassert>
#include <unordered_map>
#include <sstream>

#include "vot_pipeline.h"
#include "vocab_tree.h"
#include "io_utils.h"
#include "data_types.h"
#include "data_structures.h"
#include "global_params.h"

#define MAX_ARRAY_SIZE 8388608  // 2^23

namespace vot
{
	bool BuildVocabTree(const char *sift_list, 
						const char *output_filename, 
						int depth, int branch_num, 
						int sift_type, int thread_num)
	{
    	// read sift filenames, get the total number of sift keys, and allocate memory 
		std::vector<std::string> sift_filenames; 
		tw::IO::ExtractLines(sift_list, sift_filenames); 
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
			std::cout << "[Build Tree] Total sift keys (Type SIFT5.0): " << total_keys << std::endl; 
		} 
	    else //if(sift_type == 1) 
	    {
	    	std::cout << "[Build Tree] Sift type is wrong (should be 0). Exit...\n"; 
	    	return false;
	    }
	    size_t len = (size_t) total_keys * FDIM;
	    int num_arrays = len / MAX_ARRAY_SIZE + ((len % MAX_ARRAY_SIZE) == 0 ? 0 : 1);

	    // allocate a big chunk of memory to each array
	    std::cout << "[Build Tree] Allocate " << len << " bytes memory into " << num_arrays << " arrays\n";
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
	    	size_t num_keys = sift_data[i].getFeatureNum();
	    	if(num_keys > 0)
	    	{
	    		DTYPE *dp = sift_data[i].getDesPointer();
	    		for(size_t j = 0; j < num_keys; j++)
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
	    if(vt.BuildTree(total_keys, FDIM, depth, branch_num, mem_pointer, thread_num))
	    	vt.WriteTree(output_filename);

	    vot::VocabTree vt1;
	    vt1.ReadTree(output_filename);
	    if(vt1.Compare(vt))
	    {
	    	std::cout << "[VocabTree IO test] vt and vt1 are the same\n";
	    }
	    else
	    {
	    	std::cout << "[vocabTree IO test] vt and vt1 are different\n";
	    }

	    vt.ClearTree();
	    // free memory
	    delete [] mem_pointer;
	    for(int i = 0; i < num_arrays; i++)
	    {
	    	delete [] mem[i];
	    }
	    delete [] mem;
	    return true;
	}

	bool BuildImageDatabase(const char *sift_list, 
							const char *input_tree, 
							const char *output_filename, 
							int sift_type, 
							size_t start_id, 
							int thread_num)
	{
	    // read sift filenames, get the total number of sift keys, and allocate memory
	    std::vector<std::string> sift_filenames;
	    tw::IO::ExtractLines(sift_list, sift_filenames);
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
	        std::cout << "[BuildDB] Total sift keys (Type SIFT5.0): " << total_keys << '\n';
	    }
	    else //if(sift_type == 1)
	    {
	        std::cout << "[BuildDB] Sift type is wrong (should be 0). Exit...\n";
	        return false;
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
	        std::cout << "[BuildDB] Add image #" << start_id + i << " to database, image vector magnitude " << mag << '\n';
	    } 
	    tree.ComputeTFIDFWeight(siftfile_num);
	    tree.NormalizeDatabase(start_id, siftfile_num);

	    std::cout << "[BuildDB] Write vocabulary tree (with image database) to " << output_filename << '\n';
	    tree.WriteTree(output_filename);
	    tree.ClearTree();
	}

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

	bool QueryDatabase(const char *image_db, 
					   const char *query_sift_list, 
					   const char *match_output, 
					   int sift_type, int thread_num)
	{
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
			std::cout << "[VocabMatch] Total sift keys (Type SIFT5.0): " << total_keys << '\n';
		}
	    else //if(sift_type == 1)
	    {
	    	std::cout << "[VocabMatch] Sift type is wrong (should be 0). Exit...\n";
	    	exit(-1);
	    }

	    FILE *match_file = fopen(match_output, "w");
	    if(match_file == NULL)
	    {
	    	std::cout << "[VocabMatch] Fail to open the match file.\n";
	    }
	    size_t db_image_num = tree->database_image_num;
	    // if(thread_num == 1)
	    // {
	    float *scores = new float [db_image_num];
	    tw::IndexedFloat *indexed_scores = new tw::IndexedFloat [db_image_num];

	    for(int i = 0; i < siftfile_num; i++)
	    {
	    	std::cout << "[VocabMatch] Querying image #" << i << " to database\n";
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
	    // }
	    // else {}	// TODO(tianwei):multi-thread version
	    fclose(match_file);

	    // release memory
	    tree->ClearTree();
	    delete tree;
	    return true;
	}

	bool FilterMatchList(const char *sift_list, const char *match_list, const char *output, int num_matches)
	{		
		std::vector<std::string> sift_filenames;
		tw::IO::ExtractLines(sift_list, sift_filenames);
		int image_num = sift_filenames.size();
		num_matches = num_matches > image_num ? image_num : num_matches;

	    std::ifstream fin(match_list);
	    if(!fin.is_open())
	    {
	        std::cout << "[FilterMatchList] Fail to open the vocabulary tree match output\n";
	        return false;
	    }
	    else {
	        std::cout << "[FilterMatchList] Read the vocabulary tree match file: " << match_list << '\n';
	    }

	    std::unordered_map<size_t, std::vector<size_t> > match_pairs;
	    std::string line;
	    size_t index0, index1;
	    float score;
	    while(!fin.eof())
	    {
	        std::getline(fin, line);
	        if(line == "")
	            continue;
	        std::stringstream line_stream;
	        line_stream << line;
	        line_stream >> index0 >> index1 >> score;

	        if(index0 >= sift_filenames.size() || index1 >= sift_filenames.size())
	        {
	            std::cout << "[FilterMatchList] Invalid index pair: " << index0 << " " << index1 << '\n';
	            continue;
	        }

	        if(score > 0 && index0 != index1)
	        {
	            std::unordered_map<size_t, std::vector<size_t> >::iterator itr = match_pairs.find(index0);
	            if(itr == match_pairs.end())
	            {
	                std::vector<size_t> pairs;
	                pairs.push_back(index1);
	                match_pairs.insert(std::make_pair(index0, pairs));
	            }
	            else {
	                std::vector<size_t> & pairs = itr->second;
	                pairs.push_back(index1);
	            }
	        }
	    }
	    fin.close();

		std::ofstream fout("match_pairs_idx.txt");
		std::ofstream fout1("match_pairs_idx");
		if(!fout.is_open() || !fout1.is_open())
	    {
	        std::cout << "[FilterMatchList] Fail to open the filtered match file\n";
	        return false;
	    }
	    else 
	    {
	        std::cout << "[FilterMatchList] Write the filtered match file match_pairs_idx" << '\n';
	    }

	    for(std::unordered_map<size_t, std::vector<size_t> >::const_iterator itr = match_pairs.begin(); itr != match_pairs.end(); itr++)
	    {
	        size_t index0 = itr->first;
	        std::vector<size_t> const & pairs = itr->second;
	        size_t num_match_images = std::min((size_t)num_matches, pairs.size());
	        for(size_t idx = 0; idx < num_match_images; idx++)
	        {
	            size_t index1 = pairs[idx];
	            fout << sift_filenames[index0] << '\t' << sift_filenames[index1] << '\n';
	            fout1 << index0 << " " << index1 << '\n';
	        }
	    }
	    fout1.close();
	    fout.close();
		return true;	
	}

}	// end of namespace vot