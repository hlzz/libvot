#include <iostream>
#include <cmath>
#include <cassert>

#include "vocab_tree.h"
#include "clustering.h"

namespace vot
{
    /** VocabTree Class Implementation */
    VocabTree::VocabTree():database_image_num(0), dis_type(L2), root(NULL) {}

    VocabTree::VocabTree(int depth_, int branch_num_, int dim_, DistanceType dis_type_): 
    branch_num(branch_num_), depth(depth_), dim(dim_), dis_type(dis_type_) {};

 
    VocabTree::~VocabTree() 
    {
        if(root != NULL)
            delete root;
    }

    bool VocabTree::BuildTree(int num_keys, int dim_, int dep, int bf, DTYPE **p)
    {
        if(dep < 1)     // the root of the tree is depth 0
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

        double *means = new double [branch_num * dim];
        int *assign = new int [num_keys];
        if(means == NULL || assign == NULL)
        {
            std::cout << "[VocabTree Build] Error allocating memory in K-means\n";
            return false;
        }

        root = new TreeInNode();
        root->des = new DTYPE [dim];
        for(int i = 0; i < dim; i++)
            root->des[i] = 0;

        if(!root->RecursiveBuild(num_keys, dim, depth, 0, branch_num, p, means, assign))
            return false;

        delete [] means;
        delete [] assign;

        std::cout << "[VocabTree Build] Finish building vocabulary tree!\n";
        return true;
    }

    bool VocabTree::WriteTree(const char *filename)
    {
        return true;
    }

    // destruct this vocabulary tree
    bool VocabTree::ClearTree()
    {
        if(root != NULL)
        {
            root->ClearNode(branch_num);
            root = NULL;
        } 
        return true;
    }










    /** TreeLeafNode Class Implementation */
    bool TreeLeafNode::RecursiveBuild(int num_keys, int dim, int depth, int depth_curr, int bf, DTYPE **p, double *means, int *assign)
    {
        std::cout << "depth " << depth_curr << '\n';
        for(int i = 0; i < dim; i++)
        {
            std::cout << (double)des[i] << " ";
        }
        std::cout << '\n';
        return true;
    }

    bool TreeLeafNode::ClearNode(int bf)
    {
        delete this;
        return true;
    }













    /** TreeInNode Class Implementation */
    TreeInNode::~TreeInNode()
    {
        if(children != NULL)
            delete [] children;
    }

    bool TreeInNode::RecursiveBuild(int num_keys, int dim, int depth, int depth_curr, int bf, DTYPE **p, double *means, int *assign)
    {
        if(GlobalParam::Verbose)
            std::cout << "[RecursiveBuild] K-means in depth " << depth_curr << "\n";

        double error = tw::Kmeans(num_keys, dim, bf, p, means, assign);
        if(std::abs(error + 1) < 10e-6)
        {
            std::cerr << "[Error] Error in TreeInNode::RecursiveBuild\n";
            return false;
        }

        // the average distance between each cluster and the node descriptor
        double mean_distance = 0.0;
        for(int i = 0; i < bf; i++)
        {
            for(int j = 0; j < dim; j++)
            {
                mean_distance += (means[i*dim + j] - des[j]) * (means[i*dim + j] - des[j]);
            }
        }
        mean_distance /= bf;
        if(GlobalParam::Verbose)
            std::cout << "[RecursiveBuild] Average distance of node to clusters: " << mean_distance << "\n";

        // count the number of sift keys fallen into a interior node, stop split the node if there are too few keys.
        children = new TreeNode* [bf];
        size_t *counts = new size_t [bf];
        for(int i = 0; i < bf; i++)
            counts[i] = 0;
        for(int i = 0; i < num_keys; i++)
            counts[assign[i]]++;

        for(int i = 0; i < bf; i++)
        {
            if(counts[i] > 0)    // there are some keys fallen into this node
            {
                if(depth_curr == depth || counts[i] < 2*bf)
                    children[i] = new TreeLeafNode();
                else
                    children[i] = new TreeInNode();
                children[i]->des = new DTYPE [dim];
                for(int j = 0; j < dim; j++)
                    children[i]->des[j] = (int) (means[i*dim + j] + 0.5);     // round to the nearest integer
            }
            else
            {
                if(GlobalParam::Verbose)
                    std::cout << "[RecursiveBuild] No keys in this node\n";
                children[i] = NULL;
            }
        }

        // rearrange the pointer array so that after rearragement, the sift keys in the first cluster appear consecutively in the array first,
        // then the second cluster, so on and so forth
        size_t idx = 0;
        size_t start_idx = 0;
        for(size_t i = 0; i < bf; i++)
        {
            for(size_t j = start_idx; j < num_keys; j++)
            {
                if(assign[j] == i)
                {
                    // swap the pointer
                    DTYPE *temp = p[idx];
                    p[idx] = p[j];
                    p[j] = temp;
                    // swap the assignment
                    unsigned int temp_assign = assign[idx];
                    assign[idx] = assign[j];
                    assign[j] = temp_assign;

                    idx++;
                }
            }    
            start_idx += counts[i];
        }
        assert(start_idx == num_keys);

        // recursively build the tree in the children nodes
        size_t offset = 0;
        for(int i = 0; i < bf; i++)
        {
            if(children[i] != NULL)
            {
                children[i]->RecursiveBuild(counts[i], dim, depth, depth_curr+1, bf, p+offset, means, assign+offset);
            }
        }

        delete [] counts;
        return true;
    }

    bool TreeInNode::ClearNode(int bf)
    {
        for(int i = 0; i < bf; i++)
        {
            if(children[i] != NULL)
                children[i]->ClearNode(bf);
        }
        delete this;
        return true;
    }

}   // end of namespace vot
