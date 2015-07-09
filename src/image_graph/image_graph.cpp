#include <iostream>
#include <string>
#include <queue>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <algorithm>
#include "image_graph.h"
#include "../utils/data_structures.h"

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

    void ImageGraph::addEdge(vot::LinkNode n)
    {
        // Note: here it doesn't check repitition
        adj_lists_[n.src].push_back(n);
    }

    int ImageGraph::NumConnectedComponents(int threshold)
    {
        bool is_visited[size_];
        //memset(is_visited, false, sizeof(size_)); // this is buggy
        for(size_t i = 0; i < size_; i++)
        {
            is_visited[i] = false;
        }

        size_t numCC = 0;
        for(size_t i = 0; i < size_; i++)
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
                    size_t curr = index_queue.front();    
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

    bool ImageGraph::QueryExpansionSub(int src, int tgt, 
                                       double score, bool **visit_mat, 
                                       std::vector<std::vector<vot::LinkNode> > &expansion_lists, 
                                       int level, 
                                       int inlier_threshold)
    {
        if(level < 1) {return false;}
        int size_k = adj_lists_[tgt].size();
        for(int k = 0; k < size_k; k++)
        {
            vot::LinkNode temp(src, adj_lists_[tgt][k].dst, score * adj_lists_[tgt][k].score);
            if(!visit_mat[src][temp.dst] && adj_lists_[tgt][k].g_match > inlier_threshold)
            {
                expansion_lists[src].push_back(temp);
                visit_mat[src][temp.dst] = true;
                visit_mat[temp.dst][src] = true;
                QueryExpansionSub(src, temp.dst, temp.score, visit_mat, expansion_lists, level-1, inlier_threshold);
            }
        }
        return true; 
    }

    std::vector<std::vector<vot::LinkNode> > ImageGraph::QueryExpansion(std::vector<std::vector<vot::LinkNode> > &expansion_lists, 
                                                                        bool **visit_mat,
                                                                        int level,
                                                                        int inlier_threshold)
    {        
        const int MAX_LEVEL = 5;
        if(level < 1 || level > 5)
        {
            std::cout << "[QueryExpansion] Error: exceed the maximum expansion level (5)\n";
            return expansion_lists;
        }

        expansion_lists.resize(size_);

        for(int i = 0; i < size_; i++)
        {
            int size_i = adj_lists_[i].size();
            for(int j = 0; j < size_i; j++)
            {
                expansion_lists[i].push_back(adj_lists_[i][j]);
                visit_mat[i][adj_lists_[i][j].dst] = true;
                if(adj_lists_[i][j].g_match > inlier_threshold)
                    QueryExpansionSub(i, adj_lists_[i][j].dst, adj_lists_[i][j].score, visit_mat, expansion_lists, level - 1, inlier_threshold);
            }
        }

        return expansion_lists;
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
