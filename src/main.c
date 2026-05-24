#include <stdint.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include "io.h"

void resetBootloaderMode();
void error();
void ledOn();
void ledOff();
void initUsb();
void configureUsbEndpoints();

int main() {
    resetBootloaderMode();

    initUsb();

    // Await End of Reset
    while(!(UDINT & EORSTI));
    // Clear End of Reset
    UDINT &= ~EORSTI;

    configureUsbEndpoints();

    // Initial Setup packet
    // Await Setup packet
    while(!(UEINTX & RXSTPI));
    // Clear RXSTPI
    UEINTX &= ~RXSTPI;

    // Note: For control endpoints, RWAL and FIFOCON are always 0.
    // Waiting for these interrupts is pointless

    // Read packet from UEDATX and handle GET_DESCRIPTOR (Clear TXINI to send)
    // Note: USB fields are little-endian so the last byte must be written first!
    UEDATX = 18; // 1b - bLength
    UEDATX = 1; // 1b - bDescriptorType
    UEDATX = 0x00; // 2b - bcdUSB
    UEDATX = 0x20;
    UEDATX = 0; // bDeviceClass
    UEDATX = 0; // bDeviceSubclass
    UEDATX = 0; // bDeviceProtocol
    UEDATX = 64; // bMaxPacketSize
    // 2b - idVendor
    UEDATX = 0xED;
    UEDATX = 0xFE;
    // 2b - idProduct
    UEDATX = 0x4E;
    UEDATX = 0x00;
    // 2b - bcdDevice
    UEDATX = 0;
    UEDATX = 0;
    // 1b - iManufacturer
    UEDATX = 1;
    // 1b - iProduct
    UEDATX = 2;
    // 1b - iSerialNumber
    UEDATX = 3;
    // 1b - bNumConfigurations
    UEDATX = 1;

    // Clear TXINI to send
    UEINTX &= ~TXINI;


    // TODO: Repeat for second reset/setup for subsequent GET_DESCRIPTOR requests

    // Code should reach here
    ledOn();
    while(1){}
}

// Initialise the USB controller
void initUsb(){
    // Power-on pads regulator
    UHWCON |= UVREGE;
    // Configure PLL clock speed
    PLLCSR |= PINDIV;
    // Enable PLL
    PLLCSR |= PLLE;
    // Await PLL lock
    while (!(PLLCSR & PLOCK));
    // Enable USB
    USBCON |= USBE;
    // Unfreeze USB clock
    USBCON &= ~FRZCLK;
    // Connect USB to VBUS
    USBCON |= OTGPADE;
    // Wait for USB VBUS connection
    while (!(USBSTA & VBUS));
    // Attach USB device
    UDCON &= ~DETACH;
}

void configureUsbEndpoints(){
    UENUM = UEP0;
    UECONX |= EPEN;
    // Endpoint size 64kb for Full-Speed (USB 2.0)
    UECFG0X = 0;
    UECFG1X = EP_64B | EP_OBK | ALLOC;
    // Ensure endpoint configuration is correct
    if(!(UESTA0X & CFGOK)){
         error();
    }
}

void resetBootloaderMode(){
    /* /dev/ttyACM0 remains active after flashing. Ensure the USB controller
     * is disconnected and wait for a few ms before initialising it again
     * to ensure it disconnects and can be flashed again. */
    UDCON |= DETACH;

    // Burn a few cycles while we wait for the USB controller to disconnect
    _delay_ms(200);

    // Turn off TX/RX LEDs
    DDRD |= PD5;
    PORTD |= PD5;
    DDRB |= PB0;
    PORTB |= PB0;
}

void error() {
    // TXLED
    DDRD |= PD5;
    PORTD &= ~PD5;
}

void ledOn() {
    DDRD |= PD0;
    PORTD |= PD0;
}

void ledOff() {
    DDRD |= PD0;
    PORTD &= ~PD0;
}
