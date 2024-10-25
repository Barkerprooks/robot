# robot :)

building a robot using an RP2040 (RPi Pico W)

## install
You will need the [pico-sdk](https://github.com/raspberrypi/pico-sdk) repository.
Make sure to read the documentation and set the PICO_SDK_HOME environment variable. 

The `install.sh` script has been provided to make development easier on a linux machine 

After installing you can view the output from the board using the following command
```
sudo screen /dev/ttyACM0 115200
```