#!/bin/bash

script_dir="$(dirname "$(realpath "$0")")"
cd "$script_dir"/bin

./bank_service
