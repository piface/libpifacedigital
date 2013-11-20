/* pifacedigital.h
 * A simple static library for controlling PiFace Digital.
 * See mcp23s17.h for more register definitions.
 */

#ifndef _MCP23S17_H
#define _MCP23S17_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


#define OUTPUT 0x12 // GPIOA
#define INPUT 0x13 // GPIOB


int pifacedigital_open_noinit(uint8_t hw_addr); // open without initialising
int pifacedigital_open(uint8_t hw_addr);
int pifacedigital_close(uint8_t hw_addr);
uint8_t pifacedigital_read_reg(uint8_t reg, uint8_t hw_addr);
int pifacedigital_write_reg(uint8_t data, uint8_t reg, uint8_t hw_addr);
uint8_t pifacedigital_read_bit(uint8_t bit_num, uint8_t reg, uint8_t hw_addr);
int pifacedigital_write_bit(uint8_t data,
                            uint8_t bit_num,
                            uint8_t reg,
                            uint8_t hw_addr);


#ifdef __cplusplus
}
#endif

#endif
