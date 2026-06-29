#include <stdint.h>
#include <avr/pgmspace.h>
#include "io.h"

typedef struct {
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

#define usbSetAddress() \
    /* Copy address */ \
    UDADDR = (UEDATX & ~ADDEN); \
    /* ACK with zero-length packet */ \
    UEINTX &= ~TXINI; \
    /* Wait for IN ZLP */ \
    while(!(UEINTX & TXINI)); \
    /* Set address */ \
    UDADDR |= ADDEN;

#define usbSendPacket() \
    /* Send data */ \
    UEINTX &= ~(TXINI | FIFOCON); \
    /* Wait for OUT ZLP and handshake it */ \
    while(!(UEINTX & RXOUTI)); \
    UEINTX &= ~(RXOUTI | FIFOCON);

/* Individual USB descriptor fields are send in reverse-byte order with
 * least-significant byte sent first. For however large the field is, we
 * take each byte one-at-a-time in reverse order and write it to UEDATX. */
#define usbWriteField(b) \
    for(uint8_t i = 0; i < sizeof(b); i++){ \
        /* The "& 0xFF" is a bitmask of a single byte to ensure we only */ \
         /* get those bits. Unecessary for an 8-bit register but good */ \
         /* to do in general just in case we're writing to larger registers */ \
         /* or variables */ \
        UEDATX = (b >> (8 * i)) & 0xFF; \
    }

void usbSendDeviceDescriptor() {
    usbWriteField(DEVICE_DESCRIPTOR.bLength);
    usbWriteField(DEVICE_DESCRIPTOR.bDescriptorType);
    usbWriteField(DEVICE_DESCRIPTOR.bcdUSB);
    usbWriteField(DEVICE_DESCRIPTOR.bDeviceClass);
    usbWriteField(DEVICE_DESCRIPTOR.bDeviceSubclass);
    usbWriteField(DEVICE_DESCRIPTOR.bDeviceProtocol);
    usbWriteField(DEVICE_DESCRIPTOR.bMaxPacketSize);
    usbWriteField(DEVICE_DESCRIPTOR.idVendor);
    usbWriteField(DEVICE_DESCRIPTOR.idProduct);
    usbWriteField(DEVICE_DESCRIPTOR.bcdDevice);
    usbWriteField(DEVICE_DESCRIPTOR.iManufacturer);
    usbWriteField(DEVICE_DESCRIPTOR.iProduct);
    usbWriteField(DEVICE_DESCRIPTOR.iSerialNumber);
    usbWriteField(DEVICE_DESCRIPTOR.bNumConfigurations);

    usbSendPacket();
}
