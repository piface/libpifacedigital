#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <mcp23s17.h>
#include "pifacedigital.h"


// PiFace Digital is always at /dev/spidev0.0
static const int bus = 0, chip_select = 0;
static int pfd_count = 0; // number of PiFace Digitals
/* MCP23S17 SPI file descriptor. All PiFace Digitals are connected to
 * the same SPI bus, only need 1 fd. Keeping this hiden from novice
 * PiFace Digital users.
 * If you want to make raw SPI transactions to the device then try using
 * libmcp23s17.
 */
static int mcp23s17_fd = 0;


int pifacedigital_open_noinit(uint8_t hw_addr)
{
    // if initialising the first PiFace Digital, open the fd
    if (pfd_count <= 0) {
        if ((mcp23s17_fd = mcp23s17_open(bus, chip_select)) < 0) {
            fprintf(stderr,
                    "pifacedigital_open_noinit: ERROR Could not open MCP23S17 "
                    "device.");
            return -1;
        }
    }
    pfd_count++;
    return mcp23s17_fd; // returns the fd, for advanced users.
}

int pifacedigital_open(uint8_t hw_addr)
{
    if ((mcp23s17_fd = pifacedigital_open_noinit(hw_addr)) < 0) {
        fprintf(stderr,
                "pifacedigital_open: ERROR Could not open MCP23S17 device.");
        return -1;
    }

    // set up config register
    const uint8_t ioconfig = BANK_OFF | \
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

    return mcp23s17_fd; // returns the fd, for advanced users.
}

void pifacedigital_close(uint8_t hw_addr)
{
    if (pfd_count <= 0)
        return;

    pfd_count--;

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
        close(mcp23s17_fd);
    }
}

uint8_t pifacedigital_read_reg(uint8_t reg, uint8_t hw_addr)
{
    return mcp23s17_read_reg(reg, hw_addr, mcp23s17_fd);
}

void pifacedigital_write_reg(uint8_t data, uint8_t reg, uint8_t hw_addr)
{
    mcp23s17_write_reg(data, reg, hw_addr, mcp23s17_fd);
}

uint8_t pifacedigital_read_bit(uint8_t bit_num, uint8_t reg, uint8_t hw_addr)
{
    return mcp23s17_read_bit(bit_num, reg, hw_addr, mcp23s17_fd);
}

void pifacedigital_write_bit(uint8_t data,
                             uint8_t bit_num,
                             uint8_t reg,
                             uint8_t hw_addr)
{
    mcp23s17_write_bit(data, bit_num, reg, hw_addr, mcp23s17_fd);
}

uint8_t pifacedigital_digital_read(uint8_t pin_num)
{
    return pifacedigital_read_bit(pin_num, INPUT, 0);
}

void pifacedigital_digital_write(uint8_t pin_num, uint8_t value)
{
    pifacedigital_write_bit(value, pin_num, OUTPUT, 0);
}


int pifacedigital_enable_interrupts()
{
    unsigned int gpio = 25;
    int fd, len;
    char str_gpio[3];
    char filename[33];

    if ((fd = open("/sys/class/gpio/export", O_WRONLY)) < 0)
    {
        return -1;
    }
    len = snprintf(str_gpio, sizeof(str_gpio), "%d", gpio);
    write(fd, str_gpio, len);
    close(fd);

    snprintf(filename, sizeof(filename), "/sys/class/gpio/gpio%d/direction", gpio);
    if ((fd = open(filename, O_WRONLY)) < 0)
        return -1;

    write(fd, "in", 3);
    close(fd);

    snprintf(filename, sizeof(filename), "/sys/class/gpio/gpio%d/edge", gpio);
    if ((fd = open(filename, O_WRONLY)) < 0)
        return -1;

    write(fd, "falling", 8);
    close(fd);

    return 0;
}


uint8_t pifacedigital_wait_for_input(uint8_t hw_addr, int timeout)
{
    int epfd = epoll_create(1);
    int fd = open("/sys/class/gpio/gpio25/value", O_RDONLY | O_NONBLOCK);

    if(fd > 0) {
        struct epoll_event ev;
        struct epoll_event events;
        ev.events = EPOLLIN | EPOLLET;
        ev.data.fd = fd;

        epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);

        // Flush any pending interrupts prior to wait
        pifacedigital_read_reg(0x11, hw_addr);

        // Ignore GPIO Initial Event
        epoll_wait(epfd, &events, 1, 10);

        // Wait for user event
        epoll_wait(epfd, &events, 1, timeout); 

        close(fd);
    }
    return pifacedigital_read_reg(0x11, hw_addr);
}
