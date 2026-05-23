#define REG(addr) (*(volatile char *) (addr))
#define BIT(idx) (1 << idx)

// UHWCON - Pad regulator
#define UHWCON  REG(0xD7)
#define UVREGE  BIT(0) // 0:RW - Enable USB pad regulator

// USBCON - Controller
#define USBCON  REG(0xD8)
#define USBE    BIT(7) // 0:RW - Enable USB controller. Clear to disable/reset USB controller
#define FRZCLK  BIT(5) // 1:RW - Disable clock
#define OTGPADE BIT(4) // 0:RW - Enable VBUS pad. Can be set even if USBE is 0
#define VBUSTE  BIT(0) // 0:RW - Enable VBUS transition interrupt

// USBSTA - Status
#define USBSTA  REG(0xD9)
#define ID      BIT(1) // 1:R  - Always 1. Preserved for compatibility
#define VBUS    BIT(0) // 0:R  - Status of USB VBUS connection

// USBINT - IVBUS Transition interrupt
#define USBINT  REG(0xDA)
#define VBUSTI  BIT(0) // 0:RW - Set by hardware when transition occurs. Cleared by software

// UDCON
#define UDCON   REG(0xE0)
#define RMWKUP  BIT(1) // 1:R  - Set by firmware to reset CPU after End of Reset signal
#define LSM     BIT(2) // 0:RW - Full-speed USB 2.0 mode. Set to use low-speed USB mode
#define RSTCPU  BIT(3) // 0:RW - Send wakeup/resume signal to device. Cleared when complete
#define DETACH  BIT(0) // 1:RW - Physically detach device. Unset to attach

// Power Controller and Status
#define PLLCSR  REG(0x49) // Write addr
#define PINDIV  BIT(4) // 0:RW - Unset for 8MHz ref clock, set for 16MHz
#define PLLE    BIT(1) // 0:RW - Enable PLL
#define PLOCK   BIT(0) // 0:R  - Lock clock. Set by hardware to indicate clock is configured


// USB Endpoint selector
#define UENUM REG(0xE9) // Accepts 0-6 (first 3 bits) as the endpoint number. 0b111 is forbidden
#define UEP0  0 // Endpoint 0 (0b000)
#define UEP1  1 // Endpoint 1 (0b001)
#define UEP2  2 // Endpoint 2 (0b010)
#define UEP3  3 // Endpoint 3 (0b011)
#define UEP4  4 // Endpoint 4 (0b100)
#define UEP5  5 // Endpoint 5 (0b101)
#define UEP6  6 // Endpoint 6 (0b110)

// USB Endpoint reset
#define UERST REG(0xEA)
#define EPRST0 BIT(0) // 0:RW - Endpoint 0
#define EPRST1 BIT(1) // 0:RW - Endpoint 1
#define EPRST2 BIT(2) // 0:RW - Endpoint 2
#define EPRST3 BIT(3) // 0:RW - Endpoint 3
#define EPRST4 BIT(4) // 0:RW - Endpoint 4
#define EPRST5 BIT(5) // 0:RW - Endpoint 5
#define EPRST6 BIT(6) // 0:RW - Endpoint 6

// USB Endpoint connection
#define UECONX   REG(0xEB)
#define STALLRQ  BIT(5) // 0:W  - STALL request handshake
#define STALLRQC BIT(4) // 0:W  - STALL request clear handshake
#define RSTDT    BIT(3) // 0:W  - Reset data toggle bit
#define EPEN     BIT(0) // 0:RW - Endpoint enable

// USB Endpoint configuration
#define UECFG0X  REG(0xEC)
// EPTYPE specified by bits 7:6
#define EPTYPE(val) ((val) << 6)
#define EP_CTL   EPTYPE(0) // Control
#define EP_ISO   EPTYPE(1) // Isochronus
#define EP_BLK   EPTYPE(2) // Bulk
#define EP_INT   EPTYPE(3) // Interrupt
#define EPDIR    BIT(0) // 0:RW - Endpoint direction (set for IN, clear for OUT)

// USB Endpoint configuration
#define UECFG1X  REG(0xED)
#define ALLOC    BIT(1) // 0:RW - Allocate memory
// EPSIZE specified by bits 6:4
#define EPSIZE(val) ((val) << 4)
#define EP_8B    EPSIZE(0)
#define EP_16B   EPSIZE(1)
#define EP_32B   EPSIZE(2)
#define EP_64B   EPSIZE(3)
#define EP_128B  EPSIZE(4)
#define EP_256B  EPSIZE(5)
#define EP_512B  EPSIZE(6)
// EPBK specified by bits 3:2
#define EPBK(val) ((val) << 2)
#define EP_OBK   EPBK(0) // One bank
#define EP_DBK   EPBK(1) // Double bank

// USB Endpoint status
#define UESTA0X  REG(0xEE)
#define CFGOK    BIT(7)
#define OVERFI   BIT(6)
#define UNDERFI  BIT(5)
//#define DTSEQ
//#define NBUSYBK

#define UEINTX   REG(0xE8)
#define FIFOCON  BIT(7)
#define NAKINI   BIT(6)
#define RWAL     BIT(5)
#define NAKOUTI  BIT(4)
#define RXSTPI   BIT(3)
#define RXOUTI   BIT(2)
#define STALLEDI BIT(1)
#define TXINI    BIT(0)

// AVR Status Register
#define SREG     REG(0x5F) // Write addr
#define I        BIT(7) // 0:RW - Global interrupt enable. Must be set for interrupts to work
#define T        BIT(6) // 0:RW - Bit copy storage
#define H        BIT(5) // 0:RW - Half carry flag
#define S        BIT(4) // 0:RW - Sign bit
#define V        BIT(3) // 0:RW - Two's Complement overflow
#define N        BIT(2) // 0:RW - Negative flag
#define Z        BIT(1) // 0:RW - Zero flag
#define C        BIT(0) // 0:RW - Carry flag

#define PIND    REG(0x29)
#define DDRD    REG(0x2A)
#define PORTD   REG(0x2B)
#define PD0     BIT(0)
#define PD1     BIT(1)
#define PD5     BIT(5)

void ledOn() {
    DDRD |= PD0;
    PORTD |= PD0;
}

void ledOff() {
    DDRD |= PD0;
    PORTD &= ~PD0;
}

void error() {
    // Output PD1 (DDxn 1, PORTxn 1)
    // Input PD0 (DDxn 0, PORTxn 1)
    DDRD |= PD5;
    PORTD &= ~PD5;
}

int main() {
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

    // TODO: Register and configure endpoints
    // Endpoint 0 for protocol configuration/host requests
    // Leave 0 selected
    UENUM = UEP0;
    UECONX |= EPEN;
    // Endpoint size 64kb for Full-Speed (USB 2.0)
    UECFG0X = 0;
    UECFG1X = EP_64B | EP_OBK | ALLOC;
    // Endpoint 1 for device data (HID packets) to be sent

    if(!(UESTA0X & CFGOK)){
         error();
    }


    // Attach USB device
    UDCON &= ~DETACH;

    // TODO: Await setup completion

    // TODO: Double check this is correctly resetting this interrupt

    // TODO: Send byte over USB, read with usbmon from host

    // TODO: Initialise timers for using clocks

    // Check that code reaches here
    ledOn();
    while(1){}
}
