Project "inter" for the Allwinner H5

This was developed and tested on the
Orange Pi PC2 board.

The goal here is to generate and handle interrupts.
I began this back in 2018 and gave up.
I returned to it in 2025 after learning about the
need to set the IMO bit in the HCR register to
get interrupts running at EL2 (which this is).

This now works, and the demo is now blinking the red status
LED and athe green power LED at about 1 Hz.

They moved the status LED from A15 on the h3 boards
to A20 on the h5 boards.  This is clear on the schematic,
but it took me a while to sort it out.

Working 12-18-2025 4-22-2026
