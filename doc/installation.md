# Download and Setup

Installing libvot requires minimal efforts. To use its full functionality, you need to install some additional dependencies.

## Additional Dependencies
### Required: gflags, glog and glog (if test is enabled)
*gflags* is a handy library to parse command-line options. *glog* is a logging utility. Together with *gtest*, a unit test suite, they are made public by google to facilitate the development of modern C++ software.  
To install them on Mac:

```bash
brew update
brew glog gflags gtest
```

To install them on Linux (e.g. ubuntu):

```bash
sudo apt-get install libgoogle-glog-dev libgflags-dev libgtest-dev
```

Note: However, `apt-get` mysteriously doesn't install gtest lib even if you run `sudo apt-get install libgtest-dev`. To make things work, you have to manually build gtest from source (apt-get does ship source code in `/usr/src/gtest`). See this [post](https://askubuntu.com/questions/145887/why-no-library-files-installed-for-google-test?newreg=b9a4644b541d4d99aac52be1822d9b2b) for details.

### Optional: Boost (>1.55)

Boost is a set of powerful C++ libraries. However, it also has a huge code base and takes a long time to build. If you haven't done so, you can consult the following pages for your platform:

* [Unix Variants](http://www.boost.org/doc/libs/1_61_0/more/getting_started/unix-variants.html)
* [Windows](http://www.boost.org/doc/libs/1_55_0/more/getting_started/windows.html)

Note that if you would like to build the python export, you also need to install `boost-python`. On Mac, boost and boost-python can be easily installed via `brew`:

```bash
brew update
brew install boost
brew install boost-python --with-python
```
On Linux, specifically ubuntu, you can use `apt-get`:

```bash
sudo apt-get install libboost-all-dev
```

### Optional: OpenCV (>2.4)

Several modules of libvot depends on OpenCV. Though it is not required, we highly recommend you to build libvot with OpenCV.

See the installation guide of OpenCV [here](https://github.com/Itseez/opencv).

### Optional: Install CUDA

In order to build or run libvot with GPU support, both NVIDIA's Cuda Toolkit (7.5) and
cuDNN (v5 for cuda 7.5) need to be installed.

#### Check NVIDIA Compute Capability of your GPU card

Check compute capability [here](https://developer.nvidia.com/cuda-gpus).

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

## Install from Source
The build system of libvot is based on [CMake](http://cmake.org). To take full advantages of the new features in C++11, we require the version of CMake to be 2.8.12 or above. Current we have tested our project under Linux (Ubuntu 14.04, CentOS 7) and MacOS (10.10) using gcc. The common steps to build the library are:

1. Extract source files.
2. Create build directory and change to it.
3. Run CMake to configure the build tree.
4. Build the software using selected build tool.
5. Run "make test"
6. See src/example for some demo programs of this library.

On Unix-like systems with GNU Make as the build tool, the following sequence of commands can be used to compile the source code.

    $ cd libvot
    $ git submodule init & git submodule update  
    $ mkdir build && cd build
    $ cmake ..
    $ make && make test
