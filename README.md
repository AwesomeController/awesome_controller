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
4. Create a raw dongle for the console that you want to use. A raw dongle is the console end of a controller cord.
You'll need to cut open the cord and expose the wires so that they can be connected directly to the arduino.
You could use a controller you already own, but there are other options if this is too great a sacrifice.
You could buy a cheap used or 3rd party controller from your local video game store.
There are also controller extension cords that can be purchased online or in stores. Here is an example: http://www.amazon.com/Extension-Cable-BRAND-Original-Nintendo/dp/B000NO4X10
5. Install the Arduino environment/IDE: http://arduino.cc/en/Main/Software
6. Open the awesome_controller.ino project file in the Arduino IDE
7. DONGLE WIRING
8. Load and run the code on your Arduino!
9. The software should detect which game console it is connected to automatically, and you can hopefully begin playing!

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