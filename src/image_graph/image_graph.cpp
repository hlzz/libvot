#include <iostream>
#include <string>
#include <queue>
#include <cstdlib>
#include <cmath>
#include "image_graph.h"
#include "data_structures.h"

using std::cout;
using std::endl;

namespace vot
{
    ImageGraph::ImageGraph(int size) 
    {
        size_ = size;
        adj_lists_.resize(size);
    }

    ImageGraph::ImageGraph(const std::vector<std::string> &image_filenames, const std::vector<std::string> &sift_filenames)
    {
        size_ = image_filenames.size();
        image_filenames_ = image_filenames;
        sift_filenames_ = sift_filenames;
        assert(sift_filenames.size() == image_filenames.size());
        adj_lists_.resize(size_);
    }

    void ImageGraph::addEdge(int src, int dst, double score)
    {
        adj_lists_[src].push_back(vot::LinkNode(src, dst, score));
    }

    int ImageGraph::NumConnectedComponents(int threshold)
    {
        bool is_visited[size_];
        memset(is_visited, false, sizeof(size_));
        int numCC = 0;
        for(int i = 0; i < size_; i++)
        {
            if(!is_visited[i])
            {
                numCC++;
                std::queue<int> index_queue;
                is_visited[i] = true;
                index_queue.push(i);
                int component_size = 1;
                while(!index_queue.empty())
                {
                    int curr = index_queue.front();    
                    index_queue.pop();
                    for(int i = 0; i < adj_lists_[curr].size(); i++)
                    {
                        if(!is_visited[adj_lists_[curr][i].dst])
                        {
                            is_visited[adj_lists_[curr][i].dst] = true;
                            component_size++;
                            index_queue.push(adj_lists_[curr][i].dst);
                        }
                    }
                }
                if(component_size < threshold && threshold != 0)
                    numCC--;
            }
        }
        return numCC;
    }

    std::vector<std::vector<int> > ImageGraph::KargerCut()
    {
        std::vector<int> global_min_cut1, global_min_cut2;
        // randomly select an edge
        std::vector<std::pair<int, int> > edges;
        for(int i = 0; i < size_; i++)
            for(int j = 0; j < adj_lists_[i].size(); j++)
                edges.push_back(std::pair<int, int>(adj_lists_[i][j].src, adj_lists_[i][j].dst));    

        int iter_num = size_ * log(size_);
        int min_cut = edges.size();
        for(int iter = 0; iter < iter_num; iter++)
        {
            std::random_shuffle(edges.begin(), edges.end());
            tw::UnionFind vertex_union(size_);
            int edge_iter = 0;
            int operation_count = 0;
            while(edge_iter < edges.size() && operation_count < size_-2)
            {
                int src = edges[edge_iter].first;
                int dst = edges[edge_iter].second;
                if(vertex_union.UnionSet(src, dst)) //contention operation
                    operation_count++;
                edge_iter++;
            }
            std::vector<int> temp_parts[2];
            for(int i = 0; i < size_; i++)
            {
                if(vertex_union.Find(i) == vertex_union.Find(0))
                    temp_parts[0].push_back(i);
                else
                    temp_parts[1].push_back(i);
            }
            int temp_cut = 0;
            for(int i = 0; i < size_; i++)
            {
                for(int j = 0; j < adj_lists_[i].size(); j++)
                {
                    if(vertex_union.Find(adj_lists_[i][j].src) != vertex_union.Find(adj_lists_[i][j].dst))
                        temp_cut++;
                }
            }
            if(min_cut > temp_cut)
            {
                min_cut = temp_cut;
                global_min_cut1 = temp_parts[0];
                global_min_cut2 = temp_parts[1];
            }
        }
        std::vector<std::vector<int> > global_min_cut;
        global_min_cut.push_back(global_min_cut1);
        global_min_cut.push_back(global_min_cut2);

        return global_min_cut;
    }

    bool ImageGraph::Consolidate(int k)
    {
        return true;
    }

    void ImageGraph::ShowInfo()
    {
        std::cout << "[ImageGraph] Size: " << size_ << "\n";
        for(int i = 0; i < size_; i++)
        {
            for(int j = 0; j < adj_lists_[i].size(); j++)
            {
                std::cout << adj_lists_[i][j].src << " " << 
                adj_lists_[i][j].dst << " " << adj_lists_[i][j].score << "\n";
            }
        }
    }

}   // end of namespace vot
