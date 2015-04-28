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

#include <iostream>
#include <cmath>

#include "vocab_tree.h"
#include "utils/global_params.h"

namespace vot
{
    VocabTree::VocabTree():database_image_num(0), dis_type(L2) {}

    VocabTree::VocabTree(int depth_, int branch_num_, int dim_, DistanceType dis_type_): 
    branch_num(branch_num_), depth(depth_), dim(dim_), dis_type(dis_type_) {};

    VocabTree::~VocabTree() {}

    /** VocabTree Class Implementation */
    bool VocabTree::BuildTree(int n, int dim_, int dep, int bf, DTYPE **p)
    {
        if(dep <= 1)
        {
            std::cout << "[FATAL_ERROR] The depth of the tree should be larger than 1!\n";
            return false;
        }

        branch_num = bf;
        depth = dep;
        dim = dim_;

        if(GlobalParam::Verbose)
        {
            std::cout << "[VocabTree Build] Begin Build Vocabulary Tree ...\n";
            std::cout << "[VocabTree Build] with depth " << dep << " and branch number " << bf << ".\n";
            std::cout << "[VocabTree Build] Approxiamately " << (float)dim * pow(bf, dep+1)/(1024 * 1024 * (bf-1)) << "mb memory will be used to load the tree.\n";
        }

        return 0;
    }
}   // end of namespace vot
