FROM ubuntu:14.04

# Install apt-getable dependencies (opencv dependencies)
RUN apt-get update && \
    apt-get install -y software-properties-common && \
    add-apt-repository --yes ppa:xqms/opencv-nonfree && apt-get update && \
    apt-get install -y \
    git \
    autoconf \
    automake \
    libtool \
    unzip \
    build-essential \
    cmake \
    libgtk2.0-dev \
    pkg-config \
    libavcodec-dev \
    libavformat-dev \
    libswscale-dev \
    python-dev \
    python-numpy \
    libtbb2 \
    libtbb-dev \
    libjpeg-dev \
    libpng-dev \
    libtiff-dev \
    libjasper-dev \
    libdc1394-22-dev \
    libboost1.55-all-dev \
    libboost-python1.55-dev \
    libopencv-dev \
    libopencv-nonfree-dev


# libvot
RUN \
    mkdir /source && cd /source && \
    git clone https://github.com/hlzz/libvot.git && \
    cd /source/libvot && \
    git submodule init && git submodule update && \
    mkdir build && \
    cd build && \
    cmake ../ && \
    make -j4

