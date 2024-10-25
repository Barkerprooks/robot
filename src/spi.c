#include "hardware/spi.h"
#include "pico/stdlib.h"

#include "spi.h"

void spi_initialize_pins() {
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
}

int8_t spi_reg_get_byte(const uint8_t reg) {
    uint8_t data = reg | 0x80;
    int8_t value;

    gpio_put(SPI_PIN_CS, 0); // select the desired chip
    spi_write_blocking(spi0, &data, 1);
    sleep_ms(10);

    spi_read_blocking(spi0, 0, &value, 1);
    
    gpio_put(SPI_PIN_CS, 1); // de-select the chip
    sleep_ms(10);
    
    return value;
}

int16_t spi_reg_get_short(const uint8_t reg) {
    uint8_t data = reg | 0xc0; // read request + auto inc
    int8_t bytes[2];

    gpio_put(SPI_PIN_CS, 0); // select the desired chip
    spi_write_blocking(spi0, &data, 2);
    sleep_ms(10);

    spi_read_blocking(spi0, 0, bytes, 2);
    
    gpio_put(SPI_PIN_CS, 1); // de-select the chip
    sleep_ms(10);
    
    return ((int16_t) bytes[0]) | (((int16_t) bytes[1]) << 8);
}

void spi_reg_put_byte(const uint8_t reg, const uint8_t data) {
    uint8_t msg[2] = { reg, data };

    gpio_put(SPI_PIN_CS, 0);
    spi_write_blocking(spi0, msg, 2);
    
    gpio_put(SPI_PIN_CS, 1);
}