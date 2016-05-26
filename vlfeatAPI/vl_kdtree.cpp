//
//  vl_kdtree.cpp
//  sift-match-with-ransac
//
//  Created by willard on 3/19/16.
//  Copyright © 2016 wilard. All rights reserved.
//

#include "vl_kdtree.hpp"

VlKDForest * kdtreebuild(float *data, vl_size numData, vl_size numTrees, unsigned int maxNumComparisons, float *query, vl_size numQueries, vl_size numNeighbors, std::vector<std::vector<int>> &ind, std::vector<std::vector<float>> &dis){
    bool verbose = 0;
        
    vl_size dimension = 128 ;
    VlKDForest * forest ;
    
    vl_type dataType = VL_TYPE_FLOAT ;
    VlVectorComparisonType distance = VlDistanceL2;
    
    VlKDTreeThresholdingMethod thresholdingMethod = VL_KDTREE_MEDIAN ;
    forest = vl_kdforest_new(dataType, dimension, numTrees, distance) ;
    //forest->thresholdingMethod = VL_KDTREE_MEDIAN;
    vl_kdforest_set_thresholding_method(forest, thresholdingMethod) ;
    if (verbose) {
        char const * str = 0 ;
        VL_PRINTF("vl_kdforestbuild: data %s [%d x %d]\n",
                  vl_get_type_name(dataType), dimension, numData) ;
        switch (vl_kdforest_get_thresholding_method(forest)) {
            case VL_KDTREE_MEAN : str = "mean" ; break ;
            case VL_KDTREE_MEDIAN : str = "median" ; break ;
            default: abort() ;
        }
        VL_PRINTF("vl_kdforestbuild: threshold selection method: %s\n", str) ;
        VL_PRINTF("vl_kdforestbuild: number of trees: %d\n", vl_kdforest_get_num_trees(forest)) ;
    }
    
    vl_kdforest_build(forest, numData, data) ;
    
    if (verbose) {
        vl_uindex ti ;
        for (ti = 0 ; ti < vl_kdforest_get_num_trees(forest) ; ++ ti) {
            VL_PRINTF("vl_kdforestbuild: tree %d: depth %d, num nodes %d\n", ti, vl_kdforest_get_depth_of_tree(forest, ti),vl_kdforest_get_num_nodes_of_tree(forest, ti)) ;
        }
    }
    //VlKDForestSearcher* searcher = vl_kdforest_new_searcher(forest);
    
    vl_kdforest_set_max_num_comparisons(forest, maxNumComparisons) ;
    
    if (verbose) {
        VL_PRINTF ("vl_kdforestquery: number of queries: %d\n", numQueries) ;
        VL_PRINTF ("vl_kdforestquery: number of neighbors per query: %d\n", numNeighbors) ;
        VL_PRINTF ("vl_kdforestquery: max num of comparisons per query: %d\n",
                   vl_kdforest_get_max_num_comparisons (forest)) ;
    }
    return forest;
};


void vl_kdforestquery(VlKDForest * forest, int numNeighbors, int numQueries, float * query, std::vector<std::vector<int>> &ind, std::vector<std::vector<float>> &dis){
    
    vl_uint32 * index = (vl_uint32 *)malloc(sizeof(vl_uint32)*numNeighbors*numQueries);
    float * dist = (float *)malloc(sizeof(float)*numNeighbors*numQueries);
    
    vl_kdforest_query_with_array(forest, index, numNeighbors, numQueries, dist, query); //并行
    vl_kdforest_delete(forest);
    for(int i = 0; i < (int)numNeighbors*numQueries/2; i++){
        /*std::cout << std::setiosflags(std::ios::fixed);
         std::cout << std::setprecision(6) << "最近邻：" << index[2*i] << " 距离：" << dist[2*i] << " " "次近邻：" << index[2*i+1] << " 距离：" << dist[2*i+1] << std::endl;*/
        std::vector<int> indTmp;
        std::vector<float> disTmp;
        indTmp.push_back(index[2*i]);
        indTmp.push_back(index[2*i+1]);
        ind.push_back(indTmp);
        disTmp.push_back(dist[2*i]);
        disTmp.push_back(dist[2*i+1]);
        dis.push_back(disTmp);
    }
}
