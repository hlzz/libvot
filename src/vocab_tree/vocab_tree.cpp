#include <iostream>
#include <cmath>
#include <cassert>

#include "vocab_tree.h"
#include "clustering.h"

using std::cout;
using std::endl;

namespace vot
{
    /** VocabTree Class Implementation */
    VocabTree::VocabTree():database_image_num(0), num_nodes(0), dis_type(L2), root(NULL) {}

    VocabTree::VocabTree(int depth_, int branch_num_, int dim_, DistanceType dis_type_): 
    branch_num(branch_num_), depth(depth_), dim(dim_), dis_type(dis_type_), num_nodes(0) {};
 
    VocabTree::~VocabTree() {root = NULL;}  // do nothing since root is undetermined

    TreeInNode::~TreeInNode()
    {
        if(children != NULL)
        {
            delete [] children;
            children = NULL;
        }
    }

    TreeLeafNode::~TreeLeafNode()
    {
        inv_list.clear();
    }




    //////////////////////////////////////////////////////////////////////////////
    //                                                                          //
    //                     Vocabulary Tree Clear Funtions                       //
    //                                                                          //
    //////////////////////////////////////////////////////////////////////////////
    bool VocabTree::ClearTree()
    {
        if(root != NULL)
        {
            root->ClearNode(branch_num);
        } 

        std::cout << "[VocabTree] Successfully clearing the tree\n";
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

    bool TreeLeafNode::ClearNode(int bf)
    {
        delete this;
        return true;
    }




    //////////////////////////////////////////////////////////////////////////////
    //                                                                          //
    //                     Vocabulary Tree Build Module                         //
    //                                                                          //
    //////////////////////////////////////////////////////////////////////////////
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
            std::cout << "[RecursiveBuild] Group/Center error: " << error/num_keys << "/" << mean_distance << "\n";

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

    bool TreeLeafNode::RecursiveBuild(int num_keys, int dim, int depth, int depth_curr, int bf, DTYPE **p, double *means, int *assign)
    {
        return true;
    }







    //////////////////////////////////////////////////////////////////////////////
    //                                                                          //
    //                     Vocabulary Tree IO Module                            //
    //                                                                          //
    //////////////////////////////////////////////////////////////////////////////

    bool VocabTree::WriteTree(const char *filename) const
    {
        if(root == NULL)
            return false;
        FILE *f = fopen(filename, "wb");
        if(f == NULL)
        {
            std::cout << "[VocabTree] Error opening file " << filename << " for writing tree\n" << std::endl;
            return false;
        }

        // write header parameters
        fwrite(&branch_num, sizeof(int), 1, f);
        fwrite(&depth, sizeof(int), 1, f);
        fwrite(&dim, sizeof(int), 1, f);

        // write node information recursively
        root->WriteNode(f, branch_num, dim);
        fclose(f);

        return true;
    }

    bool TreeInNode::WriteNode(FILE *f, int branch_num, int dim) const
    {
        // write the current node's information
        char *has_children = new char [branch_num];
        char is_internal = 1;
        fwrite(&is_internal, sizeof(char), 1, f);
        fwrite(des, sizeof(DTYPE), dim, f);
        for(int i = 0; i < branch_num; i++)
        {
            if(children[i] == NULL)
            {
                has_children[i] = 0;
            }
            else
            {
                has_children[i] = 1;
            }
        }
        fwrite(has_children, sizeof(char), branch_num, f);
        delete [] has_children;

        // recursively write children's information
        for(int i = 0; i < branch_num; i++)
        {
            if(children[i] != NULL)
            {
                children[i]->WriteNode(f, branch_num, dim);
            }
        }
        return true;
    }

    bool TreeLeafNode::WriteNode(FILE *f, int branch_num, int dim) const
    {
        char is_internal = 0;
        fwrite(&is_internal, sizeof(char), 1, f);
        fwrite(des, sizeof(DTYPE), dim, f);
        fwrite(&weight, sizeof(float), 1, f);

        int num_images = (size_t) inv_list.size();
        fwrite(&num_images, sizeof(int), 1, f);
        for(size_t i = 0; i < num_images; i++)
        {
            int img = inv_list[i].index;    // WARNING: the original type is size_t
            int count = inv_list[i].count;
            fwrite(&img, sizeof(int), 1, f);    // to save space we use int
            fwrite(&count, sizeof(int), 1, f);
        }

        return true;
    }

    // Read a vocabulary tree
    bool VocabTree::ReadTree(const char *filename)
    {
        if(root != NULL)
            ClearTree();
        FILE *f = fopen(filename, "r");
        if(f == NULL)
        {
            std::cout << "[ReadTree] Error opening file " << filename << " for reading tree\n" << std::endl;
            return false;
        }

        // write header parameters
        char is_internal;
        int a, b, c, d;
        a = fread(&branch_num, sizeof(int), 1, f);
        b = fread(&depth, sizeof(int), 1, f);
        c = fread(&dim, sizeof(int), 1, f);
        d = fread(&is_internal, sizeof(char), 1, f);
        if(a != 1 || b != 1 || c != 1 || d != 1)
        {
            std::cout << "[ReadTree] Reading Error\n";
            return false;
        }

        root = new TreeInNode();
        root->ReadNode(f, branch_num, dim);

        //char end = fgetc(f);
        //cout << (end == EOF) << endl;
        fclose(f);
        num_nodes = root->CountNodes(branch_num);
        return true;
    }

    bool TreeInNode::ReadNode(FILE *f, int branch_num, int dim)
    {
        des = new DTYPE [dim];
        int a = fread(des, sizeof(DTYPE), dim, f);
        if(a != dim)
        {
            std::cout << "[ReadNode] Reading error\n";
            return false;
        }
        char *has_children = new char [branch_num];
        int b = fread(has_children, sizeof(char), branch_num, f);
        if(b != branch_num)
        {
            std::cout << "[ReadNode] Reading error\n";
            return false;
        }

        children = new TreeNode *[branch_num];
        for(int i = 0; i < branch_num; i++)
        {
            if(has_children[i] == 0)
            {
                children[i] = NULL;
            }
            else
            {
                char is_internal;
                int c = fread(&is_internal, sizeof(DTYPE), 1, f);
                if(c != 1)
                {
                    std::cout << "[ReadNode] Reading error\n";
                    return false;
                }
                if(is_internal)
                {
                    children[i] = new TreeInNode();
                }
                else
                {
                    children[i] = new TreeLeafNode();
                }
                children[i]->ReadNode(f, branch_num, dim);
            }
        }

        delete [] has_children;

        return true;
    }

    bool TreeLeafNode::ReadNode(FILE *f, int branch_num, int dim)
    {
        des = new DTYPE [dim];
        int a = fread(des, sizeof(DTYPE), dim, f);
        if(a != dim)
        {
            std::cout << "[ReadNode] Reading error\n";
            return false;
        }
        int num_images;
        int b = fread(&weight, sizeof(float), 1, f);
        int c = fread(&num_images, sizeof(int), 1, f);
        if(b != 1 || c != 1)
        {
            std::cout << "[ReadNode] Reading error\n";
            return false;
        }

        inv_list.resize(num_images);
        for(int i = 0; i < num_images; i++)
        {
            int img, count;
            int d = fread(&img, sizeof(int), 1, f);
            int e = fread(&count, sizeof(int), 1, f);
            if(d != 1 || e != 1)
            {
                std::cout << "[ReadNode] Reading error\n";
                return false;
            }
            inv_list[i] = ImageCount(img, count);
        }
        return true;
    }

    //////////////////////////////////////////////////////////////////////////////
    //                                                                          //
    //                     Vocabulary Tree Utilities(Test)                      //
    //                                                                          //
    //////////////////////////////////////////////////////////////////////////////

    void VocabTree::Show() const
    {
        std::cout << "[VocabTree] depth/branch_num: " << depth << "/" << branch_num << std::endl;
        std::cout << "[VocabTree] #nodes " << num_nodes << std::endl;
    }

    size_t TreeInNode::CountNodes(int branch_num) const
    {
        size_t num_nodes = 0;
        for(int i = 0; i < branch_num; i++)
        {
            if(children[i] != NULL)
                num_nodes += children[i]->CountNodes(branch_num);
        }
        return num_nodes + 1;
    }

    size_t TreeLeafNode::CountNodes(int branch_num) const
    {return 1;}

    size_t TreeInNode::CountLeaves(int branch_num) const
    {
        size_t num_leaves = 0;
        for(int i = 0; i < branch_num; i++)
        {
            if(children[i] != NULL)
                num_leaves += children[i]->CountLeaves(branch_num);
        }
        return num_leaves;
    }

    size_t TreeLeafNode::CountLeaves(int branch_num) const
    {return 1;}

    bool VocabTree::Compare(VocabTree & v) const
    {
        if(v.dim != dim || v.depth != depth || v.branch_num != branch_num)
        {
            return false;
        }

        size_t node_count = root->CountNodes(branch_num);
        size_t other_node_count = v.root->CountNodes(branch_num);
        if(node_count != other_node_count)
        {
            return false;
        }

        size_t leave_count = root->CountLeaves(branch_num);
        size_t other_leave_count = v.root->CountLeaves(branch_num);
        if(leave_count != other_leave_count)
        {
            return false;
        }

        return root->Compare(v.root, branch_num, dim);
    }

    bool TreeInNode::Compare(TreeNode *in, int branch_num, int dim) const 
    {
        TreeInNode *other_node = dynamic_cast<TreeInNode*>(in);
        for(int i = 0; i < dim; i++)
        {
            if(des[i] != other_node->des[i])
                return false;
        }

        for(int i = 0; i < branch_num; i++)
        {
            if(children[i] == NULL)
            {
                if(other_node->children[i] != NULL)
                    return false;
            }
            else
            {
                if(other_node->children[i] == NULL)
                    return false;
                children[i]->Compare(other_node->children[i], branch_num, dim);
            }
        }
        return true;
    }

    bool TreeLeafNode::Compare(TreeNode *leaf, int branch_num, int dim) const
    {
        TreeLeafNode *other_leaf = dynamic_cast<TreeLeafNode*>(leaf);
        for(int i = 0; i < dim; i++)
        {
            if(des[i] != other_leaf->des[i])
                return false;
        }
        if(inv_list.size() != other_leaf->inv_list.size())    // shallow comparison
            return false;
        return true;
    } 

    //////////////////////////////////////////////////////////////////////////////
    //                                                                          //
    //                     Vocabulary Tree Build Database                       //
    //                                                                          //
    //////////////////////////////////////////////////////////////////////////////

    bool VocabTree::AddImage2Tree(tw::SiftData &sift)
    {
        return true;
    }

}   // end of namespace vot
