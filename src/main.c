#include <stdint.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include "io.h"
#include "usb.h"

void resetBootloaderMode();

int main() {
    resetBootloaderMode();
    usbInit();

    while (1) {
        usbHandleEndOfReset();
        usbHandleSetupPacket();

        // Note: For control endpoints, RWAL and FIFOCON are always 0.
        // Waiting for these interrupts is pointless

        // Read packet from UEDATX and handle GET_DESCRIPTOR (Clear TXINI to send)
        // Note: USB fields are little-endian so the last byte must be written first!


        // TODO: Repeat for second reset/setup for subsequent GET_DESCRIPTOR requests

        // Code should reach here
    }
}

void resetBootloaderMode() {
    /* /dev/ttyACM0 remains active after flashing. Ensure the USB controller
     * is disconnected and wait for a few ms before initialising it again
     * to ensure it disconnects and can be flashed again. */
    usbDisconnect();

    // Burn a few cycles while we wait for the USB controller to disconnect
    _delay_ms(200);

    // Turn off TX/RX LEDs
    errorOff();
    error2Off();
}

