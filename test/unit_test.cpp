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

TEST(ImageGraph, AddRepeatedEdge)
{
    vot::ImageGraph g(10);
    g.addEdge(0, 1, 0);
    g.addEdge(0, 1, 1);
    g.addEdge(0, 1, 100);
    // also test the other direcition; this means to protect double-add
    g.addEdge(1, 0, 0.0);
    g.addEdge(1, 0, 1);
    g.addEdge(1, 0, 200);
    EXPECT_EQ(1, g.AdjListSize(0));
    EXPECT_EQ(1, g.AdjListSize(1));
}
