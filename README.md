# Keyboard Firmware

This firmware is written for use with mechanical keyboards using the ATmega32U4 chip (Pro Micro). It is written in pure C with no libraries, for fun :)

### Useful Resources

- [TinyUSB - USB Protocol Concepts](https://docs.tinyusb.org/en/latest/reference/usb_concepts.html)
- [Beyond Logic - USB In a Nutshell](https://www.beyondlogic.org/usbnutshell/usb4.shtml)
- [The Gistre Blog - The USB Keyboard Protocol](https://blog.gistre.epita.fr/posts/ivan.imbert-2024-09-09-the_usb_keyboard_protocol/)
- [Kernel Docs - USB Mon](https://docs.kernel.org/usb/usbmon.html)
- [Kernel Docs - Introduction to HID Report Descriptors](https://docs.kernel.org/hid/hidintro.html)

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

```bash
$ make
```

This will compile the firmware into a hex file, `build/firmware.hex`.

### Watch

To watch the code for changes and compile it on-the-fly, run:

```bash
$ make watch
```

This command does not produce any hex files. It will recompile the code to `/dev/null` every 1 second if changes are detected to any files under `src/`.

### Flash

To compile the firmware and flash it to your microcontroller:

- Run

  ```bash
  $ make flash
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

## Debugging

### USB protocol (with usbmon)

When writing firmware and configuring the USB controller for the microcontroller, you will want to test the data being sent over the wire. Once the USB controller has been initialised, endpoints have been selected and you have written and sent your first byte over USB, you can view it from the host computer side with the `usbmon` kernel module.

The `usbmon` module is already provided in the Arch Linux kernel. To see if it is already enabled, run:

```bash
$ lsmod | grep usbmon
```

If you need to enable it, run:

```bash
$ sudo modprobe usbmon
```

With `usbmon` enabled, mount the debugfs:

```bash
$ sudo mount -t debugfs none_debugfs /sys/kernel/debug
```

List USB devices and note the Bus number and Device number:

```bash
$ lsusb
Bus 001 Device 001: ... Foo Device 1
Bus 001 Device 002: ... Bar Device 2
Bus 002 Device 001: ... Baz Device 3
```

If you don't know which bus your device will use, you can use bus 0 below (which shows all events) or you can plug a known-good device into the same port you wish to use and note its bus number. The device number will change each time a device is plugged in and out, so ignore this for now.

If your device is already sending HID packets (meaning you've implemented this functionality already) and shows up under `lsusb` on its own, then you can keep it plugged in and grep for its device ID from `usbmon` with:

```bash
$ sudo cat /sys/kernel/debug/usb/usbmon/0u | grep ':<id>'
```

The leading colon should keep the logs to just the ones for your device.

If you do not know whether your device is working or not and want to filter out all other devices, you can get the IDs of the other devices with `lsusb` as above and filter them out with grep:

```bash
$ sudo cat /sys/kernel/debug/usb/usbmon/0u | grep -v ':\(001\|002\|003\|004\)'
```

For details about the meaning of the usbmon output, you can review the [kernel usbmon documentation](https://docs.kernel.org/usb/usbmon.html).

You can now attempt to send bytes via the USB controller on your microcontroller and read them from your PC.

### HID protocol (with dmesg)

Once you can send data correctly over USB, the next step will be to create HID structs and send HID configuration data. You can still read this data with `usbmon`, but to see how that data is interpreted by your computer you can use `dmesg`.

Dmesg will only show basic HID information like connection/configuration and disconnection events, but can be useful for debugging these aspects of the keyboard. To tail dmesg and show these events, run:

```bash
$ sudo dmesg -W
```

Then plug in your keyboard. You should see the HID descriptor appear with the hardware/vendor/product IDs, etc., meaning your HID report and serialisation is working.
