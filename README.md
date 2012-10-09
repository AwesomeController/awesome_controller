Awesome Controller
==================

This is the official repository for the Awesome Controller firmware, which runs on the
Arduino platform.

The various branches in this repository represent different steps in the evolution of
the software.

Running the code
========
1. Get yourself an Arduino. http://arduino.cc/en/Main/Buy
2. Get a USB host shield and attach it to your Ardiuno. https://www.sparkfun.com/products/9947?
3. Acquire a cheap bluetooth dongle, such as http://www.amazon.com/Bluetooth-USB-Micro-Adapter-Dongle/dp/B001EBE1LI
4. Install the Arduino environment/IDE: http://arduino.cc/en/Main/Software
5. Open the awesome_controller.ino project file in the Arduino IDE
6. DONGLE WIRING
7. Load and run the code on your Arduino!
8. The software should detect which game console it is connected to automatically, and you can hopefully begin playing!

Hardware Schematics
========

In the "eagle" directory you will find files for use with EAGLE, a software suite for printed circuit board design.
These are unfinished, but will be used if we have our own circuit boards printed.

Running Tests
=============
The following instructions work for running the high-level
tests on a Macbook Pro:

* Dependencies
  * Requires Ruby 1.9+
  * brew install cmake (linking failed for me, so I had to `brew link cmake` afterwards)
  * gem install ffi