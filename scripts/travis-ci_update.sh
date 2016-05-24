#!/usr/bin/env sh
if ( test "`uname -s`" = "darwin" )
then
    #cmake v2.8.12 is installed on the mac workers now
    #brew update
    #brew install cmake
    echo
else
    #install a newer cmake since at this time travis only has version 2.8.7
    echo "yes" | sudo add-apt-repository ppa:kalakris/cmake
    sudo apt-get update -qq
    sudo apt-get install cmake
fi