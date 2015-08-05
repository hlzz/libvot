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
    adj_maps_.resize(size_);
    nodes_.resize(size_);
    for(int i = 0; i < size_; i++)
        nodes_[i] = ImageNode();
}

ImageGraph::ImageGraph(const std::vector<std::string> &image_filenames, const std::vector<std::string> &sift_filenames)
{
    assert(sift_filenames.size() == image_filenames.size());
    size_ = image_filenames.size();
    adj_maps_.resize(size_);
    nodes_.resize(size_);
    for(int i = 0; i < size_; i++)
        nodes_[i] = ImageNode(image_filenames[i], sift_filenames[i]);
}

void ImageGraph::AddNode()
{
    nodes_.push_back(vot::ImageNode());
    adj_maps_.push_back(EdgeMap());
    size_ = nodes_.size();
}

void ImageGraph::AddNode(const vot::ImageNode &n)
{
    nodes_.push_back(vot::ImageNode(n));
    adj_maps_.push_back(EdgeMap());
    size_ = nodes_.size();
}

void ImageGraph::AddEdge(int src, int dst, double score)
{
    EdgeMap::iterator it = adj_maps_[src].find(dst);
    if(it == adj_maps_[src].end())
    {
        adj_maps_[src].insert(std::make_pair(dst, vot::LinkEdge(src, dst, score)));
    }
}

void ImageGraph::AddEdge(const vot::LinkEdge &n)
{
    int src = n.src, dst = n.dst;
    EdgeMap::iterator it = adj_maps_[src].find(dst);
    if(it == adj_maps_[src].end())
    {
        adj_maps_[src].insert(std::make_pair(dst, n));
    }
}

int ImageGraph::NumConnectedComponents(int threshold)
{
    bool is_visited[size_];
    for(int i = 0; i < size_; i++)
    {
        is_visited[i] = false;
    }

    size_t numCC = 0;
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
                for(EdgeMap::iterator it = adj_maps_[curr].begin(); it != adj_maps_[curr].end(); it++)
                {
                    if(!is_visited[it->second.dst])
                    {
                        is_visited[it->second.dst] = true;
                        component_size++;
                        index_queue.push(it->second.dst);
                    }
                }
            }
            if(component_size < threshold && threshold != 0)
                numCC--;
        }
    }
    return numCC;
}

bool ImageGraph::KargerCut(std::vector<std::vector<int> > &global_min_cut)
{
    std::vector<int> global_min_cut1, global_min_cut2;
    // randomly select an edge
    std::vector<std::pair<int, int> > edges;
    for(int i = 0; i < size_; i++)
        for(EdgeMap::iterator it = adj_maps_[i].begin(); it != adj_maps_[i].end(); it++)
            edges.push_back(std::pair<int, int>(it->second.src, it->second.dst));

    int iter_num = size_ * log(size_);
    int min_cut = edges.size();
    for(int iter = 0; iter < iter_num; iter++)
    {
        std::random_shuffle(edges.begin(), edges.end());
        tw::UnionFind vertex_union(size_);
        unsigned int edge_iter = 0;
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
            for(EdgeMap::iterator it = adj_maps_[i].begin(); it != adj_maps_[i].end(); it++)
            {
                if(vertex_union.Find(it->second.src) != vertex_union.Find(it->second.dst))
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
    global_min_cut.clear();
    global_min_cut.push_back(global_min_cut1);
    global_min_cut.push_back(global_min_cut2);

    return true;
}

bool ImageGraph::Consolidate(int k)
{
    return true;
}

bool ImageGraph::QueryExpansionSub(int src, int tgt,
                                   double score, bool **visit_mat,
                                   Edge2dArray &expansion_lists,
                                   int level,
                                   int inlier_threshold)
{
    if(level < 1) {return false;}
    for(EdgeMap::iterator it = adj_maps_[tgt].begin(); it != adj_maps_[tgt].end(); it++)
    {
        vot::LinkEdge temp(src, it->second.dst, score * it->second.score);
        if(!visit_mat[src][temp.dst] && it->second.g_match > inlier_threshold)
        {
            if(temp.src != temp.dst)
                expansion_lists[src].push_back(temp);
            visit_mat[src][temp.dst] = true;
            visit_mat[temp.dst][src] = true;
            QueryExpansionSub(src, temp.dst, temp.score, visit_mat, expansion_lists, level-1, inlier_threshold);
        }
    }
    return true;
}

std::vector<std::vector<vot::LinkEdge> > ImageGraph::QueryExpansion(Edge2dArray &expansion_lists,
                                                                    bool **visit_mat,
                                                                    int level,
                                                                    int inlier_threshold)
{
    const int MAX_LEVEL = 5;
    if(level < 1 || level > MAX_LEVEL)
    {
        std::cout << "[QueryExpansion] Error: exceed the maximum expansion level (5)\n";
        return expansion_lists;
    }

    expansion_lists.resize(size_);

    for(int i = 0; i < size_; i++)
    {
        for(EdgeMap::iterator it = adj_maps_[i].begin(); it != adj_maps_[i].end(); it++)
        {
            visit_mat[i][it->second.dst] = true;
            if(it->second.g_match > inlier_threshold)
                QueryExpansionSub(i, it->second.dst, it->second.score, visit_mat, expansion_lists, level - 1, inlier_threshold);
        }
    }

    return expansion_lists;
}

void ImageGraph::ShowInfo()
{
    std::cout << "[ImageGraph] Node size: " << size_ << "\n";
    for(int i = 0; i < size_; i++)
    {
        for(EdgeMap::iterator it = adj_maps_[i].begin(); it != adj_maps_[i].end(); it++)
        {
            std::cout << it->second.src << " " << it->second.dst << " " << it->second.score << "\n";
        }
    }
}

int ImageGraph::AdjListSize(int idx) { return adj_maps_[idx].size(); }
int ImageGraph::NodeNum() {return size_;}

}   // end of namespace vot
