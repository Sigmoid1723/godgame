#!/bin/bash

mkdir -p ./build
pushd ./build
g++ ../code/sdl_godgame.cpp -o godgame -g `sdl2-config --cflags --libs`
popd
