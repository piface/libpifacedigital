/**
 * @file  pifacedigital.h
 * @brief A simple static library for controlling PiFace Digital.
 * See mcp23s17.h for more register definitions.
 *
 * Copyright (C) 2013 Thomas Preston <thomas.preston@openlx.org.uk>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _PIFACEDIGITAL_H
#define _PIFACEDIGITAL_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


#define OUTPUT 0x12 // GPIOA
#define INPUT 0x13 // GPIOB


/**
 * Opens and initialises a PiFace Digital with the specified hardware
 * address. Returns a file descriptor for making raw SPI transactions to
 * the MCP23S17 (for advanced users only).
 *
 * Example:
 *
 *     pifacedigital_open(0);
 *     int pifacedigital_fd = pifacedigital_open(0); // advanced
 *
 * @param hw_addr The hardware address (configure with jumpers: JP1 and JP2).
 */
int pifacedigital_open(uint8_t hw_addr);

/**
 * Opens a PiFace Digital with the specified hardware address  without
 * initialising it. Returns a file descriptor for making raw SPI transactions
 * to the MCP23S17 (for advanced users only).
 *
 * Example:
 *
 *     pifacedigital_open_noinit(0);
 *     int pifacedigital_fd = pifacedigital_open_noinit(0); // advanced
 *
 * @param hw_addr The hardware address (configure with jumpers: JP1 and JP2).
 */
int pifacedigital_open_noinit(uint8_t hw_addr);

/**
 * Closes a PiFace Digital with the specified hardware address (turns
 * off interrupts, closes file descriptor if it is no longer needed).
 *
 * Example:
 *
 *     pifacedigital_close(0);
 *
 * @param hw_addr The hardware address (configure with jumpers: JP1 and JP2).
 */
void pifacedigital_close(uint8_t hw_addr);

/**
 * Reads a register from PiFace Digital.
 *
 * Example (read INPUT from PiFace Digital 0):
 *
 *     uint8_t value = pifacedigital_read_reg(INPUT, 0);
 *
 * @param reg The register to read from (example: INPUT, OUTPUT).
 * @param hw_addr The hardware address (configure with jumpers: JP1 and JP2).
 */
uint8_t pifacedigital_read_reg(uint8_t reg, uint8_t hw_addr);

/**
 * Writes to a register on PiFace Digital.
 *
 * Example (write 0xaa to OUTPUT on PiFace Digital 0):
 *
 *     pifacedigital_write_reg(0xaa, OUTPUT, 0);
 *
 * @param data The byte to write.
 * @param reg The register to write to (example: INPUT, OUTPUT, GPPUB).
 * @param hw_addr The hardware address (configure with jumpers: JP1 and JP2).
 */
void pifacedigital_write_reg(uint8_t data, uint8_t reg, uint8_t hw_addr);

/**
 * Read a bit from a register on PiFace Digital.
 *
 * Example (read bit 3 of INPUT on PiFace Digital 0):
 *
 *     uint8_t bit_value = pifacedigital_read_bit(3, INPUT, 0);
 *
 * @param bit_num The bit number to read from.
 * @param reg The register to write to (example: INPUT, OUTPUT, GPPUB).
 * @param hw_addr The hardware address (configure with jumpers: JP1 and JP2).
 */
uint8_t pifacedigital_read_bit(uint8_t bit_num, uint8_t reg, uint8_t hw_addr);

/**
 * Writes to a bit on a register on PiFace Digital.
 *
 * Example (write 1 to bit 3 of OUTPUT on PiFace Digital 0):
 *
 *     pifacedigital_write_bit(1, 3, OUTPUT, 0);
 *
 * @param data The value to write.
 * @param bit_num The bit number to read from.
 * @param reg The register to write to (example: INPUT, OUTPUT, GPPUB).
 * @param hw_addr The hardware address (configure with jumpers: JP1 and JP2).
 */
void pifacedigital_write_bit(uint8_t data,
                             uint8_t bit_num,
                             uint8_t reg,
                             uint8_t hw_addr);

/**
 * Read from the input port on PiFace Digital 0. If you want to read from
 * other ports or PiFace Digital's with other hardware addresses then use
 * the more powerful function <pifacedigital_read_bit>"()".
 *
 * Example (read from bit 6 of INPUT on PiFace Digital 0):
 *
 *     pifacedigital_digital_read(6);
 *
 * @param pin_num The pin number to read from.
 */
uint8_t pifacedigital_digital_read(uint8_t pin_num);

/**
 * Write to the output port on PiFace Digital 0. If you want to write to
 * other ports or PiFace Digital's with other hardware addresses then use
 * the more powerful function <pifacedigital_write_bit>"()".
 *
 * Example (write 1 to bit 3 of OUTPUT on PiFace Digital 0):
 *
 *     pifacedigital_digital_write(3, 1);
 *
 * @param pin_num The pin number to write to.
 * @param value The value to write.
 */
void pifacedigital_digital_write(uint8_t pin_num, uint8_t value);

/**
 * Enables interrupts and exports to the GPIO connection from
 * the PiFaceDigital.
 *
 * Example:
 *    pifacedigital_enable_interrupts();
 *
 * @return int 0 on success
 */
int pifacedigital_enable_interrupts();

/**
 * Disables interrupts and exports to the GPIO connection from
 * the PiFaceDigital.
 *
 * Example:
 *    pifacedigital_disable_interrupts();
 *
 * @return int 0 on success
 */
int pifacedigital_disable_interrupts();

/**
 * Waits for a change of any of the input pins on the PiFace
 * Digital, and returns the current state of all inputs as each
 * bit of the 8-bit return value.
 * Requires that interrupts are enabled with
 * pifacedigital_enable_interrupts() first
 *
 * Example:
 *
 *     pifacedigital_open(0);
 *     pifacedigital_enable_interrupts();
 *     uint8_t inputs = pifacedigital_wait_for_input(5, 0);
 *     pifacedigital_close(0);
 *
 * @param hw_addr The hardware address (configure with jumpers: JP1 and JP2).
 * @param timeout Maximum ms to wait for input, -1 for forever
 * @return uint8_t 8-bit input states
 */
uint8_t pifacedigital_wait_for_input(int timeout, uint8_t hw_addr);

/**
* This is similar to pifacedigital_wait_for_input except for 2 things:
*     1. The return value is > 0 if some interrupts were triggered. Otherwise it is <= 0.
*     2. Instead of returning the uint8_t that represent the states of the pins, they are copied into data_ptr;
*/
int pifacedigital_wait_for_input2(uint8_t *data_ptr, int timeout, uint8_t hw_addr);

#ifdef __cplusplus
}
#endif

#endif
