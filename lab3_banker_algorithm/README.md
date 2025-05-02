# Lab3: Banker Algorithm

## Running the Python Version

To run the Python implementation, use the following commands in your terminal:

```bash
cd lab3_banker_algorithm/src/python
python3 main.py
```

By default, the program uses the input file `../data/example_1.json` and logs output to `../data/output.log`.

To use custom file paths, open `src/python/main.py` and modify the variables `JSON_FILE` and `LOG_FILE` accordingly.

## Running the C++ Version

To build and run the C++ implementation, use the following commands in your terminal:

```bash
cd lab3_banker_algorithm

rm -rf build # only if "build/" directory exists
mkdir build
cd build

cmake ..
make # or: cmake --build .

cd ../bin
./banker_algorithm
```

To run with a custom data file, edit the last line of this script and specify your desired path before executing it.

By default, it reads input from `../data/example_1.json`.

To use a different input JSON file, pass the file path as a command-line argument:

```bash
cd bin
./bank_service ../data/your_input.json
```

The script `build_and_run.sh` automatically builds and runs the C++ program. It uses `cmake` and `make` to compile the source code, and then executes the resulting binary. Use this script if you have made changes to the source code and need to recompile.

```bash
chmod +x build_and_run.sh
./build_and_run.sh
```

The script `run.sh` simply runs the precompiled executable without rebuilding the project. Use this script if you haven't modified the source code, or if you have only changed the input JSON file path.To run with a customed data file, edit the last line of this script and specify your customed path before executing it.

```bash
chmod +x run.sh
./run.sh
```
