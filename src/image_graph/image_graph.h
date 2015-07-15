#ifndef IMAGEGRAPH_HEADER
#define IMAGEGRAPH_HEADER

#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>
#include <cassert>
#include <unordered_set>

namespace vot
{
	struct LinkNode
	{
		size_t src;
		size_t dst;
		float score;
		int p_match;
		int g_match;

		LinkNode(int src_ = -1, int dst_ = -1, float score_ = 0.0, int p_match_ = 0, int g_match_ = 0):
		src(src_), dst(dst_), score(score_), p_match(p_match_), g_match(g_match_) {}
	};

	class ImageGraph 
	{
	public:
        //! Brief construct ananymous image graph without filenames 
		ImageGraph(int size);
		//! Brief construct a image graph with filenames
		ImageGraph(const std::vector<std::string> &image_filenames, const std::vector<std::string> &sift_filenames);
		void addEdge(int src, int dst, double score);
		void addEdge(vot::LinkNode n);
		int NumConnectedComponents(int threshold = 0);
        std::vector<std::vector<int> > KargerCut();
        bool Consolidate(int k);
        bool QueryExpansionSub(int src, int tgt, 
        					   double score, 
        					   bool **visit_mat, 
        					   std::vector<std::vector<vot::LinkNode> > &expansion_lists, 
        					   int level,
        					   int inlier_threshold);
        std::vector<std::vector<vot::LinkNode> > QueryExpansion(std::vector<std::vector<vot::LinkNode> > &expansion_lists, 
                                                                bool **visit_mat,
                                                                int level,
                                                                int inlier_threshold = 150);
        void ShowInfo();

	private:
		size_t size_;
		std::vector<std::string> image_filenames_;
		std::vector<std::string> sift_filenames_;
		std::vector<std::vector<LinkNode> > adj_lists_;		//!< adjacent list that save the connection
		std::vector<std::unordered_set<int> > adj_sets_;		//!< this unordered_set is used to insert nodes
	};
}	// end of namespace vot

#endif	// IMAGEGRAPH_HEADER
