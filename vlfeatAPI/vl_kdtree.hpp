//
//  vl_kdtree.hpp
//  sift-match-with-ransac
//
//  Created by willard on 3/19/16.
//  Copyright © 2016 wilard. All rights reserved.
//

#ifndef vl_kdtree_hpp
#define vl_kdtree_hpp

#include <stdio.h>

#include "utils.h"

extern "C" {
#include <vl/kdtree.h>
}

VlKDForest * kdtreebuild(float *data, vl_size numData, vl_size numTrees, unsigned int maxNumComparisons, float *query, vl_size numQueries, vl_size numNeighbors, std::vector<std::vector<int>> &ind, std::vector<std::vector<float>> &dis);

void vl_kdforestquery(VlKDForest * forest, int numNeighbors, int numQueries, float * dist, std::vector<std::vector<int>> &ind, std::vector<std::vector<float>> &dis);

#endif /* vl_kdtree_hpp */
