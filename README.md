libpifacedigital
================
A simple library for controlling PiFace Digital. See `example.c` for example
usage. Hides the SPI file descriptors so the user only has to deal with
`hw_addr`.

Use
---
Build the library:

    $ make

This creates the library `libpifacedigital.a`. Include it in your project with:

    $ gcc -o example example.c -Isrc/ -L. -lpifacedigital -L../libmcp23s17/ -lmcp23s17

`-Isrc/` is for including the header file. `-L. -lpifacedigital` is for
including the library file (from this directory).
`-L../libmcp23s17/ -lmcp23s17` is for including the mcp23s17 library.

Todo
----
Feel free to contribute!

- mcp23s17_fd checking for each function. Print to stderr if not found.
- Documentation (Doxygen? Good comments?)
- Debian install
- Interrupts (using libmcp23s17 interrupts?)
