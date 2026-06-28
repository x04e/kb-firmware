#include <stdint.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include "io.h"
#include "usb.h"

void resetBootloaderMode();
void error();
void errorOff();
void error2();
void error2Off();
void ledOn();
void ledOff();
void initUsb();
void configureUsbEndpoints();
void handleUsbEndOfReset();
void handleUsbSetupPacket();

int main() {
    resetBootloaderMode();
    initUsb();

    while(1){
        handleUsbEndOfReset();
        handleUsbSetupPacket();

        // Note: For control endpoints, RWAL and FIFOCON are always 0.
        // Waiting for these interrupts is pointless
        
        // Read packet from UEDATX and handle GET_DESCRIPTOR (Clear TXINI to send)
        // Note: USB fields are little-endian so the last byte must be written first!


        // TODO: Repeat for second reset/setup for subsequent GET_DESCRIPTOR requests
        
        // Code should reach here
    }
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
    // Allocate 64 Bytes with 1 bank. If multiple banks are used, see 22.13.1 for
    // how to properly clear and read FIFOCON/RXOUTI multiple times.
    UECFG1X = EP_64B | EP_OBK | ALLOC;
    // Ensure endpoint configuration is correct
    if(!(UESTA0X & CFGOK)){
        error();
    }
}

void handleUsbEndOfReset(){
    // End of Reset
    if(!(UDINT & EORSTI)) {
        return;
    }

    // Clear End of Reset
    UDINT &= ~EORSTI;
    configureUsbEndpoints();
}

void handleUsbSetupPacket(){
    // Setup packet
    if(!(UEINTX & RXSTPI)){
        return;
    }

    uint8_t x;
    for(uint8_t i = 0; i < 2; i++) {
        x = UEDATX;
    }

    // Clear RXSTPI and endpoint bank
    UEINTX &= ~(RXSTPI);

    // TODO: Refactor with structs for each packet type and helper functions for responses
    switch(x){
        case 0x06: // GET_DESCRIPTOR
            usbSendDeviceDescriptor();
            break;
        default:
            error();
            break;
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
    errorOff();
    error2Off();
}

void error() {
    // TXLED
    DDRD |= PD5;
    PORTD &= ~PD5;
}

void errorOff() {
    // TXLED
    DDRD |= PD5;
    PORTD |= PD5;
}

void error2() {
    // TXLED
    DDRB |= PB0;
    PORTB &= ~PB0;
}

void error2Off() {
    // TXLED
    DDRB |= PB0;
    PORTB |= PB0;
}

void ledOn() {
    DDRD |= PD0;
    PORTD |= PD0;
}

void ledOff() {
    DDRD |= PD0;
    PORTD &= ~PD0;
}
