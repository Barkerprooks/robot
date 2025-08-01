# Robot
Building a robot using an RP2040 as the main microcontroller (Raspberry Pi Pico W)

## Easy build (+ install)
Just run the script: `make.sh`

You will need the dependencies found on the [pico-sdk](https://github.com/raspberrypi/pico-sdk) homepage

Optionally, with a Pico W plugged in, Hold down the `BOOTSEL` button on the Pico, then plug it into your development machine. You can supply an argument for `--install` which will copy the correct file over to the supplied path. The argument `--mount-install` allows you to install the binary on an unmounted block device. _note_: `sudo` permissions for mounting are required.

## Build
You will need the [pico-sdk](https://github.com/raspberrypi/pico-sdk) repository. Make sure to read the documentation and set the `PICO_SDK_PATH` environment variable. After that, run these commands in the root directory
```
mkdir build
cd build
cmake -DPICO_BOARD=pico_w .. && make
```

## Install
Hold down the `BOOTSEL` button on the Pico, then plug it into your development machine, and copy the `uf2` file over to the RPI media storage device. The storage device will unmount itself after installation.

After installing you can view the output from the board using the following command
```
sudo screen /dev/ttyACM0 115200
```
