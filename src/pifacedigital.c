#include <stdio.h>

#include <stdint.h>
#include <unistd.h>
#include <mcp23s17.h>
#include "pifacedigital.h"


// PiFace Digital is always at /dev/spidev0.0
static const int bus = 0, chip_select = 0;
static int pfd_count = 0; // number of PiFace Digitals
static int mcp23s17_fd = 0; // MCP23S17 SPI file descriptor


int pifacedigital_open_noinit(uint8_t hw_addr)
{
    // All PiFace Digital are connected to the same SPI bus, only need 1 fd.
    if (pfd_count <= 0) {
        if ((mcp23s17_fd = mcp23s17_open(bus, chip_select)) < 0) {
            return -1;
        }
    }
    pfd_count++;
    return mcp23s17_fd; // returns the fd in case you want to use it
}

int pifacedigital_open(uint8_t hw_addr)
{
    if ((mcp23s17_fd = pifacedigital_open_noinit(hw_addr)) < 0) {
        return -1;
    }

    // set up config register
    uint8_t ioconfig = BANK_OFF | \
                       INT_MIRROR_OFF | \
                       SEQOP_OFF | \
                       DISSLW_OFF | \
                       HAEN_ON | \
                       ODR_OFF | \
                       INTPOL_LOW;
    mcp23s17_write_reg(ioconfig, IOCON, hw_addr, mcp23s17_fd);

    // I/O direction
    mcp23s17_write_reg(0x00, IODIRA, hw_addr, mcp23s17_fd);
    mcp23s17_write_reg(0xff, IODIRB, hw_addr, mcp23s17_fd);

    // GPIOB pull ups
    mcp23s17_write_reg(0xff, GPPUB, hw_addr, mcp23s17_fd);

    // enable interrupts
    mcp23s17_write_reg(0xff, GPINTENB, hw_addr, mcp23s17_fd);

    return mcp23s17_fd;
}

int pifacedigital_close(uint8_t hw_addr)
{
    pfd_count--;
    if (pfd_count < 0)
        return 0;

    // disable interrupts if enabled
    const uint8_t intenb = mcp23s17_read_reg(GPINTENB, hw_addr, mcp23s17_fd);
    if (intenb) {
        mcp23s17_write_reg(0, GPINTENB, hw_addr, mcp23s17_fd);
        // now do some other interrupt stuff...
        // TODO
    }

    // if no more PiFace Digital's, close the fd
    if (pfd_count <= 0) {
        pfd_count = 0;
        return close(mcp23s17_fd);
    }
    return 0;
}

uint8_t pifacedigital_read_reg(uint8_t reg, uint8_t hw_addr)
{
    return mcp23s17_read_reg(reg, hw_addr, mcp23s17_fd);
}

int pifacedigital_write_reg(uint8_t data, uint8_t reg, uint8_t hw_addr)
{
    return mcp23s17_write_reg(data, reg, hw_addr, mcp23s17_fd);
}

uint8_t pifacedigital_read_bit(uint8_t bit_num, uint8_t reg, uint8_t hw_addr)
{
    return (mcp23s17_read_reg(reg, hw_addr, mcp23s17_fd) >> bit_num) & 0x1;
}

int pifacedigital_write_bit(uint8_t data,
                            uint8_t bit_num,
                            uint8_t reg,
                            uint8_t hw_addr)
{
    uint8_t reg_data = mcp23s17_read_reg(reg, hw_addr, mcp23s17_fd);
    if (data) {
        reg_data |= 1 << bit_num; // set
    } else {
        reg_data &= 0xff ^ (1 << bit_num); // clear
    }
    return pifacedigital_write_reg(reg_data, reg, hw_addr);
}
