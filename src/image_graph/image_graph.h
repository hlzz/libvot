#ifndef IMAGEGRAPH_HEADER
#define IMAGEGRAPH_HEADER

#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>
#include <cassert>
#include <unordered_map>

namespace vot
{
struct LinkEdge
{
    size_t src;
    size_t dst;
    float score;
    int p_match;
    int g_match;

    LinkEdge(int src_ = -1, int dst_ = -1, float score_ = 0.0, int p_match_ = 0, int g_match_ = 0):
        src(src_), dst(dst_), score(score_), p_match(p_match_), g_match(g_match_) {}

    //! Copy constructor
    LinkEdge(const LinkEdge &e)
    {
        src = e.src;
        dst = e.dst;
        score = e.score;
        p_match = e.p_match;
        g_match = e.g_match;
    }
};

struct ImageNode
{
    int idx;                    //!< the optional original index (maybe in the image_list)
    std::string image_name;     //!< the image name
    std::string sift_name;      //!< the sift name
    ImageNode(int idx_ = -1, const std::string &iname = "", const std::string &sname = ""): idx(idx_), image_name(iname), sift_name(sname) {}

    //! Copy constructor
    ImageNode(const ImageNode & node)
    {
        idx = node.idx;
        image_name = node.image_name;
        sift_name = node.sift_name;
    }
};

class ImageGraph
{
    typedef std::unordered_map<int, LinkEdge> EdgeMap;
    typedef std::vector<std::vector<LinkEdge> > Edge2dArray;
public:
    //! Brief construct ananymous image graph without filenames
    ImageGraph(int size);
    //! Brief construct a image graph with filenames
    ImageGraph(const std::vector<std::string> &image_filenames, const std::vector<std::string> &sift_filenames);
    void addNode();
    void addNode(const vot::ImageNode &n);
    //! Brief add one-way edge
    void addEdge(int src, int dst, double score = 0.0);
    void addEdge(const vot::LinkEdge &n);
    //! Brief add undirected edge
    void addEdgeu(int src, int dst, double score = 0.0);
    void addEdgeu(const vot::LinkEdge &n);
    //! Brief compute the number of connected components in a undirected graph (edge (i,j) and edge (j,i) are both in the graph)
    int numConnectedComponents(int threshold = 0);
    bool kargerCut(std::vector<std::vector<int> > &global_min_cut);
    //! Brief Remove the singleton node from the graph
    bool consolidate(int k);
    //! Brief Query expansion and its sub-routine
    bool queryExpansion(Edge2dArray &expansion_lists, int level, int inlier_threshold = 150);
    bool queryExpansionSub(int src, int tgt, double score, Edge2dArray &expansion_lists, bool **visit_mat, int level, int inlier_threshold);
    void showInfo();
    //! Brief output the information
    int adjListSize(int idx);
    int nodeNum();

private:
    int size_;										//!< the total number of nodes in the graph
    std::vector<ImageNode> nodes_;                  //!< this stores the nodes information
    std::vector<EdgeMap> adj_maps_;					//!< find the edge index by adj_maps_[src][dst]
};
}	// end of namespace vot

#endif	// IMAGEGRAPH_HEADER
