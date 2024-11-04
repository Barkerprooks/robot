#!/bin/bash

PICO_SDX=sda1

rm -rf build
mkdir -p build && cd build

cmake -DPICO_BOARD=pico_w .. && make

sudo mount /dev/$PICO_SDX /mnt
sudo cp robot.uf2 /mnt
sudo umount /mnt
