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

bool IsXInVector(int x, std::vector<int> &vec)
{
	for(int i = 0; i < vec.size(); i++)
	{
		if(x == vec[i])
			return true;
	}
	return false;
}

TEST(ImageGraph, KargerCut)
{
	// test Karger's Cut, assume the graph is a connected component, undirected
	// Case 1: {{0, 1, 2}, {3, 4, 5}}
	const int node_num = 6;
	vot::ImageGraph g(node_num);
	g.addEdge(0, 1, 1);
	g.addEdge(1, 2, 1);
	g.addEdge(0, 2, 1);
	g.addEdge(2, 3, 1);
	g.addEdge(3, 4, 1);
	g.addEdge(4, 5, 1);
	g.addEdge(3, 5, 1);
	std::vector<std::vector<int> > two_parts;
	g.KargerCut(two_parts);
	EXPECT_EQ(2, two_parts.size());
	EXPECT_EQ(3, two_parts[0].size());
	EXPECT_EQ(3, two_parts[1].size());
	bool flag = true;
	for(int i = 0; i < node_num; i++)
	{
		if(!IsXInVector(i, two_parts[0]) && !IsXInVector(i, two_parts[1]))
			flag = false;
	}
	if(!IsXInVector(0, two_parts[0]))	flag = false;
	if(!IsXInVector(1, two_parts[0]))	flag = false;
	if(!IsXInVector(2, two_parts[0]))	flag = false;
	if(!IsXInVector(3, two_parts[1]))	flag = false;
	if(!IsXInVector(4, two_parts[1]))	flag = false;
	if(!IsXInVector(5, two_parts[1]))	flag = false;
	EXPECT_EQ(true, flag);
}