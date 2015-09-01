#libvot - A multithread library for image retrieval
##Introduction

*libvot* is an implementation of vocabulary tree, which is an algorithm widely used in image retrieval and computer vision. This project is inspired by Snavely's [VocabTree2](https://github.com/snavely/VocabTree2) project. Currently this library is under active development for research purpose. 

##Installation

The build system of libvot is based on [CMake](http://cmake.org). To take full advantages of the new features in C++11, we require the version of CMake to be 3.1 or above. Current we have tested our program under Linux (Ubuntu) and MacOS (10.10). The common steps to build the library is:

1. Extract source files.
2. Create build directory and change to it.
3. Run CMake to configure the build tree.
4. Build the software using selected build tool.
5. Run unit_test in the 'test' folder
5. See src/example for the use of this library.

On Unix-like systems with GNU Make as build tool, the following sequence of commands
can be used to compile the source code.

    $ cd libvot
    $ mkdir build && cd build
    $ cmake ..
    $ make
    $ cd test && ./unit_test

## Contact
For inquiries and suggestions, please send your emails to 
<shentianweipku@gmail.com>

