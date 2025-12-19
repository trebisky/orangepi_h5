Project "inter" for the Allwinner H5

The goal here is to generate and handle interrupts.
I began this back in 2018 and gave up.
I returned to it in 2025 after learning about the
need to set the IMO bit in the HCR register to
get interrupts running at EL2 (which this is).

This now works, and the demo is now blinking the red status
LED at about 1 Hz.

My notes from 2018 indicate that I am unable to manipulate
(i.e. blink) the POWER led.  This is on the oddball R_GPIO port
on bit 10.  I can blink it fine on my H3 boards, so why not here?
Perhaps I need to set up clocks on the bus that holds all the "R_"
peripherals.  I don't know, and this issue is unsolved.

Working 12-18-2025
