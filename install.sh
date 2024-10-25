#!/bin/bash

PICO_SDX=sda1

mkdir -p build && cd build

cmake .. && make

sudo mount /dev/$PICO_SDX /mnt
sudo cp robot.uf2 /mnt
sudo umount /mnt