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

void usbWriteUint8t(uint8_t b){
    UEDATX = b;
}

void usbWriteUint16t(uint16_t b){
    UEDATX = (b >> 0) & 0xFF;
    UEDATX = (b >> 8) & 0xFF;
}

void usbWriteUint32t(uint32_t b){
    usbWriteUint16t(b);
    usbWriteUint16t((b << 16) & 0xFF);
}

void usbWriteUint64t(uint64_t b){
    usbWriteUint32t(b);
    usbWriteUint32t((b << 16) & 0xFF);
}

void usbSendDeviceDescriptor(){
    usbWriteUint8t(DEVICE_DESCRIPTOR.bLength);
    usbWriteUint8t(DEVICE_DESCRIPTOR.bDescriptorType);
    usbWriteUint16t(DEVICE_DESCRIPTOR.bcdUSB);
    usbWriteUint8t(DEVICE_DESCRIPTOR.bDeviceClass);
    usbWriteUint8t(DEVICE_DESCRIPTOR.bDeviceSubclass);
    usbWriteUint8t(DEVICE_DESCRIPTOR.bDeviceProtocol);
    usbWriteUint8t(DEVICE_DESCRIPTOR.bMaxPacketSize);
    usbWriteUint16t(DEVICE_DESCRIPTOR.idVendor);
    usbWriteUint16t(DEVICE_DESCRIPTOR.idProduct);
    usbWriteUint16t(DEVICE_DESCRIPTOR.bcdDevice);
    usbWriteUint8t(DEVICE_DESCRIPTOR.iManufacturer);
    usbWriteUint8t(DEVICE_DESCRIPTOR.iProduct);
    usbWriteUint8t(DEVICE_DESCRIPTOR.iSerialNumber);
    usbWriteUint8t(DEVICE_DESCRIPTOR.bNumConfigurations);
    // Clear TXINI to send
    UEINTX &= ~TXINI;
}