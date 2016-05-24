#!/usr/bin/env sh
cd test && ./unit_test
cd ../src/example
./image_search ../../../test_data/list ./vocab_out 6 8 1