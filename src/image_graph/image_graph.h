#ifndef __IMAGEGRAPH_HEADER__
#define __IMAGEGRAPH_HEADER__

#include <cstdlib>
#include <vector>
#include <string>
#include <assert.h>

namespace vot
{
	struct LinkNode
	{
		size_t src;
		size_t dst;
		int p_match;
		int g_match;
		float score;

		LinkNode(int src_ = -1, int dst_ = -1, float score_ = 0.0, int p_match_ = 0, int g_match_ = 0):
		src(src_), dst(dst_), score(score_), p_match(p_match_), g_match(g_match_) {}
	};

	class ImageGraph 
	{
	public:
		ImageGraph()
		{
			size_ = 0;
		}

		ImageGraph(const std::vector<std::string> &image_filenames,
			const std::vector<std::string> &sift_filenames,
			std::vector<LinkNode> *adj_lists = NULL)
		{
			size_ = image_filenames.size();
			image_filenames_ = image_filenames;
			sift_filenames_ = sift_filenames;
			assert(sift_filenames.size() == image_filenames.size());
		}

		~ImageGraph()
		{
			if(adj_lists_ != NULL)
			{
				delete [] adj_lists_;
			}
		}

		int NumConnectedComponents();

	private:
		size_t size_;
		std::vector<std::string> image_filenames_;
		std::vector<std::string> sift_filenames_;
		std::vector<LinkNode> *adj_lists_;
	};
}	// end of namespace vot

#endif	// __IMAGEGRAPH_HEADER__
