#!/bin/bash
mkdir -p build && cd build
cmake -DPICO_BOARD=pico_w .. && make
mv robot.uf2 .. && cd .. && rm -rf build