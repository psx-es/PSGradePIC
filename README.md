PSGradePIC
==========

PSGradePIC is an open-source reimplementation of the psdowngrade exploit for PIC18F microcontrollers.

It is known to work on:

- PIC18F14K50 (20 pins / 16KB Flash)
- PIC18F2450 (28 pins / 16KB Flash)
- PIC18F2455 (28 pins / 24KB Flash)
- PIC18F2550 (28 pins / 32KB Flash)
- PIC18F2553 (28 pins / 32KB Flash)
- PIC18F27J53 (28 pins / 128KB Flash)
- PIC18F4450 (40 pins / 16KB Flash)
- PIC18F4455 (40 pins / 24KB Flash)
- PIC18F4550 (40 pins / 32KB Flash)
- PIC18F4553 (40 pins / 32KB Flash)
- PIC18F47J53 (40 pins / 128KB Flash)
- PIC18F67J50 (64 pins / 128KB Flash)

**This software is not intended to enable piracy, and such features
have been disabled.  This software is intended to allow the execution
of unsigned third-party apps and games on the PS3.**

No one involved in maintaining the psgroopic git is responsible for or has any involvement with any existing usb dongles sporting "psgrade" / "psgradepic" in its name. Thank you for your understanding.


Cloning
-------
To clone, use something like:

    git clone git://github.com/Noltari/PSGradePIC.git
    cd PSGradePIC


Building
--------

    make clean
    make


Using
-----
To use this exploit:
  
* Hard power cycle your PS3 (using the switch in back, or unplug it)
* Plug the dongle into your PS3.
* Press the PS3 power button, followed quickly by the eject button.

After a few seconds, the first LED on your dongle should light up.
After about 5 seconds, the second LED will light up (or the LED will
just go off, if you only have one).  This means the exploit worked!


Notes
-----
A programmed dongle won't enumerate properly on a PC, so don't worry
about that.


Credits
-------
/* Credits to be done :D. */

And thanks to all the community around this work.