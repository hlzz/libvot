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

#ifndef VOCAB_TREE_HEADER
#define VOCAB_TREE_HEADER

#include <vector>

#include "utils/global_params.h"

namespace vot
{
    typedef enum
    {
        L1 = 0,
        L2 = 1 
    } DistanceType;

    ///
    /// \brief The virtual class of tree node
    ///
    class TreeNode
    {
        public:
            TreeNode()  {}
            virtual ~TreeNode() {}

            DTYPE *des; //!< the descriptor vector
            unsigned long id; //!< the id of the node 
    };

    ///
    /// \brief The leaf node class of the tree
    ///
    class TreeLeafNode: public TreeNode
    {
        public:
            TreeLeafNode() {}
            ~TreeLeafNode() {}
    };

    ///
    /// \brief The interior node class of the tree
    ///
    class TreeInNode: public TreeNode
    {
        public:
            TreeInNode() {}
            ~TreeInNode() {}
    };

    ///
    /// \brief The vocabulary tree class
    ///
    class VocabTree
    {
        public:
            // constructors and destructors
            VocabTree();
            VocabTree(int depth_, int branch_num_, int dim_ = 128, DistanceType dis_type_ = L2);
            ~VocabTree();

            // member function 
            bool BuildTree(int num_keys, int dim, int dep, int bf, DTYPE **p);      //!< build a vocabulary tree from a set of features
            bool BuildImageDatabase();      //!< populate the images through the tree
            bool Query();           //!< query a image against the database
            bool WriteTree(const char *filename);       //!< save the vocabulary tree in a file

            // public data member 
            int branch_num;             //!< the branch number of a node
            int depth;                  //!< the depth of the tree
            int database_image_num;     //!< the number of the database images
            int dim;                    //!< the dimension of the descriptor
            unsigned long num_nodes;    //!< the number of nodes in the tree
            DistanceType dis_type;      //!< the distance type
            TreeNode *root;             //!< the root of the tree
    };

}   // end of namespace vot

#endif  //VOCAB_TREE_HEADER
