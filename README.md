This is a collection of programs I have been writing to run on
the Orange Pi PC 2 board with the Allwinner h5 chip.

11-2018, 12-2025

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
3. inter - use timer interrupts to blink the status LED

The following was a interesting but unnecessary side track

1. no_atf - U-Boot without ATF and secure mode

I did not go too far with these demos, then moved work on
the H5 to my Kyu project
