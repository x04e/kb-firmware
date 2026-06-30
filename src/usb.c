#include <stdint.h>
#include <avr/pgmspace.h>
#include "io.h"
#include "usb.h"

typedef struct DeviceDescriptor {
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint16_t bcdUSB;
    uint8_t  bDeviceClass;
    uint8_t  bDeviceSubclass;
    uint8_t  bDeviceProtocol;
    uint8_t  bMaxPacketSize;
    uint16_t idVendor;
    uint16_t idProduct;
    uint16_t bcdDevice;
    uint8_t  iManufacturer;
    uint8_t  iProduct;
    uint8_t  iSerialNumber;
    uint8_t  bNumConfigurations;
} DeviceDescriptor;

typedef struct SetupPacket {
    uint8_t bmRequestType;
    uint8_t bRequest;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
} SetupPacket;

void usbDisconnect();
void usbInit();
void usbHandleSetupPacket();
SetupPacket readSetupPacket();
void sendDeviceDescriptor(SetupPacket sp);
void setAddress(SetupPacket sp);
void usbHandleEndOfReset();
void configureEndpoints();
void sendPacket();

/* Disconnect the USB controller from the host */
void usbDisconnect() {
    UDCON |= DETACH;
}

// Initialise the USB controller
void usbInit() {
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

void usbHandleSetupPacket() {
    // Setup packet
    if (!(UEINTX & RXSTPI)) {
        return;
    }

    SetupPacket sp = readSetupPacket();
    // TODO: Refactor with structs for each packet type and helper functions for responses
    switch (sp.bRequest) {
        case 0x06: // GET_DESCRIPTOR
            sendDeviceDescriptor(sp);
            break;
        case 0x05: // SET_ADDRESS
            setAddress(sp);
            break;
            // TODO: Handle GET_DESCRIPTOR - DEVICE_QUALIFIER
            // TODO: Handle GET_DESCRIPTOR - CONFIGURATION
        default:
            error2();
            break;
    }
}

void usbHandleEndOfReset() {
    // End of Reset
    if (!(UDINT & EORSTI)) {
        return;
    }

    // Clear End of Reset
    UDINT &= ~EORSTI;
    configureEndpoints();
}

void configureEndpoints() {
    UENUM = UEP0;
    UECONX |= EPEN;
    // Endpoint size 64kb for Full-Speed (USB 2.0)
    UECFG0X = 0;
    // Allocate 64 Bytes with 1 bank. If multiple banks are used, see 22.13.1 for
    // how to properly clear and read FIFOCON/RXOUTI multiple times.
    UECFG1X = EP_64B | EP_OBK | ALLOC;
    // Ensure endpoint configuration is correct
    if (!(UESTA0X & CFGOK)) {
        error();
    }
}

// TODO: Replace important fields with definitions in a config.h
static const DeviceDescriptor DEVICE_DESCRIPTOR PROGMEM = {
    .bLength = 18,
    .bDescriptorType = 1,
    .bcdUSB = 0x200,
    .bDeviceClass = 0,
    .bDeviceSubclass = 0,
    .bDeviceProtocol = 0,
    .bMaxPacketSize = 64,
    .idVendor = 0xFEED,
    .idProduct = 0x004E,
    .bcdDevice = 0,
    .iManufacturer = 1,
    .iProduct = 2,
    .iSerialNumber = 3,
    .bNumConfigurations = 1
};

void setAddress(SetupPacket sp) {
    /* Copy address */
    UDADDR = (sp.wValue & ~ADDEN);
    /* ACK with zero-length packet */
    UEINTX &= ~(TXINI | FIFOCON);
    /* Wait for IN ZLP */
    while (!(UEINTX & TXINI));
    /* Set address */
    UDADDR |= ADDEN;
}

void sendPacket() {
    /* Send data */
    UEINTX &= ~(TXINI | FIFOCON);
    /* Wait for OUT ZLP and handshake it */
    while (!(UEINTX & RXOUTI));
    UEINTX &= ~(RXOUTI | FIFOCON);
}

/* Individual USB descriptor fields are send in reverse-byte order with
 * least-significant byte sent first. For however large the field is, we
 * take each byte one-at-a-time in reverse order and write it to UEDATX. */
#define writeField(b) \
    for(uint8_t i = 0; i < sizeof(b); i++){ \
        /* The "& 0xFF" is a bitmask of a single byte to ensure we only */ \
         /* get those bits. Unecessary for an 8-bit register but good */ \
        /* to do in general just in case we're writing to larger registers */ \
         /* or variables */ \
        UEDATX = (b >> (8 * i)) & 0xFF; \
    }

uint8_t readUint8t() {
    return (uint8_t)UEDATX;
}

uint16_t readUint16t() {
    uint16_t value = 0;
    value |= (uint16_t)UEDATX;
    value |= (uint16_t)UEDATX << 8;
    return value;
}

uint32_t readUint32t() {
    uint32_t value = 0;
    value |= (uint32_t)UEDATX;
    value |= (uint32_t)UEDATX << 8;
    value |= (uint32_t)UEDATX << 16;
    value |= (uint32_t)UEDATX << 24;
    return value;
}

SetupPacket readSetupPacket() {
    SetupPacket sp = {
        .bmRequestType = readUint8t(),
        .bRequest = readUint8t(),
        .wValue = readUint16t(),
        .wIndex = readUint16t(),
        .wLength = readUint16t()
    };

    // Clear RXSTPI and endpoint bank
    UEINTX &= ~(RXSTPI);
    return sp;
}

void sendDeviceDescriptor(SetupPacket sp) {
    //TODO: Handle sub-requests
    writeField(DEVICE_DESCRIPTOR.bLength);
    writeField(DEVICE_DESCRIPTOR.bDescriptorType);
    writeField(DEVICE_DESCRIPTOR.bcdUSB);
    writeField(DEVICE_DESCRIPTOR.bDeviceClass);
    writeField(DEVICE_DESCRIPTOR.bDeviceSubclass);
    writeField(DEVICE_DESCRIPTOR.bDeviceProtocol);
    writeField(DEVICE_DESCRIPTOR.bMaxPacketSize);
    writeField(DEVICE_DESCRIPTOR.idVendor);
    writeField(DEVICE_DESCRIPTOR.idProduct);
    writeField(DEVICE_DESCRIPTOR.bcdDevice);
    writeField(DEVICE_DESCRIPTOR.iManufacturer);
    writeField(DEVICE_DESCRIPTOR.iProduct);
    writeField(DEVICE_DESCRIPTOR.iSerialNumber);
    writeField(DEVICE_DESCRIPTOR.bNumConfigurations);

    sendPacket();
}
