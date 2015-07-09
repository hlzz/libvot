#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cassert>
#include <unordered_map>
#include <algorithm>
#include <sstream>
#include <thread>
#include <mutex>

#include "vot_pipeline.h"
#include "vocab_tree.h"
#include "io_utils.h"
#include "data_types.h"
#include "data_structures.h"
#include "global_params.h"

#define MAX_ARRAY_SIZE 8388608  // 2^23

namespace vot
{
    /** Random sample "sample_num" number of numbers from [0, 1, ... , total] */
	std::vector<size_t> RandomSample(size_t total, size_t sample_num)
	{
		std::vector<size_t> samples;
		std::vector<size_t> total_numbers;
		for(size_t i = 0; i < total; i++)
		{
			total_numbers.push_back(i);
		}
		std::random_shuffle(total_numbers.begin(), total_numbers.end());
		for(size_t i = 0; i < sample_num; i++)
		{
			samples.push_back(total_numbers[i]);
		}
		return samples;
	}

	bool BuildVocabTree(const char *sift_list, 
						const char *output_filename, 
						int depth, int branch_num, 
						int sift_type, int thread_num)
	{
    	// read sift filenames, get the total number of sift keys, and allocate memory 
		std::vector<std::string> sift_filenames; 
		tw::IO::ExtractLines(sift_list, sift_filenames); 
		size_t siftfile_num = sift_filenames.size(); 
		// sample a part of sift files
		size_t sample_size = siftfile_num;
		std::vector<size_t> siftfile_samples = RandomSample(siftfile_num, sample_size);

		size_t total_keys = 0; 
		std::vector<tw::SiftData> sift_data; 
		if(sift_type == 0) 
		{
			sift_data.resize(sample_size); 
			for(size_t i = 0; i < sample_size; i++) 
			{
				if(sizeof(DTYPE) == 1)
					sift_data[i].ReadSiftFile(sift_filenames[siftfile_samples[i]]); 
				else
					sift_data[i].ReadChar2DTYPE(sift_filenames[siftfile_samples[i]]);
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
	    for(size_t i = 0; i < sample_size; i++)
	    {
	    	int num_keys = sift_data[i].getFeatureNum();
	    	if(num_keys > 0	)
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
	        	if(sizeof(DTYPE) == 1)
	        		sift_data[i].ReadSiftFile(sift_filenames[i]);
	        	else
	        		sift_data[i].ReadChar2DTYPE(sift_filenames[i]);
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
	        std::cout << "[BuildDB] Add image #" << start_id + i << " to database\n";
	    } 
	    tree.ComputeTFIDFWeight(siftfile_num);
	    tree.NormalizeDatabase(start_id, siftfile_num);

	    std::cout << "[BuildDB] Write vocabulary tree (with image database) to " << output_filename << '\n';
	    tree.WriteTree(output_filename);
	    tree.ClearTree();
	    
	    return true;
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

	std::mutex match_file_mutex;
	void MultiQueryDatabase(vot::VocabTree *tree, 
							std::vector<tw::SiftData> *sift_data, 
							size_t first_index,
							size_t num_images, 
							float *scores, 
							tw::IndexedFloat *indexed_scores, 
							FILE *match_file)
	{
		size_t db_image_num = tree->database_image_num;
		for(size_t i = first_index; i < first_index + num_images; i++)
		{
			match_file_mutex.lock();
			std::cout << "[VocabMatch] Querying image #" << i << " to database\n";
			match_file_mutex.unlock();

			memset(scores, 0.0, sizeof(float) * db_image_num);
			tree->Query((*sift_data)[i], scores);
			for(size_t j = 0; j < db_image_num; j++)
			{
				indexed_scores[j].value = scores[j];
				indexed_scores[j].index = j;
			}
			qsort(indexed_scores, db_image_num, sizeof(tw::IndexedFloat), CompareIndexedFloat);

			match_file_mutex.lock();
			for(size_t j = 0; j < db_image_num; j++)
			{
				fprintf(match_file, "%zd %zd %0.4f\n", i, indexed_scores[j].index, indexed_scores[j].value);
			}
			match_file_mutex.unlock();
		}
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

			for(size_t i = 0; i < sift_filenames.size(); i++)
			{
				if(sizeof(DTYPE) == 1)
					sift_data[i].ReadSiftFile(sift_filenames[i]);
				else
					sift_data[i].ReadChar2DTYPE(sift_filenames[i]);
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
	    if(thread_num == 1)
	    {
		    float *scores = new float [db_image_num];
		    tw::IndexedFloat *indexed_scores = new tw::IndexedFloat [db_image_num];

		    for(size_t i = 0; i < siftfile_num; i++)
		    {
		    	std::cout << "[VocabMatch] Querying image #" << i << " to database\n";
		    	memset(scores, 0.0, sizeof(float) * db_image_num);
		    	tree->Query(sift_data[i], scores);
		    	for(size_t j = 0; j < db_image_num; j++)
		    	{
		    		indexed_scores[j].value = scores[j];
		    		indexed_scores[j].index = j;
		    	}
		    	qsort(indexed_scores, db_image_num, sizeof(tw::IndexedFloat), CompareIndexedFloat);
		    	for(size_t j = 0; j < db_image_num; j++)
		    	{
		    		fprintf(match_file, "%zd %zd %0.4f\n", i, indexed_scores[j].index, indexed_scores[j].value);
		    	}
		    }

		    delete [] scores;
		    delete [] indexed_scores;
	    }
	    else 	
	    {
	    	std::vector<std::thread> threads;
	    	float **scores = new float* [thread_num];
	    	tw::IndexedFloat **indexed_scores = new tw::IndexedFloat* [thread_num];
	    	for(int i = 0; i < thread_num; i++)
	    	{
	    		scores[i] = new float [db_image_num];
	    		indexed_scores[i] = new tw::IndexedFloat [db_image_num];
	    	}

	    	size_t off = 0;
	    	for(int i = 0; i < thread_num; i++)
	    	{
	    		size_t thread_image = siftfile_num / thread_num;
	    		if(i == thread_num - 1)
	    			thread_image = siftfile_num - (thread_num - 1) * thread_image;
		    	threads.push_back(std::thread(MultiQueryDatabase, tree, &sift_data, off, thread_image, scores[i], indexed_scores[i], match_file));
		    	off += thread_image;
	    	}
	    	std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));
	    	// release memory
	    	for(int i = 0; i < thread_num; i++)
	    	{
	    		delete [] scores[i];
	    		delete [] indexed_scores[i];
	    	}
	    	delete [] scores;
	    	delete [] indexed_scores;
	    }	
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
	    else 
	    {
	        std::cout << "[FilterMatchList] Read the vocabulary tree match file: " << match_list << '\n';
	    }

	    std::string line;
	    size_t index0, index1;
	    float score;
	    std::vector<std::vector<size_t> > top_matches;
	    top_matches.resize(image_num);
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
	        // find top-'num_matches' pairs
	        if(top_matches[index0].size() < num_matches && index0 != index1 && score > 0)
	        {
	        	top_matches[index0].push_back(index1);
	        }
	    }
	    fin.close();

	    std::vector<std::vector<size_t> > filtered_matches;
	    filtered_matches.resize(image_num);
	    for(size_t i = 0; i < image_num; i++)
	    {
	    	for(size_t j = 0; j < top_matches[i].size(); j++)
	    	{
	    		index0 = i; 
	    		index1 = top_matches[i][j];
	    		if(index0 > index1)
	    		{
	    			int temp = index1;
	    			index1 = index0;
	    			index0 = temp;
	    		}
	    		filtered_matches[index0].push_back(index1);
	    	}
	    }

	    // get unique match pairs
	    for(size_t i = 0; i < image_num; i++)
	    {
	    	std::sort(filtered_matches[i].begin(), filtered_matches[i].end());
	    	filtered_matches[i].erase(std::unique(filtered_matches[i].begin(), filtered_matches[i].end()), filtered_matches[i].end());
	    }

	    // write match pairs (names and indexes) to two files
		std::ofstream fout("match_pairs_idx.txt");		// used for match (sift filenames pairs)
		std::ofstream fout1("match_pairs_idx");			// used for query expansion (sift index pairs)
		if(!fout.is_open() || !fout1.is_open())
	    {
	        std::cout << "[FilterMatchList] Fail to open the filtered match file\n";
	        return false;
	    }
	    else 
	    {
	        std::cout << "[FilterMatchList] Write the filtered match file match_pairs_idx" << '\n';
	    }

	    for(size_t i = 0; i < image_num; i++)
	    {
	    	for(size_t j = 0; j < filtered_matches[i].size(); j++)
	    	{
	    		fout << sift_filenames[i] << " " << sift_filenames[filtered_matches[i][j]] << '\n';
	    	}
	    } 
	    fout.close();

	    for(size_t i = 0; i < image_num; i++)
	    {
	    	for(size_t j = 0; j < top_matches[i].size(); j++)
	    	{
	    		fout1 << i << " " << top_matches[i][j] << '\n';
	    	}
	    }
	    fout1.close();

		return true;	
	}
}	// end of namespace vot