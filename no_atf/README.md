
The "no-atf" project

This is really a silly and pointless exercise, but I didn't know that back in 2018 when I
was first working with 64 bit ARM and didn't really understand what ATF was all about.

This little project had two goals.  One is to examine in detail the h5 boot process.
The other is to eliminate the ATF part of that process (the "ARM trusted firmware" component).
The ATF component (otherwise known as bl31.bin) switches from EL3 to EL2, and it also places the
processor in NS (non-secure) mode, which is a sort of "sanboxed" mode.

At the time I wrote this, I didn't understand what secure mode was all about
(and I still don't) and thought that hving to run non-secure was limiting me.
I now know this is not true, but I had fun and learned some things (maybe)
monkeying with all of this.

My aim here is to start with various binary files and reproduce the packaging and place the
result on a bootable SD card which can be placed into an Orange Pi PC2 board (with H5 chip)
and end up running U-Boot in secure mode at EL2.

To build bl31.bin (not part of this project) --

1. cd arm-trusted-firmware
1. make realclean
1. make PLAT=sun50iw1p1 DEBUG=1 bl31
1. or perhaps: make V=1 PLAT=sun50iw1p1 DEBUG=1 bl31
1. yields: build/sun50iw1p1/debug/bl31/bl31.elf

Using "make sdcard" will yield an SD card that will run U-boot with the U-boot environment
set up to do DHCP and TFTP booting of an image called "h5.bin"
