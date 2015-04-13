#ifndef VOCAB_TREE_HEADER
#define VOCAB_TREE_HEADER

#include <vector>

#define DTYPE unsigned char
#define LTYPE float

namespace vot
{
    typedef enum
    {
        Euclidean = 0;
    } DistanceType;

    ///
    /// \brief The virtual class of tree node
    ///
    class TreeNode
    {
        public:
            TreeNode(des=NULL)  {}
            virtual ~TreeNode() {}

            DTYPE *des; //!< the descriptor vector
            unsigned long id //!< the id of the node 
    }

    ///
    /// \brief The leaf node class of the tree
    ///
    class TreeLeafNode: public TreeNode
    {
        public:
            TreeLeafNode {}
            ~TreeLeafNode {}
    }

    ///
    /// \brief The interior node class of the tree
    ///
    class TreeInNode: public TreeNode
    {
        public:
            TreeInNode() {}
            ~TreeInNode() {}
    }

    ///
    /// \brief The vocabulary tree class
    ///
    class VocabTree
    {
        public:
            VocabTree():database_image_num(0), dis_type(Euclidean) {}
            ~VocabTree() {}

            int branch_num;             //!< the branch number of a node
            int depth;                  //!< the depth of the tree
            int database_image_num;     //!< the number of the database images
            int dim;                    //!< the dimension of the descriptor
            unsigned long num_nodes;    //!< the number of nodes in the tree
            DistanceType dis_type;      //!< the distance type
            VocabTreeNode *root;        //!< the root of the tree
    }

}   // end of namespace vot

#endif  //VOCAB_TREE_HEADER
