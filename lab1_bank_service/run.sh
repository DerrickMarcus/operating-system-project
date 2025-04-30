#!/bin/bash

rm -rf build

mkdir build
cd build

cmake ..

make -j$(nproc)

cd ../bin

./bank_service
