# Download and Setup

Installing libvot requires minimal efforts. To use its full functionality, you need to install some additional dependencies.

## Additional Dependencies
### Optional: OpenCV (>2.4)

Several modules of libvot depends on OpenCV. Though it is not required, we highly recommend you to build libvot with OpenCV.

See the installation guide of OpenCV [here](https://github.com/Itseez/opencv).

### Optional: Install CUDA

In order to build or run libvot with GPU support, both NVIDIA's Cuda Toolkit (7.5) and
cuDNN (v5 for cuda 7.5) need to be installed.

#### Check NVIDIA Compute Capability of your GPU card

Check compute capability at [here](https://developer.nvidia.com/cuda-gpus).

#### Download and install Cuda Toolkit

Install [Cuda Toolkit 7.5](https://developer.nvidia.com/cuda-downloads)

Install the toolkit into e.g. `/usr/local/cuda`

#### Download and install cuDNN

Download [cuDNN v5](https://developer.nvidia.com/cudnn)

Uncompress and copy the cuDNN files into the toolkit directory. Assuming the
toolkit is installed in `/usr/local/cuda`, run the following commands (edited
to reflect the cuDNN version you downloaded):

``` bash
tar xvzf cudnn-7.5-<platform>-x64-v5.0.tgz
sudo cp cudnn-7.5-<platform>-x64-v5.0/cudnn.h /usr/local/cuda/include
sudo cp cudnn-7.5-<platform>-x64-v5.0/libcudnn* /usr/local/cuda/lib
sudo chmod a+r /usr/local/cuda/include/cudnn.h /usr/local/cuda/lib/libcudnn*
```

You may also put cuDNN libs and includes in another folder. Then you have to change the CMake configurations to reflect this path.