# Keyboard Firmware

This firmware is written for use with mechanical keyboards using the ATmega32U4 chip (Pro Micro). It is written in pure C with no libraries, for fun :)

## Prerequisites

This project is written for Linux, with GNU Make and the /dev/ttyACM0 serial device in mind. You may need to tweak the Makefile to suit your OS. The avr-gcc toolchain commands are relatively straightforward, however.

### Install dependencies

Install the avr-gcc toolchain:

Arch Linux:

```bash
$ sudo pacman -S avr-gcc
```

## Building the Firmware

### Compile

To compile the project, run:

```
make
```

This will compile the firmware into a hex file, `build/firmware.hex`.

### Watch

To watch the code for changes and compile it on-the-fly, run:

```
make watch
```

This command does not produce any hex files. It will recompile the code to `/dev/null` every 1 second if changes are detected to any files under `src/`.

### Flash

To compile the firmware and flash it to your microcontroller:

- Run

  ```
  make flash
  ```

  This command will run `await-avr.sh` to wait for the microcontroller to become available for 20 seconds, after which it will error out.

- Set your microcontroller into bootloader mode by shorting the RST pin to GND. The bootloader should then appear as `/dev/ttyACM0`.

  For some models, you may need to short RST twice quickly. You can solder a switch to these pins to be able to double-tap reset quickly enough.

- If successful, you will see output like the below:

  ```
  $ make flash
  avr-gcc -mmcu=atmega32u4 -Wall -Os -o build/firmware.elf src/main.c
  avr-objcopy -j .text -j .data -O ihex build/firmware.elf build/firmware.hex
  rm build/firmware.elf
  ./await-avr.sh
  Waiting for AVR to be writable........
  avrdude -P /dev/ttyACM0 -p atmega32u4 -c avr109 -U flash:w:build/firmware.hex
  Reading 206 bytes for flash from input file firmware.hex
  Writing 206 bytes to flash
  Writing | ################################################## | 100% 0.02 s
  Reading | ################################################## | 100% 0.00 s
  206 bytes of flash verified

  Avrdude done.  Thank you.
  ```
