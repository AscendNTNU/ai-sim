#!/bin/bash

cd build
g++ ../gui.cpp -o sim -lGL `sdl2-config --cflags --libs`
./sim
