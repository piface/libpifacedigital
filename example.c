/* compile with:
 * gcc -o example example.c -Isrc/ -L. -lpifacedigital -L../libmcp23s17/ -lmcp23s17
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "pifacedigital.h"


int main(int argc, char * argv[])
{
    // first argument changes hw_addr, defaults to 0
    int hw_addr = 0;
    if (argc > 1) {
        hw_addr = atoi(argv[1]);
    }

    pifacedigital_open(hw_addr);

    // Write to output
    pifacedigital_write_reg(0x00, OUTPUT, hw_addr);
    sleep(1);
    pifacedigital_write_reg(0xaa, OUTPUT, hw_addr);
    sleep(1);
    pifacedigital_write_reg(0x55, OUTPUT, hw_addr);
    sleep(1);
    pifacedigital_write_reg(0x00, OUTPUT, hw_addr);

    // set bit 0 on the output port
    pifacedigital_write_bit(1, 0, OUTPUT, hw_addr);
    sleep(1);
    pifacedigital_write_bit(0, 0, OUTPUT, hw_addr);

    // set the input pullups (must #include "mcp23s17.h")
    //pifacedigital_write_reg(0xff, GPPUB, hw_addr);

    // print the input state
    uint8_t input = pifacedigital_read_reg(INPUT, hw_addr);
    printf("Inputs: 0x%x\n", input);

    pifacedigital_close(hw_addr);
}
