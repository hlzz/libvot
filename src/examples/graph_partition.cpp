//graph_partition

#include <iostream>
#include <vector>
#include "image_graph/image_graph.h"
#include "utils/io_utils.h"

int main(int argc, char **argv)
{
    if(argc < 2)
    {
        std::cout << "Usage: " << argv[0] << " <adj_lists_file>\n";
    }

    if(argc >=2)
        const char *adj_lists_file = argv[1];

    /**
     *  make test data
     */
    std::vector<vot::LinkNode> *adj_lists;
    adj_lists = new std::vector<vot::LinkNode>[6];
    adj_lists[0].push_back(vot::LinkNode(0, 1, 1.0));
    adj_lists[0].push_back(vot::LinkNode(0, 2, 1.0));
    adj_lists[1].push_back(vot::LinkNode(1, 0, 1.0));
    adj_lists[1].push_back(vot::LinkNode(1, 2, 1.0));
    adj_lists[2].push_back(vot::LinkNode(2, 0, 1.0));
    adj_lists[2].push_back(vot::LinkNode(2, 1, 1.0));
    adj_lists[2].push_back(vot::LinkNode(2, 3, 1.0));
    adj_lists[3].push_back(vot::LinkNode(3, 2, 1.0));
    adj_lists[3].push_back(vot::LinkNode(3, 4, 1.0));
    adj_lists[3].push_back(vot::LinkNode(3, 5, 1.0));
    adj_lists[4].push_back(vot::LinkNode(4, 3, 1.0));
    adj_lists[4].push_back(vot::LinkNode(4, 5, 1.0));
    adj_lists[5].push_back(vot::LinkNode(5, 3, 1.0));
    adj_lists[5].push_back(vot::LinkNode(5, 4, 1.0));

    vot::ImageGraph ig(6);
    ig.SetAdjLists(adj_lists);
    ig.ShowInfo();
    return 0;
}
