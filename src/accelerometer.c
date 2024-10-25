#include "accelerometer.h"
#include "spi.h"

#include "pico/stdlib.h"

bool initialize_accelerometer(const uint16_t sample_rate, const uint8_t range) {
    uint8_t control_1_options = 0x07; // start with xyz enabled
    uint8_t control_4_options = 0x88; // start with 

    // initialize the chip select pin (normal GPIO on/off)
    gpio_init(SPI_PIN_CS);
    gpio_set_dir(SPI_PIN_CS, GPIO_OUT);
    gpio_put(SPI_PIN_CS, 1);

    // start up SPI interface for accelerometer at 1MHz
    spi_init(spi0, 1000000);
    spi_set_format(spi0, 8, 1, 1, SPI_MSB_FIRST); // big-endian
    
    // configure pins for SPI
    gpio_set_function(SPI_PIN_SCK, GPIO_FUNC_SPI);
    gpio_set_function(SPI_PIN_RX, GPIO_FUNC_SPI);
    gpio_set_function(SPI_PIN_TX, GPIO_FUNC_SPI);
    
    // ask for the value at register 0x0F
    if (spi_reg_get_byte(REG_ACCELEROMETER_DEVICE_ID) != 0x33)
        return false;
    
    switch (sample_rate) {
        case 1:
            control_1_options |= 0x10;
            break;
        case 10:
            control_1_options |= 0x20;
            break;
        case 25:
            control_1_options |= 0x30;
            break;
        case 50:
            control_1_options |= 0x40;
            break;
        case 100:
            control_1_options |= 0x50;
            break;
        case 200:
            control_1_options |= 0x60;
            break;
        case 400:
            control_1_options |= 0x70;
            break;
        case 1600:
            control_1_options |= 0x80;
            break;
        case 5000:
            control_1_options |= 0x90;
            break;
        default: return false;
    }

    switch(range) {
        case 2: 
            control_4_options |= 0x00;
            break;
        case 4:
            control_4_options |= 0x10;
            break;
        case 8:
            control_4_options |= 0x20;
            break;
        case 16:
            control_4_options |= 0x30;
            break;
        default: return false;
    }

    // apply settings and activate the device
    spi_reg_put_byte(REG_ACCELEROMETER_CONTROL_1, control_1_options);
    spi_reg_put_byte(REG_ACCELEROMETER_CONTROL_4, control_4_options);

    return true;
}

int16_t accelerometer_get_axis(uint8_t axis_pin) {
    uint16_t value = 0xff;
    while (value == 0xff)
        value = spi_reg_get_short(axis_pin);
    return value;
}

int16_t accelerometer_get_x() {
    return accelerometer_get_axis(REG_ACCELEROMETER_X);
}

int16_t accelerometer_get_y() {
    return accelerometer_get_axis(REG_ACCELEROMETER_Y);
}

int16_t accelerometer_get_z() {
    return accelerometer_get_axis(REG_ACCELEROMETER_Z);
}