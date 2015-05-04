#ifndef VOCAB_TREE_HEADER
#define VOCAB_TREE_HEADER

#include <vector>
#include <cstdlib>

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
            TreeNode():des(NULL)  {}
            virtual ~TreeNode()
            {
                if(des != NULL) delete [] des;
            }
            // pure virtual function that will be implemented in derived classes
            virtual bool RecursiveBuild(int num_keys, int dim, int depth, int depth_curr, int bf, DTYPE **p , double *means, int *assign) = 0;
            virtual bool ClearNode(int bf) = 0;

            DTYPE *des; //!< the descriptor vector
            size_t id; //!< the id of the node 
    };

    ///
    /// \brief The leaf node class of the tree
    ///
    class TreeLeafNode: public TreeNode
    {
        public:
            TreeLeafNode() {}
            virtual ~TreeLeafNode() {}
            virtual bool RecursiveBuild(int num_keys, int dim, int depth, int depth_curr, int bf, DTYPE **p , double *means, int *assign);
            virtual bool ClearNode(int bf);

            std::vector<size_t> inv_list;
    };

    ///
    /// \brief The interior node class of the tree
    ///
    class TreeInNode: public TreeNode
    {
        public:
            TreeInNode():children(NULL) {}
            virtual ~TreeInNode();
            virtual bool RecursiveBuild(int num_keys, int dim, int depth, int depth_curr, int bf, DTYPE **p, double *means, int *assign);
            virtual bool ClearNode(int bf);

            TreeNode **children;
    };

    ///
    /// \brief The vocabulary tree class. The depth of the root is 0. The depth of the leaf nodes is depth+1.
    ///
    class VocabTree
    {
        public:
            // constructors and destructors
            VocabTree();
            VocabTree(int depth_, int branch_num_, int dim_ = 128, DistanceType dis_type_ = L2);
            ~VocabTree();

            // member function 
            bool BuildTree(int num_keys, int dim, int depth, int bf, DTYPE **p);      //!< build a vocabulary tree from a set of features
            bool BuildImageDatabase();      //!< populate the images through the tree
            bool Query();           //!< query a image against the database
            bool WriteTree(const char *filename);       //!< save the vocabulary tree in a file
            bool ClearTree();

            // public data member 
            int branch_num;             //!< the branch number of a node
            int depth;                  //!< the depth of the tree
            int database_image_num;     //!< the number of the database images
            int dim;                    //!< the dimension of the descriptor
            size_t num_nodes;    //!< the number of nodes in the tree
            DistanceType dis_type;      //!< the distance type
            TreeNode *root;             //!< the root of the tree
    };

}   // end of namespace vot

#endif  //VOCAB_TREE_HEADER
