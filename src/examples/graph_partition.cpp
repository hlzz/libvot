//graph_partition

#include <iostream>
#include <vector>
#include <fstream>
#include "image_graph/image_graph.h"
#include "utils/io_utils.h"

using namespace std;

int main(int argc, char **argv)
{
    if(argc < 2)
    {
        std::cout << "Usage: " << argv[0] << " <adj_lists_file>\n";
        exit(1);
    }

    const char* adj_lists_file = argv[1];

    /**
     *  Case #1: make a undirected graph data
     */
    vot::ImageGraph ig(6);
    ig.addEdge(0, 1, 1.0);
    ig.addEdge(0, 2, 1.0);
    ig.addEdge(1, 2, 1.0);
    ig.addEdge(2, 3, 1.0);
    ig.addEdge(3, 4, 1.0);
    ig.addEdge(3, 5, 1.0);
    ig.addEdge(4, 5, 1.0);

    std::vector<std::vector<int> > cuts;
    // Karger's algorithm requires the graph to be a connected component
    if(ig.NumConnectedComponents())
    {
        cuts = ig.KargerCut();
        cout << cuts[0].size() << " " << cuts[1].size() << endl;
        cout << "part1\n";
        for(int i = 0; i < cuts[0].size(); i++)
            cout << cuts[0][i] << endl;
        cout << "part2\n";
        for(int i = 0; i < cuts[1].size(); i++)
            cout << cuts[1][i] << endl;
    }

    /**
     *  Case #2: read graph data from a file
     */
    ifstream fin;
    fin.open(adj_lists_file);
    int image_num, src, dst;
    double score;
    fin >> image_num;
    vot::ImageGraph large_graph(image_num);
    while(fin >> src >> dst >> score)
    {
        large_graph.addEdge(src, dst, score);
    }

    cout << "Number of CC: " << large_graph.NumConnectedComponents(5) << endl;

    return 0;
}
