/**
* Copyright 2016 NVIDIA Corporation.  All rights reserved.
*
* Please refer to the NVIDIA end user license agreement (EULA) associated
* with this source code for terms and conditions that govern your use of
* this software. Any use, reproduction, disclosure, or distribution of
* this software and related documentation outside the terms of the EULA
* is strictly prohibited.
*
*/

#include <cudnn.h>
#include <cuda.h>
#include <stdio.h>


// Reference outputs (calculated on an M40 GPU)
// > ./RNN 20 2 512 64 0
// Forward: 1299 GFLOPs
// Backward: 2171 GFLOPs, (1564 GFLOPs), (3549 GFLOPs)
// i checksum 1.315793E+06     h checksum 1.315212E+05
// di checksum 6.676003E+01    dh checksum 6.425067E+01
// dw checksum 1.453750E+09
//
// > ./RNN 20 2 512 64 1
// Forward: 1296 GFLOPs
// Backward: 2235 GFLOPs, (1567 GFLOPs), (3896 GFLOPs)
// i checksum 6.319591E+05     h checksum 6.319605E+04
// di checksum 4.501830E+00    dh checksum 4.489546E+00
// dw checksum 5.012598E+07
//
// > ./RNN 20 2 512 64 2
// Forward: 2635 GFLOPs
// Backward: 2757 GFLOPs, (2001 GFLOPs), (4433 GFLOPs)
// i checksum 5.749536E+05     c checksum 4.365091E+05     h checksum 5.774818E+04
// di checksum 3.842206E+02    dc checksum 9.323785E+03    dh checksum 1.182566E+01
// dw checksum 4.313461E+08
//
// > ./RNN 20 2 512 64 3
// Forward: 2428 GFLOPs
// Backward: 2645 GFLOPs, (1915 GFLOPs), (4270 GFLOPs)
// i checksum 6.358978E+05     h checksum 6.281680E+04
// di checksum 6.296622E+00    dh checksum 2.289960E+05
// dw checksum 5.397419E+07



// Define some error checking macros.
#define cudaErrCheck(stat) { cudaErrCheck_((stat), __FILE__, __LINE__); }
void cudaErrCheck_(cudaError_t stat, const char *file, int line) {
   if (stat != cudaSuccess) {
      fprintf(stderr, "CUDA Error: %s %s %d\n", cudaGetErrorString(stat), file, line);
   }
}

#define cudnnErrCheck(stat) { cudnnErrCheck_((stat), __FILE__, __LINE__); }
void cudnnErrCheck_(cudnnStatus_t stat, const char *file, int line) {
   if (stat != CUDNN_STATUS_SUCCESS) {
      fprintf(stderr, "cuDNN Error: %s %s %d\n", cudnnGetErrorString(stat), file, line);
   }
}

__global__ void initGPUData_ker(float *data, int numElements, float value) {
   int tid = blockIdx.x * blockDim.x + threadIdx.x;
   if (tid < numElements) {
      data[tid] = value;
   }
}

void initGPUData(float *data, int numElements, float value) {
   dim3 gridDim;
   dim3 blockDim;
   
   blockDim.x = 1024;
   gridDim.x = (numElements + blockDim.x - 1) / blockDim.x;
   
   initGPUData_ker <<< gridDim, blockDim >>> (data, numElements, value);
}

  
int main(int argc, char* argv[]) {

   int seqLength;
   int numLayers;
   int hiddenSize;
   int inputSize;
   int miniBatch;
   float dropout;
   bool bidirectional;
   int mode;

   FILE *fp;
   fp=fopen("result.txt","w");

   if (argc == 6) {
      seqLength = atoi(argv[1]);
      numLayers = atoi(argv[2]);
      hiddenSize = atoi(argv[3]);
      inputSize = hiddenSize;
      miniBatch = atoi(argv[4]);
      dropout = 0;
      bidirectional = 0;
      mode = atoi(argv[5]);
   }
   else {
      printf("Usage:\n");
      printf("./RNN <seqLength> <numLayers> <hiddenSize> <miniBatch> <mode>\n");
      printf("Modes: 0 = RNN_RELU, 1 = RNN_TANH, 2 = LSTM, 3 = GRU\n");
      return 1;
   }

   // -------------------------   
   // Create cudnn context
   // -------------------------  
   cudnnHandle_t cudnnHandle;   
   cudnnErrCheck(cudnnCreate(&cudnnHandle));

   
   // -------------------------   
   // Set up inputs and outputs
   // -------------------------
   void *x;
   void *hx = NULL;
   void *cx = NULL;
   
   void *dx;
   void *dhx = NULL;
   void *dcx = NULL;
  
   void *y;
   void *hy = NULL;
   void *cy = NULL;
   
   void *dy;
   void *dhy = NULL;
   void *dcy = NULL;
   
   // Memory allocation. hx, cx, dhx, dcx, hy, cy, dhy and dcy can be NULL.
   cudaErrCheck(cudaMalloc((void**)&x, seqLength * inputSize * miniBatch * sizeof(float)));
   cudaErrCheck(cudaMalloc((void**)&hx, numLayers * hiddenSize * miniBatch * (bidirectional ? 2 : 1) * sizeof(float)));
   cudaErrCheck(cudaMalloc((void**)&cx, numLayers * hiddenSize * miniBatch * (bidirectional ? 2 : 1) * sizeof(float)));
   
   cudaErrCheck(cudaMalloc((void**)&dx, seqLength * inputSize * miniBatch * sizeof(float)));
   cudaErrCheck(cudaMalloc((void**)&dhx, numLayers * hiddenSize * miniBatch * (bidirectional ? 2 : 1) * sizeof(float)));
   cudaErrCheck(cudaMalloc((void**)&dcx, numLayers * hiddenSize * miniBatch * (bidirectional ? 2 : 1) * sizeof(float)));
   
   cudaErrCheck(cudaMalloc((void**)&y, seqLength * hiddenSize * miniBatch * (bidirectional ? 2 : 1) * sizeof(float)));
   cudaErrCheck(cudaMalloc((void**)&hy, numLayers * hiddenSize * miniBatch * (bidirectional ? 2 : 1) * sizeof(float)));
   cudaErrCheck(cudaMalloc((void**)&cy, numLayers * hiddenSize * miniBatch * (bidirectional ? 2 : 1) * sizeof(float)));
   
   cudaErrCheck(cudaMalloc((void**)&dy, seqLength * hiddenSize * miniBatch * (bidirectional ? 2 : 1) * sizeof(float)));
   cudaErrCheck(cudaMalloc((void**)&dhy, numLayers * hiddenSize * miniBatch * (bidirectional ? 2 : 1) * sizeof(float)));
   cudaErrCheck(cudaMalloc((void**)&dcy, numLayers * hiddenSize * miniBatch * (bidirectional ? 2 : 1) * sizeof(float)));
      
   // Set up tensor descriptors. x/y/dx/dy are arrays, one per time step.
   cudnnTensorDescriptor_t *xDesc, *yDesc, *dxDesc, *dyDesc;
   cudnnTensorDescriptor_t hxDesc, cxDesc;
   cudnnTensorDescriptor_t hyDesc, cyDesc;
   cudnnTensorDescriptor_t dhxDesc, dcxDesc;
   cudnnTensorDescriptor_t dhyDesc, dcyDesc;
   
   xDesc = (cudnnTensorDescriptor_t*)malloc(seqLength * sizeof(cudnnTensorDescriptor_t));
   yDesc = (cudnnTensorDescriptor_t*)malloc(seqLength * sizeof(cudnnTensorDescriptor_t));
   dxDesc = (cudnnTensorDescriptor_t*)malloc(seqLength * sizeof(cudnnTensorDescriptor_t));
   dyDesc = (cudnnTensorDescriptor_t*)malloc(seqLength * sizeof(cudnnTensorDescriptor_t));
   
   int dimA[3];
   int strideA[3];

   // In this example dimA[1] is constant across the whole sequence
   // This isn't required, all that is required is that it does not increase.
   for (int i = 0; i < seqLength; i++) {
      cudnnErrCheck(cudnnCreateTensorDescriptor(&xDesc[i]));
      cudnnErrCheck(cudnnCreateTensorDescriptor(&yDesc[i]));
      cudnnErrCheck(cudnnCreateTensorDescriptor(&dxDesc[i]));
      cudnnErrCheck(cudnnCreateTensorDescriptor(&dyDesc[i]));
   
      dimA[0] = miniBatch;
      dimA[1] = inputSize;
      dimA[2] = 1;
     
      strideA[0] = dimA[2] * dimA[1];
      strideA[1] = dimA[2];
      strideA[2] = 1;

      cudnnErrCheck(cudnnSetTensorNdDescriptor(xDesc[i], CUDNN_DATA_FLOAT, 3, dimA, strideA));
      cudnnErrCheck(cudnnSetTensorNdDescriptor(dxDesc[i], CUDNN_DATA_FLOAT, 3, dimA, strideA));
      
      dimA[0] = miniBatch;
      dimA[1] = bidirectional ? hiddenSize * 2 : hiddenSize;
      dimA[2] = 1;

      strideA[0] = dimA[2] * dimA[1];
      strideA[1] = dimA[2];
      strideA[2] = 1;
      
      cudnnErrCheck(cudnnSetTensorNdDescriptor(yDesc[i], CUDNN_DATA_FLOAT, 3, dimA, strideA));
      cudnnErrCheck(cudnnSetTensorNdDescriptor(dyDesc[i], CUDNN_DATA_FLOAT, 3, dimA, strideA));
   }
   
   
   dimA[0] = numLayers * (bidirectional ? 2 : 1);
   dimA[1] = miniBatch;
   dimA[2] = hiddenSize;
   
   strideA[0] = dimA[2] * dimA[1];
   strideA[1] = dimA[2];
   strideA[2] = 1;
   
   cudnnErrCheck(cudnnCreateTensorDescriptor(&hxDesc));
   cudnnErrCheck(cudnnCreateTensorDescriptor(&cxDesc));
   cudnnErrCheck(cudnnCreateTensorDescriptor(&hyDesc));
   cudnnErrCheck(cudnnCreateTensorDescriptor(&cyDesc));
   cudnnErrCheck(cudnnCreateTensorDescriptor(&dhxDesc));
   cudnnErrCheck(cudnnCreateTensorDescriptor(&dcxDesc));
   cudnnErrCheck(cudnnCreateTensorDescriptor(&dhyDesc));
   cudnnErrCheck(cudnnCreateTensorDescriptor(&dcyDesc));
   
   cudnnErrCheck(cudnnSetTensorNdDescriptor(hxDesc, CUDNN_DATA_FLOAT, 3, dimA, strideA));
   cudnnErrCheck(cudnnSetTensorNdDescriptor(cxDesc, CUDNN_DATA_FLOAT, 3, dimA, strideA));
   cudnnErrCheck(cudnnSetTensorNdDescriptor(hyDesc, CUDNN_DATA_FLOAT, 3, dimA, strideA));
   cudnnErrCheck(cudnnSetTensorNdDescriptor(cyDesc, CUDNN_DATA_FLOAT, 3, dimA, strideA));
   cudnnErrCheck(cudnnSetTensorNdDescriptor(dhxDesc, CUDNN_DATA_FLOAT, 3, dimA, strideA));
   cudnnErrCheck(cudnnSetTensorNdDescriptor(dcxDesc, CUDNN_DATA_FLOAT, 3, dimA, strideA));
   cudnnErrCheck(cudnnSetTensorNdDescriptor(dhyDesc, CUDNN_DATA_FLOAT, 3, dimA, strideA));
   cudnnErrCheck(cudnnSetTensorNdDescriptor(dcyDesc, CUDNN_DATA_FLOAT, 3, dimA, strideA));
  
  
   // -------------------------
   // Set up the dropout descriptor (needed for the RNN descriptor)
   // -------------------------
   unsigned long long seed = 1337ull; // Pick a seed.
   
   cudnnDropoutDescriptor_t dropoutDesc;
   cudnnErrCheck(cudnnCreateDropoutDescriptor(&dropoutDesc));
   
   // How much memory does dropout need for states?
   // These states are used to generate random numbers internally
   // and should not be freed until the RNN descriptor is no longer used
   size_t stateSize;
   void *states;
   cudnnErrCheck(cudnnDropoutGetStatesSize(cudnnHandle, &stateSize));
   
   cudaErrCheck(cudaMalloc(&states, stateSize));
   
   cudnnErrCheck(cudnnSetDropoutDescriptor(dropoutDesc, 
                             cudnnHandle,
                             dropout, 
                             states, 
                             stateSize, 
                             seed));
                             
   // -------------------------   
   // Set up the RNN descriptor
   // -------------------------
   cudnnRNNDescriptor_t rnnDesc;
   cudnnRNNMode_t RNNMode;
   
   cudnnErrCheck(cudnnCreateRNNDescriptor(&rnnDesc));
   
   if      (mode == 0) RNNMode = CUDNN_RNN_RELU;
   else if (mode == 1) RNNMode = CUDNN_RNN_TANH;
   else if (mode == 2) RNNMode = CUDNN_LSTM;
   else if (mode == 3) RNNMode = CUDNN_GRU;
      
   cudnnErrCheck(cudnnSetRNNDescriptor(rnnDesc,
                                       hiddenSize, 
                                       numLayers, 
                                       dropoutDesc,
                                       CUDNN_LINEAR_INPUT, // We can also skip the input matrix transformation
                                       bidirectional ? CUDNN_BIDIRECTIONAL : CUDNN_UNIDIRECTIONAL, 
                                       RNNMode, 
                                       CUDNN_DATA_FLOAT));
   
   
   // -------------------------
   // Set up parameters
   // -------------------------
   // This needs to be done after the rnn descriptor is set as otherwise
   // we don't know how many parameters we have to allocate
   void *w;   
   void *dw;   

   cudnnFilterDescriptor_t wDesc, dwDesc;
   
   cudnnErrCheck(cudnnCreateFilterDescriptor(&wDesc));
   cudnnErrCheck(cudnnCreateFilterDescriptor(&dwDesc));
   
   size_t weightsSize;
   cudnnErrCheck(cudnnGetRNNParamsSize(cudnnHandle, rnnDesc, xDesc[0], &weightsSize, CUDNN_DATA_FLOAT));
   
   int dimW[3];   
   dimW[0] =  weightsSize / sizeof(float);
   dimW[1] = 1;
   dimW[2] = 1;
      
   cudnnErrCheck(cudnnSetFilterNdDescriptor(wDesc, CUDNN_DATA_FLOAT, CUDNN_TENSOR_NCHW, 3, dimW));   
   cudnnErrCheck(cudnnSetFilterNdDescriptor(dwDesc, CUDNN_DATA_FLOAT, CUDNN_TENSOR_NCHW, 3, dimW));   
   
   cudaErrCheck(cudaMalloc((void**)&w,  weightsSize));
   cudaErrCheck(cudaMalloc((void**)&dw, weightsSize));
   
   
   // -------------------------
   // Set up work space and reserved memory
   // -------------------------   
   void *workspace;
   void *reserveSpace;   
   
   size_t workSize;
   size_t reserveSize;

   // Need for every pass
   cudnnErrCheck(cudnnGetRNNWorkspaceSize(cudnnHandle, rnnDesc, seqLength, xDesc, &workSize));
   // Only needed in training, shouldn't be touched between passes.
   cudnnErrCheck(cudnnGetRNNTrainingReserveSize(cudnnHandle, rnnDesc, seqLength, xDesc, &reserveSize));
    
   cudaErrCheck(cudaMalloc((void**)&workspace, workSize));
   cudaErrCheck(cudaMalloc((void**)&reserveSpace, reserveSize));
   
   // *********************************************************************************************************
   // Initialise weights and inputs
   // *********************************************************************************************************
   // We initialise to something simple.
   // Matrices are initialised to 1 / matrixSize, biases to 1, data is 1.
   initGPUData((float*)x, seqLength * inputSize * miniBatch, 1.f);
   if (hx != NULL) initGPUData((float*)hx, numLayers * hiddenSize * miniBatch * (bidirectional ? 2 : 1), 1.f);
   if (cx != NULL) initGPUData((float*)cx, numLayers * hiddenSize * miniBatch * (bidirectional ? 2 : 1), 1.f);
   
   initGPUData((float*)dy, seqLength * hiddenSize * miniBatch * (bidirectional ? 2 : 1), 1.f);
   if (dhy != NULL) initGPUData((float*)dhy, numLayers * hiddenSize * miniBatch * (bidirectional ? 2 : 1), 1.f);
   if (dcy != NULL) initGPUData((float*)dcy, numLayers * hiddenSize * miniBatch * (bidirectional ? 2 : 1), 1.f);
      
   
   // Weights
   int numLinearLayers = 0;
   if (RNNMode == CUDNN_RNN_RELU || RNNMode == CUDNN_RNN_TANH) {
      numLinearLayers = 2;
   }
   else if (RNNMode == CUDNN_LSTM) {
      numLinearLayers = 8;
   }
   else if (RNNMode == CUDNN_GRU) {
      numLinearLayers = 6;
   }
   
   for (int layer = 0; layer < numLayers * (bidirectional ? 2 : 1); layer++) {
      for (int linLayerID = 0; linLayerID < numLinearLayers; linLayerID++) {
         cudnnFilterDescriptor_t linLayerMatDesc;
         cudnnErrCheck(cudnnCreateFilterDescriptor(&linLayerMatDesc));
         float *linLayerMat;
         
         cudnnErrCheck(cudnnGetRNNLinLayerMatrixParams( cudnnHandle,
                                                        rnnDesc,  
                                                        layer,
                                                        xDesc[0], 
                                                        wDesc, 
                                                        w,
                                                        linLayerID,  
                                                        linLayerMatDesc, 
                                                        (void**)&linLayerMat));
         
         cudnnDataType_t dataType;
         cudnnTensorFormat_t format;
         int nbDims;
         int filterDimA[3];
         cudnnErrCheck(cudnnGetFilterNdDescriptor(linLayerMatDesc,
                                                  3,
                                                  &dataType,
                                                  &format,
                                                  &nbDims,
                                                  filterDimA));
                                                  
         initGPUData(linLayerMat, filterDimA[0] * filterDimA[1] * filterDimA[2], 1.f / (float)(filterDimA[0] * filterDimA[1] * filterDimA[2]));                                                 

         cudnnErrCheck(cudnnDestroyFilterDescriptor(linLayerMatDesc));         
         
         cudnnFilterDescriptor_t linLayerBiasDesc;
         cudnnErrCheck(cudnnCreateFilterDescriptor(&linLayerBiasDesc));
         float *linLayerBias;
         
         cudnnErrCheck(cudnnGetRNNLinLayerBiasParams( cudnnHandle,
                                                        rnnDesc,  
                                                        layer,
                                                        xDesc[0], 
                                                        wDesc, 
                                                        w,
                                                        linLayerID,  
                                                        linLayerBiasDesc, 
                                                        (void**)&linLayerBias));
         
         cudnnErrCheck(cudnnGetFilterNdDescriptor(linLayerBiasDesc,
                                                  3,
                                                  &dataType,
                                                  &format,
                                                  &nbDims,
                                                  filterDimA));
                                                  
         initGPUData(linLayerBias, filterDimA[0] * filterDimA[1] * filterDimA[2], 1.f);
                                                  
         cudnnErrCheck(cudnnDestroyFilterDescriptor(linLayerBiasDesc));
      }
   }
   
   // *********************************************************************************************************
   // At this point all of the setup is done. We now need to pass through the RNN.
   // *********************************************************************************************************
   
  
   
   cudaErrCheck(cudaDeviceSynchronize());
   
   cudaEvent_t start, stop;
   float timeForward, timeBackward1, timeBackward2;
   cudaErrCheck(cudaEventCreate(&start));
   cudaErrCheck(cudaEventCreate(&stop));
   
   cudaErrCheck(cudaEventRecord(start));   

   // If we're not training we use this instead
   // cudnnErrCheck(cudnnRNNForwardInference(cudnnHandle, 
                                         // rnnDesc, 
                                         // xDesc, 
                                         // x, 
                                         // hxDesc,
                                         // hx, 
                                         // cxDesc, 
                                         // cx, 
                                         // wDesc, 
                                         // w, 
                                         // yDesc,  
                                         // y, 
                                         // hyDesc, 
                                         // hy, 
                                         // cyDesc, 
                                         // cy, 
                                         // workspace, 
                                         // workSize));

   cudnnErrCheck(cudnnRNNForwardTraining(cudnnHandle, 
                                         rnnDesc, 
                                         seqLength,                                          
                                         xDesc, 
                                         x, 
                                         hxDesc,
                                         hx, 
                                         cxDesc, 
                                         cx, 
                                         wDesc, 
                                         w, 
                                         yDesc,  
                                         y, 
                                         hyDesc, 
                                         hy, 
                                         cyDesc, 
                                         cy, 
                                         workspace, 
                                         workSize,
                                         reserveSpace, 
                                         reserveSize));
                
   cudaErrCheck(cudaEventRecord(stop));   
   cudaErrCheck(cudaEventSynchronize(stop));
   cudaErrCheck(cudaEventElapsedTime(&timeForward, start, stop));
   
   cudaErrCheck(cudaEventRecord(start));
   
   cudnnErrCheck(cudnnRNNBackwardData(cudnnHandle, 
                               rnnDesc, 
                               seqLength,                                
                               yDesc, 
                               y,
                               dyDesc, 
                               dy, 
                               dhyDesc, 
                               dhy, 
                               dcyDesc, 
                               dcy, 
                               wDesc, 
                               w, 
                               hxDesc, 
                               hx,
                               cxDesc, 
                               cx,
                               dxDesc, 
                               dx, 
                               dhxDesc,
                               dhx,
                               dcxDesc,
                               dcx,
                               workspace,
                               workSize,
                               reserveSpace, 
                               reserveSize ));
   
   cudaErrCheck(cudaEventRecord(stop));   
   cudaErrCheck(cudaEventSynchronize(stop));
   cudaErrCheck(cudaEventElapsedTime(&timeBackward1, start, stop));
   
   cudaErrCheck(cudaEventRecord(start));
   
   // cudnnRNNBackwardWeights adds to the data in dw.
   cudaErrCheck(cudaMemset(dw, 0, weightsSize));
   
   cudnnErrCheck(cudnnRNNBackwardWeights( cudnnHandle, 
                                    rnnDesc, 
                                    seqLength, 
                                    xDesc, 
                                    x, 
                                    hxDesc, 
                                    hx,                                                   
                                    yDesc, 
                                    y,
                                    workspace, 
                                    workSize, 
                                    dwDesc, 
                                    dw,
                                    reserveSpace, 
                                    reserveSize ));
                     
                     

   cudaErrCheck(cudaEventRecord(stop));   

   cudaErrCheck(cudaEventSynchronize(stop));
   cudaErrCheck(cudaEventElapsedTime(&timeBackward2, start, stop));

   
   int numMats = 0;
   
   if (RNNMode == CUDNN_RNN_RELU || RNNMode == CUDNN_RNN_TANH) {
      numMats = 2;
   }
   else if (RNNMode == CUDNN_LSTM) {
      numMats = 8;
   }
   else if (RNNMode == CUDNN_GRU) {
      numMats = 6;
   }
   
   // Calculate FLOPS
   printf("Forward: %3.0f GFLOPS\n", numMats * 2ull * (bidirectional ? 2 : 1) * hiddenSize * hiddenSize * seqLength * miniBatch * numLayers / (1e6 * timeForward));
   printf("Backward: %3.0f GFLOPS, ", numMats * 4ull * (bidirectional ? 2 : 1) * hiddenSize * hiddenSize * seqLength * miniBatch * numLayers / (1e6 * (timeBackward1 + timeBackward2)));
   printf("(%3.0f GFLOPS), ", numMats * 2ull * (bidirectional ? 2 : 1) * hiddenSize * hiddenSize * seqLength * miniBatch * numLayers / (1e6 * timeBackward1));
   printf("(%3.0f GFLOPS)\n", numMats * 2ull * (bidirectional ? 2 : 1) * hiddenSize * hiddenSize * seqLength * miniBatch * numLayers / (1e6 * timeBackward2));

   // Calculate FLOPS
   fprintf(fp,"Forward: %3.0f GFLOPS\n", numMats * 2ull * (bidirectional ? 2 : 1) * hiddenSize * hiddenSize * seqLength * miniBatch * numLayers / (1e6 * timeForward));
   fprintf(fp,"Backward: %3.0f GFLOPS, ", numMats * 4ull * (bidirectional ? 2 : 1) * hiddenSize * hiddenSize * seqLength * miniBatch * numLayers / (1e6 * (timeBackward1 + timeBackward2)));
   fprintf(fp,"(%3.0f GFLOPS), ", numMats * 2ull * (bidirectional ? 2 : 1) * hiddenSize * hiddenSize * seqLength * miniBatch * numLayers / (1e6 * timeBackward1));
   fprintf(fp,"(%3.0f GFLOPS)\n", numMats * 2ull * (bidirectional ? 2 : 1) * hiddenSize * hiddenSize * seqLength * miniBatch * numLayers / (1e6 * timeBackward2));

   // Make double-sure everything is finished before we copy for result checking.
   cudaDeviceSynchronize();
   
   // *********************************************************************************************************
   // Print checksums.
   // *********************************************************************************************************
   if (true) {
      float* testOutputi;
      float* testOutputh;
      float* testOutputc;
      
      int biDirScale = (bidirectional ? 2 : 1);
      
      testOutputi = (float*)malloc(hiddenSize * seqLength * miniBatch * biDirScale * sizeof(float));
      testOutputh = (float*)malloc(hiddenSize * miniBatch * numLayers * biDirScale * sizeof(float));
      testOutputc = (float*)malloc(hiddenSize * miniBatch * numLayers * biDirScale * sizeof(float));
 
      cudaErrCheck(cudaMemcpy(testOutputi, y, hiddenSize * seqLength * miniBatch * biDirScale * sizeof(float), cudaMemcpyDeviceToHost));
      if (hy != NULL) cudaErrCheck(cudaMemcpy(testOutputh, hy, numLayers * hiddenSize * miniBatch * biDirScale * sizeof(float), cudaMemcpyDeviceToHost));
      if (cy != NULL && RNNMode == CUDNN_LSTM) cudaErrCheck(cudaMemcpy(testOutputc, cy, numLayers * hiddenSize * miniBatch * biDirScale * sizeof(float), cudaMemcpyDeviceToHost));
      
      double checksumi = 0.f;
      double checksumh = 0.f;
      double checksumc = 0.f;
      
      for (int m = 0; m < miniBatch; m++) {
         double localSumi = 0;
         double localSumh = 0;
         double localSumc = 0;
         
         for (int j = 0; j < seqLength; j++) {
            for (int i = 0; i < hiddenSize * biDirScale; i++) {   
               localSumi += testOutputi[j * miniBatch * hiddenSize * biDirScale + m * hiddenSize * biDirScale + i];
            }
         }
         for (int j = 0; j < numLayers * biDirScale; j++) {
            for (int i = 0; i < hiddenSize; i++) {         
               if (hy != NULL) localSumh += testOutputh[j * hiddenSize * miniBatch + m * hiddenSize + i];
               if (cy != NULL) if (RNNMode == CUDNN_LSTM) localSumc += testOutputc[j * hiddenSize * miniBatch + m * hiddenSize + i];
            }
         }
                  
         checksumi += localSumi;
         checksumh += localSumh;
         checksumc += localSumc;
      }
      
      printf("i checksum %E     ", checksumi);
      fprintf(fp,"i checksum %E     ", checksumi);
      if (RNNMode == CUDNN_LSTM) { printf("c checksum %E     ", checksumc); fprintf(fp,"c checksum %E     ", checksumc); }
      printf("h checksum %E\n", checksumh);
      fprintf(fp,"h checksum %E\n", checksumh);
      
      free(testOutputi);
      free(testOutputc);
      free(testOutputh);
   }   
   
   if (true) {
      float* testOutputdi;
      float* testOutputdh;
      float* testOutputdc;

      int biDirScale = (bidirectional ? 2 : 1);
      
      testOutputdi = (float*)malloc(inputSize * seqLength * miniBatch * sizeof(float));
      testOutputdh = (float*)malloc(hiddenSize * miniBatch * numLayers * biDirScale * sizeof(float));
      testOutputdc = (float*)malloc(hiddenSize * miniBatch * numLayers * biDirScale * sizeof(float));
      cudaErrCheck(cudaMemcpy(testOutputdi, dx, seqLength * miniBatch * inputSize * sizeof(float), cudaMemcpyDeviceToHost));
      if (dhx != NULL) cudaErrCheck(cudaMemcpy(testOutputdh, dhx, numLayers * hiddenSize * miniBatch * biDirScale * sizeof(float), cudaMemcpyDeviceToHost));
      if (dcx != NULL) if (RNNMode == CUDNN_LSTM) cudaErrCheck(cudaMemcpy(testOutputdc, dcx, numLayers * hiddenSize * miniBatch * biDirScale * sizeof(float), cudaMemcpyDeviceToHost));
      
      float checksumdi = 0.f;
      float checksumdh = 0.f;
      float checksumdc = 0.f;
      
      for (int m = 0; m < miniBatch; m++) {
         double localSumdi = 0;
         double localSumdh = 0;
         double localSumdc = 0;

         for (int j = 0; j < seqLength; j++) {
            for (int i = 0; i < inputSize; i++) {
               localSumdi += testOutputdi[j * miniBatch * inputSize + m * inputSize + i];
            }
         }

         for (int j = 0; j < numLayers * biDirScale; j++) {
            for (int i = 0; i < hiddenSize; i++) {         
               localSumdh += testOutputdh[j * hiddenSize * miniBatch + m * hiddenSize + i];
               if (RNNMode == CUDNN_LSTM) localSumdc += testOutputdc[j * hiddenSize * miniBatch + m * hiddenSize + i];
            }
         }         

         checksumdi += localSumdi;
         checksumdh += localSumdh;
         checksumdc += localSumdc;
         
      }
      
      printf("di checksum %E    ", checksumdi);
      fprintf(fp,"di checksum %E    ", checksumdi);
      if (RNNMode == CUDNN_LSTM) { printf("dc checksum %E    ", checksumdc); fprintf(fp,"dc checksum %E    ", checksumdc); }
      printf("dh checksum %E\n", checksumdh);
      fprintf(fp,"dh checksum %E\n", checksumdh);
      
      free(testOutputdi);
      free(testOutputdh);
      free(testOutputdc);
   }

   if (true) {
      float* testOutputdw;
      testOutputdw = (float*)malloc(weightsSize);
 
      cudaErrCheck(cudaMemcpy(testOutputdw, dw, weightsSize, cudaMemcpyDeviceToHost));
      
      double checksumdw = 0.;
            
      for (int i = 0; i < weightsSize / sizeof(float); i++) {
         checksumdw += testOutputdw[i];
      }
      
      printf("dw checksum %E\n", checksumdw);
      fprintf(fp,"dw checksum %E\n", checksumdw);
      
      free(testOutputdw);
   }   
  
   cudaFree(x);
   cudaFree(hx);
   cudaFree(cx);
   cudaFree(y);
   cudaFree(hy);
   cudaFree(cy);
   cudaFree(dx);
   cudaFree(dhx);
   cudaFree(dcx);
   cudaFree(dy);
   cudaFree(dhy);
   cudaFree(dcy);
   cudaFree(workspace);
   cudaFree(reserveSpace);
   cudaFree(w);
   cudaFree(dw);
   
   cudnnDestroy(cudnnHandle);
   fclose(fp);
   return 0;
}



