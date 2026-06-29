#include <stdint.h>
#include <avr/pgmspace.h>
#include "io.h"

typedef struct {
    uint8_t bmRequestType;
    uint8_t bRequest;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
} SetupPacket;

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

void usbSetAddress(SetupPacket sp) {
    /* Copy address */
    UDADDR = (sp.wValue & ~ADDEN);
    /* ACK with zero-length packet */
    UEINTX &= ~(TXINI | FIFOCON);
    /* Wait for IN ZLP */
    while (!(UEINTX & TXINI));
    /* Set address */
    UDADDR |= ADDEN;
}

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

uint8_t usbReadUint8t() {
    return (uint8_t)UEDATX;
}

uint16_t usbReadUint16t() {
    uint16_t value = 0;
    value |= (uint16_t)UEDATX;
    value |= (uint16_t)UEDATX << 8;
    return value;
}

uint32_t usbReadUint32t() {
    uint32_t value = 0;
    value |= (uint32_t)UEDATX;
    value |= (uint32_t)UEDATX << 8;
    value |= (uint32_t)UEDATX << 16;
    value |= (uint32_t)UEDATX << 24;
    return value;
}

SetupPacket usbReadSetupPacket() {
    SetupPacket sp = {
        .bmRequestType = usbReadUint8t(),
        .bRequest = usbReadUint8t(),
        .wValue = usbReadUint16t(),
        .wIndex = usbReadUint16t(),
        .wLength = usbReadUint16t()
    };

    // Clear RXSTPI and endpoint bank
    UEINTX &= ~(RXSTPI);
    return sp;
}

void usbSendDeviceDescriptor(SetupPacket sp) {
    //TODO: Handle sub-requests
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
