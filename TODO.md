Features
--------

* Refactor into system objects (NES, SNES, etc.)
* Refactor WiiRemote library into USB+BluetoothManager and actual
  WiiRemote class.
  * API for WiiRemote with USB+Bluetooth
* Fixed-address functionality on Bluesmirf (1 Wiimote)
* Discovery functionality on Bluesmirf
* Accept multiple controllers via Bluetooth
* Get Bluetooth working with PS3 controller
* N64 spike to figure out timings
* Limit BT discovery to PS3 and Wii controllers
* Create Eagle schematic
* Create Gerber schematic
* Power down controllers when no clock/data line seen in awhile

Bugs
----

* Unplugging Arduino from console during gameplay freezes NES
* Unplugging Arduino from power during gameplay freezes NES
* Uploading sketch to Arduino during gameplay freezes NES
