//
//  covdetExtractor.hpp
//  sift-match-with-ransac
//
//  Created by willard on 1/20/16.
//  Copyright © 2016 wilard. All rights reserved.
//

#ifndef covdetExtractor_hpp
#define covdetExtractor_hpp

#include "utils.h"

class siftDesctor{
public:
    siftDesctor(){};
    std::string imageName;
    std::vector<std::vector<float>> frame;
    std::vector<std::vector<float>> desctor;
    void covdet_keypoints_and_descriptors(std::string &imageName, std::vector<std::vector<float>> &frames, std::vector<std::vector<float>> &desctor);
    void flip_descriptor(std::vector<float> &dst, float *src);
};

#endif /* covdetExtractor_hpp */
