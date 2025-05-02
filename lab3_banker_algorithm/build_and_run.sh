#!/bin/bash

script_dir="$(dirname "$(realpath "$0")")"
cd "$script_dir"

rm -rf build

mkdir build
cd build

cmake ..

make -j$(nproc)

cd ../bin

./banker_algorithm ../data/example_1.json
