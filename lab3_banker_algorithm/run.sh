#!/bin/bash

script_dir="$(dirname "$(realpath "$0")")"
cd "$script_dir"/bin

./banker_algorithm ../data/example_2.json
