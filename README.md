# Keyboard Firmware

This firmware is written for use with mechanical keyboards using the ATmega32U4 chip (Pro Micro)

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
// NOTE: You will have to do this every time your machine is restarted unless you enable the usbmon module permenantly (not recommended)
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

#### Wireshark

Wireshark is capable of logging USB traffic in a GUI format. To set it up:

Install wireshark:

```bash
$ sudo pacman -S wireshark-qt
```

Create a `usbmon` group:

```bash
$ sudo groupadd usbmon
```

Create a `udev` rule for wireshark:

```bash
cat 'SUBSYSTEM=="usbmon", GROUP=="usbmon", MODE="640"' > sudo tee /etc/udev.d/rules/90-wireshark-usbmon.rules
```

Update `udev` with the changes:

```bash
$ sudo udevadm trigger --subsystem-match=usbmon
```

Finally, add your user to both the `usbmon` and `wireshark` groups:

```bash
$ sudo usermod -aG usermon <your-username>
$ sudo usermod -aG wireshark <your-username>
```

Reboot your machine. Remember to enable the `usbmon` module. Then open wireshark and you should see the usbmon interfaces in Wireshark.

##### Using Wireshark

In Wireshark:

- Connect to the `usbmon0` interface
- Disable the `USBHUB` protocol to hide packets from the USB hubs themselves (Analyze > Enabled Protocols > Uncheck USBHUB)
- Filter out devices you don't care about with a filter like:

  ```
  not (usb.src in { 1.2.3, 1.2.4, 1.2.5 } or usb.addr in { 1.2.3, 1.2.4, 1.2.5 })
  ```

  where the addresses are the values in the `Source` and `Address` columns

  **Note:** If you want to see new devices being plugged in, you should not filter out your USB hubs. Requests will come in on their .0 endpoint each time a devices is connected, which might be useful.

  **Note:** If you have other USB devices like keyboards, mice, or external hard drives plugged into the same USB Bus that your microcontroller will be plugged into, you may have to keep filtering out new addresses from those devices as they enter an idle state and then resume, aquiring a new address for themselves.

- Plug in your microcontroller. `usbmon`/ the kernel cannot see the deivce address negotiation process, but you should see some initial requests to the 0 address of your USB hub (`lsusb` will show a matching device ID) and then requests coming from a new device ID. This is your controller. These addresses are negotiated when a device is plugged in, so they will change every time. This is why we filter out known addresses.

### HID protocol (with dmesg)

Once you can send data correctly over USB, the next step will be to create HID structs and send HID configuration data. You can still read this data with `usbmon`, but to see how that data is interpreted by your computer you can use `dmesg`.

Dmesg will only show basic HID information like connection/configuration and disconnection events, but can be useful for debugging these aspects of the keyboard. To tail dmesg and show these events, run:

```bash
$ sudo dmesg -W
```

Then plug in your keyboard. You should see the HID descriptor appear with the hardware/vendor/product IDs, etc., meaning your HID report and serialisation is working.
