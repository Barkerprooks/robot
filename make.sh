#!/bin/bash

set -e

SDK_DIR_NAME=pico-sdk

if ! [ -d $SDK_DIR_NAME ]; then
    echo "Downloading and setting up pico-sdk."

    # Get the SDK
    git clone https://github.com/raspberrypi/pico-sdk.git $SDK_DIR_NAME

    # Use the develop brach because of some recent bugs. You can probably keep it like this
    # until the develop branch causes issues itself.

    # Update the submodules so that we have those repositories as well.
    cd $SDK_DIR_NAME && git switch develop && git submodule update --init
fi

mkdir -p ./build # create a folder called 'build'. ignore if it already exists
cd ./build # execute from build/ for the next two instructions
# Code is written and tested for Pico W
cmake -DPICO_BOARD=pico_w .. && make
cd .. # return to the parent folder

# After building, optionally supply an install path
if [ "$1" == "--mount-install" ] || [ "$1" == "-mi" ]; then
    # For machines that don't automount an external filesystem.
    if ! [ -b "$2" ]; then
        echo "$2 is not a block device."
        exit 0
    fi
    sudo mount "$2" /mnt
    sudo cp ./build/robot.uf2 /mnt && sync
    sudo umount /mnt
elif [ "$1" == "--install" ] || [ "$1" == "-i" ]; then
    if ! [ -d "$2" ]; then
        echo "$2 is not a location."
        exit 0
    fi
    cp ./build/robot.uf2 $2 && sync
fi