#!/usr/bin/env sh
git submodule init & git submodule update
mkdir build
cd build
cmake ../
make -j8

# test
cd test && ./unit_test
cd ../src/example
./image_search ../../../test_data/list ./vocab_out 6 8 1
