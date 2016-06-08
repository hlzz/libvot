# sudo add-apt-repository "deb http://ppa.launchpad.net/alexei.colin/opencv/ubuntu precise main" -y
sudo add-apt-repository -y ppa:philip5/extra
sudo apt-get install autoconf automake libtool unzip
sudo apt-get update -qq
# sudo apt-get -qq install libopencv-dev #libopencv-nonfree-dev

# OpenCV dependencies - Details available at: http://docs.opencv.org/trunk/doc/tutorials/introduction/linux_install/linux_install.html
sudo apt-get install -y build-essential
sudo apt-get install -y cmake git libgtk2.0-dev pkg-config libavcodec-dev libavformat-dev libswscale-dev
sudo apt-get install -y python-dev python-numpy libtbb2 libtbb-dev libjpeg-dev libpng-dev libtiff-dev libjasper-dev libdc1394-22-dev

# clone the latest opencv repo in github
git clone -b 2.4 https://github.com/Itseez/opencv.git
cd opencv
# set build instructions for linux 
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=RELEASE -DCMAKE_INSTALL_PREFIX=/usr/local -DBUILD_opencv_nonfree=ON -DWITH_TBB=ON -DWITH_V4L=ON -DWITH_QT=ON -DWITH_OPENGL=ON -DWITH_GTK=ON ..
make -j4
sudo make install

# return to the root directory
cd ../../