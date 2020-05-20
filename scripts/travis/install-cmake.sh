#!/bin/sh

sudo apt remove --purge --auto-remove cmake

VERSION=3.17
BUILD=2

mkdir -p ~/temp
cd ~/temp

wget https://cmake.org/files/v$VERSION/cmake-$VERSION.$BUILD.tar.gz
tar -xzvf cmake-$VERSION.$BUILD.tar.gz
cd cmake-$VERSION.$BUILD/

./bootstrap

make -j$(nproc)
sudo make install