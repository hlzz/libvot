#include <iostream>
#include <string>
#include "image_graph.h"

namespace vot
{
    int ImageGraph::NumConnectedComponents()
    {
        return 1;
    }

    bool ImageGraph::SetAdjLists(std::vector<LinkNode> *adj_lists)
    {
        adj_lists_ = adj_lists;
        return true;
    }

    std::vector<int>* ImageGraph::GraphPartition()
    {
        std::vector<int>* parts = NULL;

        return parts;
    }

    void ImageGraph::ShowInfo()
    {
        std::cout << "[ImageGraph] Size: " << size_ << "\n";
        if(adj_lists_ != NULL)
        {
            for(int i = 0; i < size_; i++)
            {
                for(int j = 0; j < adj_lists_[i].size(); j++)
                {
                    std::cout << adj_lists_[i][j].src << " " << 
                        adj_lists_[i][j].dst << " " << adj_lists_[i][j].score << "\n";
                }
            }
        }
    }

}   // end of namespace vot
