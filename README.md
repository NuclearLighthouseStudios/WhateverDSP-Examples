# WhateverDSP Examples

This repository contains a whole bunch of examples for different effects and synthesizers built on the WhateverDSP library.  
Not all examples run on all boards, but most of them do.

## Included examples
* **AIRHORN** Clearly the best instrument. Plays back a sample when pressing a button
* **filter** A simple combination lowpass/highpass filter
* **midi** Generates random midi sequences from audio triggers or midi clock
* **null** Does nothing
* **reverb** A simple reverb effect for the FXDSP pedal built on the Freeverb C++ code
* **sawyer** A reverby saw synth pad kind of thing
* **sine** Just plays a 440hz sine wave
* **so-666** A port of the so-666 feedback drone synth

## Getting started

* Initialize `WhateverDSP` submodule via `git submodule update --init`
* Go into your example directory of choice
* Change the `BOARD =` to the board you're using, either `wdsp-dev` or `fxdsp`
* Run `make`
* Flash the compiled binary to your board using either DFU or a debugger
* ???
* PROFIT

For more info read the README in the [main project repository](https://github.com/NuclearLighthouseStudios/WhateverDSP).

## License

All examples are published under the Unlicense. See `UNLICENSE` for more information.

The original Freeverb algorithm and code were developed by Jezar at Dreampoint and are in the public domain.