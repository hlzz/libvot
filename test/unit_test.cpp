#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <unistd.h>

#include "../src/vocab_tree/vot_pipeline.h"
#include "gtest/gtest.h"
#include "../src/image_graph/image_graph.h"
	
TEST(ImageGraph, ImageNode)
{
	vot::ImageNode a("123.jpg", "123.sift");	// constructor use image_name and sift_name
	vot::ImageNode b(a);		// copy constructor
	EXPECT_EQ("123.jpg", b.image_name);
	EXPECT_EQ("123.sift", b.sift_name);
	vot::ImageNode c;
	vot::ImageGraph image_graph(0);
	image_graph.AddNode(a);
	image_graph.AddNode(b);
	image_graph.AddNode(c);
	image_graph.AddNode();
	EXPECT_EQ(4, image_graph.NodeNum());
	EXPECT_EQ(4, image_graph.NumConnectedComponents());
}

TEST(ImageGraph, NumConnectedComponents)
{
	vot::ImageGraph g(6);
	g.AddEdge(0, 1, 0);
	g.AddEdge(1, 2, 0);
	g.AddEdge(2, 3, 0);
	g.AddEdge(3, 4, 0);
	g.AddEdge(4, 5, 0);
	EXPECT_EQ(1, g.NumConnectedComponents());		

	vot::ImageGraph q(6);
	q.AddEdge(0, 1, 0);
	q.AddEdge(1, 2, 0);
	q.AddEdge(2, 3, 0);
	q.AddEdge(4, 5, 0);
	EXPECT_EQ(2, q.NumConnectedComponents());		

	// {{0,1,2,3,4}, {5}, {6,7}, {8,9}}
	vot::ImageGraph double_link(10);
	double_link.AddEdge(0, 1, 3);
	double_link.AddEdge(1, 0, 3);
	EXPECT_EQ(1, double_link.AdjListSize(1));
	EXPECT_EQ(1, double_link.AdjListSize(0));

	double_link.AddEdge(2, 3, 3);
	double_link.AddEdge(3, 4, 3);
	double_link.AddEdge(8, 9, 3);
	double_link.AddEdge(9, 8, 3);
	double_link.AddEdge(9, 8, 120);		// this add won't do anything to the graph
	double_link.AddEdge(1, 6, 100);
	double_link.AddEdge(6, 7, 3);
	double_link.AddEdge(7, 6, 3);
	EXPECT_EQ(4, double_link.NumConnectedComponents());
	// the number of CC of size greater or equal than 2
	EXPECT_EQ(3, double_link.NumConnectedComponents(2));

	// test size function
	EXPECT_EQ(10, double_link.NodeNum());
	EXPECT_EQ(6, q.NodeNum());
	EXPECT_EQ(6, g.NodeNum());
}

TEST(ImageGraph, AddRepeatedEdge)
{
    vot::ImageGraph g(10);
    g.AddEdge(0, 1, 0);
    g.AddEdge(0, 1, 1);
    g.AddEdge(0, 1, 100);
    // also test the other direcition; this means to protect double-add
    g.AddEdge(1, 0, 0.0);
    g.AddEdge(1, 0, 1);
    g.AddEdge(1, 0, 200);
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

TEST(ImageGraph, KargerCutSimpleTest)
{
	// test Karger's Cut, assume the graph is a connected component, undirected
	// Case 1: {{0, 1, 2}, {3, 4, 5}}
	const int node_num = 6;
	vot::ImageGraph g(node_num);
	g.AddEdge(0, 1, 1);
	g.AddEdge(1, 2, 1);
	g.AddEdge(0, 2, 1);
	g.AddEdge(2, 3, 1);
	g.AddEdge(3, 4, 1);
	g.AddEdge(4, 5, 1);
	g.AddEdge(3, 5, 1);
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
