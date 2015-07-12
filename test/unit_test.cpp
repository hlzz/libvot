#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <unistd.h>

#include "../src/vocab_tree/vot_pipeline.h"
#include "gtest/gtest.h"
#include "../src/image_graph/image_graph.h"
	
TEST(ImageGraph, NumConnectedComponents)
{
	vot::ImageGraph g(6);
	g.addEdge(0, 1, 0);
	g.addEdge(1, 2, 0);
	g.addEdge(2, 3, 0);
	g.addEdge(3, 4, 0);
	g.addEdge(4, 5, 0);
	EXPECT_EQ(1, g.NumConnectedComponents());		

	vot::ImageGraph q(6);
	q.addEdge(0, 1, 0);
	q.addEdge(1, 2, 0);
	q.addEdge(2, 3, 0);
	q.addEdge(4, 5, 0);
	EXPECT_EQ(2, q.NumConnectedComponents());		
}

TEST(VocabTree, Pipeline)
{
    const char *sift_input_file = "small_stadium/sift_list";
    const char *tree_output = "tree.out";
    const char *db_output = "db.out";
    const char *match_output = "match.out";
    const char *filtered_output = "match";

    // optional parameters
    int depth = 6;
    int branch_num = 8;
    int sift_type = 0;
    int thread_num = sysconf(_SC_NPROCESSORS_ONLN);     // this works on unix and mac
    int start_id = 0;
    int num_matches = 50;

    vot::BuildVocabTree(sift_input_file, tree_output, depth, branch_num, sift_type, thread_num);
    vot::BuildImageDatabase(sift_input_file, tree_output, db_output, sift_type, start_id, thread_num);
    vot::QueryDatabase(db_output, sift_input_file, match_output, sift_type, thread_num);
    vot::FilterMatchList(sift_input_file, match_output, filtered_output, num_matches);
}