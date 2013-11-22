libpifacedigital
================
A simple library for controlling PiFace Digital. See `example.c` for example
usage. Hides the SPI file descriptors so the user only has to deal with
`hw_addr`.

Documentation
-------------

[http://piface.github.io/piface/libpifacedigital](http://piface.github.io/piface/libpifacedigital)

Build the docs with:

    $ git clone https://github.com/piface/libpifacedigital.git
    $ cd libpifacedigital/docs/
    $ doxygen pifacedigital-doc.conf

To view as HTML, point your browser to `libpifacedigital/docs/html/index.html`.

To view as PDF:

    $ cd latex/
    $ make

The pdf is called `refman.pdf`.
