#!/bin/bash

mkdir -p work
cd work
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build . --target install ../app/addons/godot_sfxr/
