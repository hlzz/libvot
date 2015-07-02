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
        L1 = 1,
        L2 = 2 
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
        float count; 
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
            virtual bool RecursiveBuild(size_t num_keys, int dim, int depth, int depth_curr, int bf, DTYPE **p , double *means, int *assign, int thread_num = 1) = 0;
            virtual bool ClearNode(int bf) = 0;
            virtual bool WriteNode(FILE *f, int branch_num, int dim) const = 0;
            virtual bool ReadNode(FILE *f, int branch_num, int dim) = 0;
            virtual size_t CountNodes(int branch_num) const = 0;
            virtual size_t CountLeaves(int branch_num) const = 0;
            virtual bool Compare(TreeNode *in, int branch_num, int dim) const = 0;
            virtual bool ClearScores(int bf) = 0;             //!< refresh the temporary score for this tree
            virtual size_t DescendFeature(DTYPE *v, size_t image_index, int branch_num, int dim, bool add = true) = 0;
            virtual double ComputeImageVectorMagnitude(int bf, DistanceType dt) = 0;
            virtual bool SetConstantWeight(int bf) = 0;   //!< set a constant weight to the leaf nodes
            virtual bool ComputeTFIDFWeight(int bf, size_t n) = 0;  //!< compute TF-IDF weight and pre-apply weight adjusting to inverted lists
            virtual bool ComputeDatabaseMagnitude(int bf, DistanceType dis_type, size_t start_id, std::vector<float> &database_mag) = 0; //!< compute the vector magnitude of all images in the database
            virtual bool NormalizeDatabase(int bf, size_t start_id, std::vector<float> &database_mag) = 0;   //!< normalize the inverted list score by the magnitude of image vector
            // member functions for querying database
            virtual bool IndexLeaves(int branch_num) = 0;
            virtual bool FillQueryVector(float *q, int branch_num, float normalize_factor) = 0;     //!< fill the query vector
            virtual bool ScoreQuery(float *q, int branch_num, DistanceType dt, float *scores) = 0;       //!< score each image in the database
            virtual size_t MultiDescendFeature(float *q, DTYPE *v, size_t image_index, int branch_num, int dim) = 0;    //!< used for query database

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
            virtual bool RecursiveBuild(size_t num_keys, int dim, int depth, int depth_curr, int bf, DTYPE **p, double *means, int *assign, int thread_num = 1);
            virtual bool ClearNode(int bf);
            virtual bool WriteNode(FILE *f, int branch_num, int dim) const;
            virtual bool ReadNode(FILE *f, int branch_num, int dim);
            virtual size_t CountNodes(int branch_num) const ;
            virtual size_t CountLeaves(int branch_num) const ;
            virtual bool Compare(TreeNode *in, int branch_num, int dim) const;
            virtual bool ClearScores(int bf);             //!< refresh the temporary score for this tree
            // function for build image database
            virtual size_t DescendFeature(DTYPE *v, size_t image_index, int branch_num, int dim, bool add = true);
            virtual double ComputeImageVectorMagnitude(int bf, DistanceType dt);
            virtual bool SetConstantWeight(int bf);   //!< set a constant weight to the leaf nodes
            virtual bool ComputeTFIDFWeight(int bf, size_t n);  //!< compute TF-IDF weight and pre-apply weight adjusting to inverted lists
            virtual bool ComputeDatabaseMagnitude(int bf, DistanceType dis_type, size_t start_id, std::vector<float> &database_mag); //!< compute the vector magnitude of all images in the database
            virtual bool NormalizeDatabase(int bf, size_t start_id, std::vector<float> &database_mag);   //!< normalize the inverted list score by the magnitude of image vector
            // member functions for querying database
            virtual bool IndexLeaves(int branch_num);
            virtual bool FillQueryVector(float *q, int branch_num, float normalize_factor);     //!< fill the query vector
            virtual bool ScoreQuery(float *q, int branch_num, DistanceType dt, float *scores);       //!< score each image in the database
            virtual size_t MultiDescendFeature(float *q, DTYPE *v, size_t image_index, int branch_num, int dim);    //!< used for query database

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
            virtual bool RecursiveBuild(size_t num_keys, int dim, int depth, int depth_curr, int bf, DTYPE **p , double *means, int *assign, int thread_num = 1);
            virtual bool ClearNode(int bf);
            virtual bool WriteNode(FILE *f, int branch_num, int dim) const;
            virtual bool ReadNode(FILE *f, int branch_num, int dim);
            virtual size_t CountNodes(int branch_num) const;
            virtual size_t CountLeaves(int branch_num) const;
            virtual bool Compare(TreeNode *leaf, int branch_num, int dim) const;
            virtual bool ClearScores(int bf);             //!< refresh the temporary score for this tree
            // function for build image database
            virtual size_t DescendFeature(DTYPE *v, size_t image_index, int branch_num, int dim, bool add = true);
            virtual double ComputeImageVectorMagnitude(int bf, DistanceType dt);
            virtual bool SetConstantWeight(int bf);   //!< set a constant weight to the leaf nodes
            virtual bool ComputeTFIDFWeight(int bf, size_t n);  //!< compute TF-IDF weight and pre-apply weight adjusting to inverted lists
            virtual bool ComputeDatabaseMagnitude(int bf, DistanceType dis_type, size_t start_id, std::vector<float> &database_mag); //!< compute the vector magnitude of all images in the database
            virtual bool NormalizeDatabase(int bf, size_t start_id, std::vector<float> &database_mag);   //!< normalize the inverted list score by the magnitude of image vector
            // member functions for querying database
            virtual bool IndexLeaves(int branch_num);
            virtual bool FillQueryVector(float *q, int branch_num, float normalize_factor);     //!< fill the query vector
            virtual bool ScoreQuery(float *q, int branch_num, DistanceType dt, float *scores);       //!< score each image in the database
            virtual size_t MultiDescendFeature(float *q, DTYPE *v, size_t image_index, int branch_num, int dim);    //!< used for query database

            float score;            //!< temporary score, for querying and computing magnitude use
            float weight;           //!< weight for this node
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

            // member function for building vocabulary tree
            bool BuildTree(size_t num_keys, int dim, int depth, int bf, DTYPE **p, int thread_num = 1);      //!< build a vocabulary tree from a set of features
            bool WriteTree(const char *filename) const;       //!< save the vocabulary tree in a file
            bool ReadTree(const char *filename);            //!< read a vocabulary tree from a file
            bool ClearTree();                   //!< release the memory
            bool Compare(VocabTree &v) const; //!< compare two vocabulary tree and returns whether they are the same
            // member function for building image database
            double AddImage2Tree(size_t image_index, tw::SiftData &sift, int thread_num);   //!< add an image into the database (support multi-thread)
            void Show() const;      //!< a test function
            bool SetConstantWeight();   //!< set a constant weight to the leaf nodes
            bool ComputeTFIDFWeight(size_t image_num);  //!< compute TF-IDF weight and pre-apply weight adjusting to inverted lists
            bool NormalizeDatabase(size_t start_id, size_t image_num);    //!< normalize the inverted list score by the magnitude of image vector
            // member function for querying database
            bool Query(tw::SiftData &sift, float *scores);   //!< query database and return the scores
            size_t IndexLeaves(); //!< index the leaf nodes

            // public data member 
            int branch_num;             //!< the branch number of a node
            int depth;                  //!< the depth of the tree
            int dim;                    //!< the dimension of the descriptor
            size_t database_image_num;  //!< the number of the database images
            size_t num_nodes;           //!< the number of nodes in the tree
            size_t num_leaves;          //!< the number of leaf nodes in the tree
            DistanceType dis_type;      //!< the distance type
            TreeNode *root;             //!< the root of the tree
    };

}   // end of namespace vot

#endif  //VOCAB_TREE_HEADER
