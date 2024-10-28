# Robot
Building a robot using an RP2040 as the main microcontroller (Raspberry Pi Pico W)

## Build
You will need the [pico-sdk](https://github.com/raspberrypi/pico-sdk) repository. Make sure to read the documentation and set the `PICO_SDK_PATH` environment variable. After that, run these commands in the root directory
```
mkdir build
cd build
cmake .. && make
```

## Install
The `install.sh` script has been provided to make development easier on a linux machine. it will build the project, then mount the Pico (you should have plugged it in with the BOOLSEL button held down), then it will transfer the built `uf2` file to the Pico, unmounting it and promptly running the firmware 

After installing you can view the output from the board using the following command
```
sudo screen /dev/ttyACM0 115200
```
