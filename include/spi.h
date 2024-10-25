#ifndef __spi_h__
#define __spi_h__

#define SPI_PIN_TX 19
#define SPI_PIN_SCK 18
#define SPI_PIN_CS 17
#define SPI_PIN_RX 16

#include <stdint.h>

void spi_initialize_pins();

int8_t spi_reg_get_byte(const uint8_t reg);
int16_t spi_reg_get_short(const uint8_t reg);

void spi_reg_put_byte(const uint8_t reg, const uint8_t data);

#endif