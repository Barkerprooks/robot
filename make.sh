#!/bin/bash

set -e

mkdir -p ./build
cd ./build
cmake -DPICO_BOARD=pico_w ..
make
cd ..

if [ "$1" == "install" ]; then
    sudo mount "$2" /mnt
    sudo cp ./build/robot.uf2 /mnt && sync
    sudo umount /mnt
fi