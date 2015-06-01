#ifndef VOCAB_TREE_HEADER
#define VOCAB_TREE_HEADER

#include <vector>
#include <cstdlib>

#include "utils/global_params.h"
#include "data_types.h"

namespace vot
{
    typedef enum
    {
        L1 = 0,
        L2 = 1 
    } DistanceType;

    ///
    /// \brief This class contains a index for an image and a count for image 
    ///
    class ImageCount 
    {
    public: 
        ImageCount() : index(0), count(0.0) { }
        ImageCount(size_t index_, float count_) : index(index_), count(count_) { }

        size_t index; 
        int count; 
    };

    ///
    /// \brief The virtual class of tree node
    ///
    class TreeNode
    {
        public:
            TreeNode():des(NULL)  {}
            virtual ~TreeNode()
            {
                if(des != NULL) 
                {
                    delete [] des;
                    des = NULL;
                }
            }
            // pure virtual function that will be implemented in derived classes
            virtual bool RecursiveBuild(int num_keys, int dim, int depth, int depth_curr, int bf, DTYPE **p , double *means, int *assign) = 0;
            virtual bool ClearNode(int bf) = 0;
            virtual bool WriteNode(FILE *f, int branch_num, int dim) const = 0;
            virtual bool ReadNode(FILE *f, int branch_num, int dim) = 0;
            virtual size_t CountNodes(int branch_num) const = 0;
            virtual size_t CountLeaves(int branch_num) const = 0;
            virtual bool Compare(TreeNode *in, int branch_num, int dim) const = 0;

            DTYPE *des; //!< the descriptor vector
            size_t id; //!< the id of the node 
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
            virtual bool WriteNode(FILE *f, int branch_num, int dim) const;
            virtual bool ReadNode(FILE *f, int branch_num, int dim);
            virtual size_t CountNodes(int branch_num) const ;
            virtual size_t CountLeaves(int branch_num) const ;
            virtual bool Compare(TreeNode *in, int branch_num, int dim) const;

            TreeNode **children;
    };

    ///
    /// \brief The leaf node class of the tree
    ///
    class TreeLeafNode: public TreeNode
    {
        public:
            TreeLeafNode():score(0.0), weight(0.0) {}
            virtual ~TreeLeafNode();
            virtual bool RecursiveBuild(int num_keys, int dim, int depth, int depth_curr, int bf, DTYPE **p , double *means, int *assign);
            virtual bool ClearNode(int bf);
            virtual bool WriteNode(FILE *f, int branch_num, int dim) const;
            virtual bool ReadNode(FILE *f, int branch_num, int dim);
            virtual size_t CountNodes(int branch_num) const;
            virtual size_t CountLeaves(int branch_num) const;
            virtual bool Compare(TreeNode *leaf, int branch_num, int dim) const;

            float score;
            float weight;
            std::vector<ImageCount> inv_list;
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
            bool WriteTree(const char *filename) const;       //!< save the vocabulary tree in a file
            bool ReadTree(const char *filename);            //!< read a vocabulary tree from a file
            bool ClearTree();                   //!< release the memory
            bool Compare(VocabTree &v) const; //!< compare two vocabulary tree and returns whether they are the same
            bool AddImage2Tree(tw::SiftData &sift);          //!< add an image into the database
            void Show() const;      //!< a test function


            // public data member 
            int branch_num;             //!< the branch number of a node
            int depth;                  //!< the depth of the tree
            int database_image_num;     //!< the number of the database images
            int dim;                    //!< the dimension of the descriptor
            size_t num_nodes;           //!< the number of nodes in the tree
            DistanceType dis_type;      //!< the distance type
            TreeNode *root;             //!< the root of the tree
    };

}   // end of namespace vot

#endif  //VOCAB_TREE_HEADER
