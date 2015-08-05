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
};

struct ImageNode
{
    std::string image_name;
    std::string sift_name;
    ImageNode(const std::string &iname, const std::string &sname): image_name(iname), sift_name(sname) {}
    ImageNode(): image_name(""), sift_name("") {}
    //! Copy constructor
    ImageNode(const ImageNode & node)
    {
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
    void AddNode();
    void AddNode(const vot::ImageNode &n);
    void AddEdge(int src, int dst, double score);
    void AddEdge(const vot::LinkEdge &n);
    int NumConnectedComponents(int threshold = 0);
    bool KargerCut(std::vector<std::vector<int> > &global_min_cut);
    //! Brief Remove the singleton node from the graph
    bool Consolidate(int k);
    //! Brief Query expansion and its sub-routine
    std::vector<std::vector<vot::LinkEdge> > QueryExpansion(Edge2dArray &expansion_lists,
                                                            bool **visit_mat,
                                                            int level,
                                                            int inlier_threshold = 150);
    bool QueryExpansionSub(int src, int tgt,
                           double score,
                           bool **visit_mat,
                           Edge2dArray &expansion_lists,
                           int level,
                           int inlier_threshold);
    void ShowInfo();
    //! Brief output the information
    int AdjListSize(int idx);
    int NodeNum();

private:
    int size_;										//!< the total number of nodes in the graph
    std::vector<ImageNode> nodes_;                  //!< this stores the nodes information
    std::vector<EdgeMap> adj_maps_;					//!< find the edge index by adj_maps_[src][dst]
};
}	// end of namespace vot

#endif	// IMAGEGRAPH_HEADER
