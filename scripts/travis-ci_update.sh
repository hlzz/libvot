#!/usr/bin/env sh
if ( test "`uname -s`" = "darwin" )
then
    #cmake v2.8.12 is installed on the mac workers now
    #brew update
    #brew install cmake
    echo
else
    #install a newer cmake since at this time travis only has version 2.8.7
    sudo add-apt-repository --yes ppa:kubuntu-ppa/backports
    sudo add-apt-repository "deb http://ppa.launchpad.net/alexei.colin/opencv/ubuntu precise main" -y
    sudo apt-get update -qq
    sudo apt-get install cmake
    sudo apt-get -qq install libopencv-dev libopencv-nonfree-dev
fi