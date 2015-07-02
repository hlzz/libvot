#ifndef VOT_PIPELINE_HEADER
#define VOT_PIPELINE_HEADER

namespace vot
{
	bool BuildVocabTree(const char *sift_list, 
						const char *output_filename, 
						int depth = 6, int branch_num = 8, 
						int sift_type = 0, int thread_num = 1);

	bool BuildImageDatabase(const char *sift_list, 
							const char *input_tree, 
							const char *output_filename, 
							int sift_type = 0, 
							size_t start_id = 0, 
							int thread_num = 1);

	bool QueryDatabase(const char *image_db, 
					   const char *query_sift_list, 
					   const char *match_output, 
					   int sift_type = 0, int thread_num = 1);

	bool FilterMatchList(const char *sift_list, const char *match_list, const char *output, int num_matches);
}

#endif