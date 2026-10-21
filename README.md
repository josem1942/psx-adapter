# psx-adapter

This is both a hardware and software project that allows you to connect a DualShock 1 or DualShock 2 (untested) controller to a PC. This works by reading the signals from the controller and converting them to USB HID commands, so in theory it should support any device that supports USB HID GamePads.

## Hardware

The main microcontroller used is the RP2040. The board is designed with heavy inspiration from the Raspberry Pi Pico, as such many of the parts are similar if not identical to the Pico. As for the the header, you can typically find some on AliExpress to order. The whole project is designed using KiCad, so as long as you have that installed, you should be able to open it.

## Firmware

The firmware is built with the pico-sdk and TinyUSB.

### Building and Flashing

This was built on macOS, but I'm pretty sure you can build it on Windows and Linux if you get the same dependencies.

Dependencies:
- CMake 3.13 or newer
- Make
- [Arm GNU Toolchain](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads)
- Python 3
- Git
- GCC/G++ or whatever compiler you use to build C/C++ 

Install the required dependencies using `brew`:

```
brew install --cask gcc-arm-embedded
brew install cmake ninja python git picotool
```

Clone the repository:

```
git clone --recurse-submodules=firmware/pico-sdk https://github.com/josem1942/psx-adapter.git
cd psx-adapter
git -C firmware/pico-sdk submodule update --init lib/tinyusb
```

Then finally, you can build the firmware:

```
mkdir firmware/build
cd firmware/build
cmake ..
make
```

Then, to flash, hold down `SW1` (the only button) on the board while plugging in the USB, then drag `firmware/build/psxc.uf2` to the drive that appears when you plugged in the board.