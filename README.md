This is a collection of programs I have been writing to run on
the Orange Pi PC 2 board with the Allwinner h5 chip.

The H5 has a 4 core A53 (64 bit armv8) inside.

Amazingly the peripherals wrapped around the CPU cluster in the h5
are essentially identical to those in the h3 chip.

They are what you might call "bare-metal"
programming, although they get loaded by U-Boot.

For lots of notes and supporting information, see my website:

http://cholla.mmto.org/orange_pi/

If you want to follow my work in order, take them like this.

1. hello_asm - hello world written entirely in assembly.
2. hello - hello world written in C
3. no_atf - U-Boot without ATF and secure mode
4. inter - try to generate interrupts

My work with the h5 has been frustrating.
I have been unable to get interrupts to work.
This probably has to do with setting up clocks, but I suppose
there is some chance that the ATF secure/insecure mode stuff plays a part.
