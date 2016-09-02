//
//  covdetExtractor.cpp
//  sift-match-with-ransac
//
//  Created by willard on 1/20/16.
//  Copyright © 2016 wilard. All rights reserved.
//

#include "covdetExtractor.hpp"

extern "C" {
#include "vl/covdet.h"
#include "vl/sift.h"
#include "vl/generic.h"
#include "vl/host.h"
#include <time.h>
}

void siftDesctor::flip_descriptor(std::vector<float> &dst, float *src){
    dst.resize(128);
    int const BO = 8; /* number of orientation bins */
    int const BP = 4; /* number of spatial bins */
    int i, j, t;
    for(j = 0; j < BP; ++j){
        int jp = BP -1 - j;
        for(i = 0; i < BP; ++i){
            int o = BO*i + BP*BO*j;
            int op = BO*i + BP*BO*jp;
            dst[op] = src[o];
            //for(t = 0; t < BO; ++t){ //
            for(t = 1; t < BO; ++t){ //
                dst[BO - t + op] = src[t+o];
            }
        }
    }
}


void siftDesctor::covdet_keypoints_and_descriptors(std::string &imageName, std::vector<std::vector<float>> &frames, std::vector<std::vector<float>> &desctor){
    
    // Loading image
    int verbose = 1;
    bool display_image = 0;
    std::string image_path = imageName;
    cv::Mat img = cv::imread(imageName.c_str());   // Read the file
    cv::cvtColor(img, img, cv::COLOR_BGR2GRAY);
    
    img.convertTo(img, CV_32FC1);
    
    vl_size numCols, numRows;
    numCols = img.cols; //61
    numRows = img.rows; //74
    
    img = (cv::Mat_<float>)img/255.0;
    
    /*if (! img.data )                              // Check for invalid input
    {
        std::cout <<  "Could not open or find image " << image_path << std::endl ;
        //return false;
    }*/
    
    // This is for debugging, checking the image was correctly loaded.
    if (display_image) {
        std::string window_name = "Image " + image_path;
        namedWindow(window_name, cv::WINDOW_AUTOSIZE );// Create a window for display.
        imshow(window_name, img);               // Show our image inside it.
        cv::waitKey(0);                                 // Wait for a keystroke in the window
    }
    
    cv::Mat imageTest(img.rows, img.cols, CV_32FC1);
    cv::Mat imgT(img.cols, img.rows, CV_32FC1);
    img.copyTo(imageTest);
    imgT = img.t();
    imgT = imgT.reshape(1,1);
    //std::cout << imgT << std::endl;
    
    float *image = new float[(int)imgT.cols];
    for(int i = 0; i < imgT.cols; i++){
        image[i] = (float)imgT.at<float>(i);
        //std::cout << image[i] << " ";
    }
    
    typedef enum _VlCovDetDescriptorType{
        VL_COVDET_DESC_SIFT
    } VlCovDetDescriporType;
    
    
    VlCovDetMethod method = VL_COVDET_METHOD_DOG;
    //vl_bool estimateAffineShape = VL_FALSE;
    //vl_bool estimateOrientation = VL_FALSE;
    
    vl_bool doubleImage = VL_TRUE;
    vl_index octaveResolution = -1;
    double edgeThreshold = 10;
    double peakThreshold = 0.01;
    double lapPeakThreshold = -1;
    
    int descriptorType = -1;
    vl_index patchResolution = -1;
    double patchRelativeExtent = -1;
    double patchRelativeSmoothing = -1;
    
    double boundaryMargin = 2.0;
    
    if (descriptorType < 0) descriptorType = VL_COVDET_DESC_SIFT;
    
    switch (descriptorType){
        case VL_COVDET_DESC_SIFT:
            if (patchResolution < 0) patchResolution = 15;
            if (patchRelativeExtent < 0) patchRelativeExtent = 10;
            if (patchRelativeSmoothing <0) patchRelativeSmoothing = 1;
            printf("vl_covdet: VL_COVDET_DESC_SIFT: patchResolution=%lld, patchRelativeExtent=%g, patchRelativeSmoothing=%g\n", patchResolution, patchRelativeExtent, patchRelativeSmoothing);
    }
    
    if (1) {
        //clock_t t_start = clock();
        // create a detector object: VL_COVDET_METHOD_HESSIAN
        VlCovDet * covdet = vl_covdet_new(method);
        
        // set various parameters (optional)
        vl_covdet_set_transposed(covdet, VL_TRUE);
        vl_covdet_set_first_octave(covdet, doubleImage? -1 : 0);
        if (octaveResolution >= 0) vl_covdet_set_octave_resolution(covdet, octaveResolution);
        if (peakThreshold >= 0) vl_covdet_set_peak_threshold(covdet, peakThreshold);
        if (edgeThreshold >= 0) vl_covdet_set_edge_threshold(covdet, edgeThreshold);
        if (lapPeakThreshold >= 0) vl_covdet_set_laplacian_peak_threshold(covdet, lapPeakThreshold);
        
        //vl_covdet_set_target_num_features(covdet, target_num_features);
        //vl_covdet_set_use_adaptive_suppression(covdet, use_adaptive_suppression);
        
        if(verbose){
            VL_PRINTF("vl_covdet: doubling image: %s, image size: numRows = %d, numCols = %d\n", VL_YESNO(vl_covdet_get_first_octave(covdet) < 0), numCols, numRows);
        }
        
        // process the image and run the detector, im.shape(1) is column, im.shape(0) is row
        //see http://www.vlfeat.org/api/covdet_8h.html#affcedda1fdc7ed72d223e0aab003024e for detail
        vl_covdet_put_image(covdet, image, numRows, numCols);
        
        if (verbose) {
            VL_PRINTF("vl_covdet: detector: %s\n", vl_enumeration_get_by_value(vlCovdetMethods, method)->name);
            VL_PRINTF("vl_covdet: peak threshold: %g, edge threshold: %g\n", vl_covdet_get_peak_threshold(covdet),vl_covdet_get_edge_threshold(covdet));
        }

        vl_covdet_detect(covdet);
        
        if (verbose) {
            vl_size numFeatures = vl_covdet_get_num_features(covdet) ;
            VL_PRINTF("vl_covdet: %d features suppressed as duplicate (threshold: %g)\n", vl_covdet_get_num_non_extrema_suppressed(covdet), vl_covdet_get_non_extrema_suppression_threshold(covdet));
            VL_PRINTF("vl_covdet: detected %d features", numFeatures);
        }
        
        
        //drop feature on the margin(optimal)
        if(boundaryMargin > 0){
            vl_covdet_drop_features_outside(covdet, boundaryMargin);
            if(verbose){
                vl_size numFeatures = vl_covdet_get_num_features(covdet);
                VL_PRINTF("vl_covdet: kept %d inside the boundary margin (%g)\n", numFeatures, boundaryMargin);
            }
        }
        
        /* affine adaptation if needed */
        bool estimateAffineShape = true;
        if (estimateAffineShape) {
            if (verbose) {
                vl_size numFeaturesBefore = vl_covdet_get_num_features(covdet) ;
                VL_PRINTF("vl_covdet: estimating affine shape for %d features", numFeaturesBefore);
            }
            
            vl_covdet_extract_affine_shape(covdet) ;
            
            if (verbose) {
                vl_size numFeaturesAfter = vl_covdet_get_num_features(covdet) ;
                VL_PRINTF("vl_covdet: %d features passed affine adaptation\n", numFeaturesAfter);
            }
        }
        
        // compute the orientation of the features (optional)
        //vl_covdet_extract_orientations(covdet);
        
        // get feature descriptors
        vl_size numFeatures = vl_covdet_get_num_features(covdet);
        VlCovDetFeature const * feature = (VlCovDetFeature const *)vl_covdet_get_features(covdet);
        VlSiftFilt * sift = vl_sift_new(16, 16, 1, 3, 0);
        vl_size patchSide = 2 * patchResolution + 1;
        double patchStep = (double)patchRelativeExtent / patchResolution;
        float tempDesc[128];
        if (verbose) {
            VL_PRINTF("vl_covdet: descriptors: type = sift, resolution = %d, extent = %g, smoothing = %g\n\n", patchResolution,patchRelativeExtent, patchRelativeSmoothing);
        }
        
        //std::vector<float> points(6 * numFeatures);
        //std::vector<float> desc(dimension * numFeatures);
        
        std::vector<float> desc(128);
        //float * desc;
        std::vector<float> frame(6);
        
        //std::vector<float> patch(patchSide * patchSide);
        //std::vector<float> patchXY(2 * patchSide * patchSide);
        float *patch = (float*)malloc(sizeof(float)*patchSide*patchSide);
        float *patchXY = (float*)malloc(2*sizeof(float)*patchSide*patchSide);
        
        vl_sift_set_magnif(sift, 3.0);
        for (vl_index i = 0; i < (signed)numFeatures; i++) {
            frame.clear();
            frame.push_back(feature[i].frame.y);
            frame.push_back(feature[i].frame.x);
            frame.push_back(feature[i].frame.a22);
            frame.push_back(feature[i].frame.a12);
            frame.push_back(feature[i].frame.a21);
            frame.push_back(feature[i].frame.a11);
            frames.push_back(frame);
            //std::cout << std::setiosflags(std::ios::fixed);
            //std::cout << std::setprecision(6) << feature[i].frame.y << ", " << feature[i].frame.x << ", " << feature[i].frame.a22 << ", "<< feature[i].frame.a12 << ", "<< feature[i].frame.a21 << ", " << feature[i].frame.a11 << ", "<< std::endl;
            
            vl_covdet_extract_patch_for_frame(covdet,
                                              patch,
                                              patchResolution,
                                              patchRelativeExtent,
                                              patchRelativeSmoothing,
                                              feature[i].frame);
            
            vl_imgradient_polar_f(patchXY, patchXY+1,
                                  2, 2 * patchSide,
                                  patch, patchSide, patchSide, patchSide);
            
            vl_sift_calc_raw_descriptor(sift,
                                        patchXY,
                                        tempDesc,
                                        (int)patchSide, (int)patchSide,
                                        (double)(patchSide - 1) / 2, (double)(patchSide - 1) / 2,
                                        (double)patchRelativeExtent / (3.0 * (4 + 1) / 2) / patchStep,
                                        VL_PI / 2);
            flip_descriptor(desc, tempDesc);
            desctor.push_back(desc);
            /*std::cout << std::setiosflags(std::ios::fixed);
            for(vl_index j = 0; j < 128; j++){
                std::cout << std::setprecision(6) << desc[j] << " ";
            }
            std::cout << "\n" << std::endl;*/
        }
        vl_sift_delete(sift);
        //delete[] sift;
        vl_covdet_delete(covdet);
        delete[] patch;
        delete[] patchXY;
        delete[] image;
    }
}