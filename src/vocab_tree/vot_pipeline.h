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

#ifndef VOT_PIPELINE_HEADER
#define VOT_PIPELINE_HEADER

#include "../utils/global_params.h"

namespace vot
{
	bool BuildVocabTree(const char *sift_list, 
						const char *output_filename, 
						int depth = 6, int branch_num = 8, 
						SiftType sift_type = E3D_SIFT, int thread_num = 1);

	bool BuildImageDatabase(const char *sift_list, 
							const char *input_tree, 
							const char *output_filename, 
							SiftType sift_type = E3D_SIFT, 
							size_t start_id = 0, 
							int thread_num = 1);

	bool QueryDatabase(const char *image_db, 
					   const char *query_sift_list, 
					   const char *match_output, 
					   SiftType sift_type = E3D_SIFT, int thread_num = 1);

	bool FilterMatchList(const char *sift_list, const char *match_list, const char *output, size_t num_matches);
}

#endif