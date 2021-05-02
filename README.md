# FP-GAme
FP-GAme is a retro video game console bundled with a development kit. The
project was developed at Carnegie Mellon University for the 2021 ECE Capstone
Design course by our team of two: Andrew Spaulding and Joseph Yankel.

The project runs on the DE10-Nano SoC Board from Intel/Altera. This Soc Board contains an FPGA and
a 32-bit ARM CPU. The FPGA is programmed with the following functionality:
* A tile and sprite based graphics processor similar in capability to those found on the NES or
SNES. Outputs video to HDMI on the DE10-Nano.
* A sound card, which plays a stream of 32KHz, 8-bit PCM from the CPU. Outputs audio to HDMI on the
DE10-Nano.
* An SNES controller port, which reads inputs from the controller via the GPIO pins on the
DE10-Nano.

This repository contains all the files you need to get started with FP-GAme. We recommend the
fpgame_getting_started.pdf guide which gives a step-by-step process to downloading the SD Card Image
file, flashing your SD card, and starting the tech-demo.

Note, FP-GAme requires a bit of DIY handiwork to modify an SNES controller extension cable to be
compatible with GPIO headers. Instructions on how to accomplish this is included in
snes_controlled_mod_instructions.pdf under the docs subfolder.

When you have the tech-demo running and are ready to develop your own games, check out the
fpgame_developers_manual.pdf included in the docs subfolder. This is also a great resource to view
to find out more about the FP-GAme hardware capabilities and the inner-workings of the system.

If you are an advanced user, are looking to make changes to FP-GAme hardware or system software, or
are interesting in building the project from source, check out the FP-GAme source repository:
https://github.com/FP-GAme/fpgame-src