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
#define USBINT  REG(0xE1)
#define VBUSTI  BIT(0) // 0:RW - Set by hardware when transition occurs. Cleared by software

// UDCON
#define UDCON   REG(0xE0)
#define RMWKUP  BIT(1) // 1:R  - Set by firmware to reset CPU after End of Reset signal
#define LSM     BIT(2) // 0:RW - Full-speed USB 2.0 mode. Set to use low-speed USB mode
#define RSTCPU  BIT(3) // 0:RW - Send wakeup/resume signal to device. Cleared when complete
#define DETACH  BIT(0) // 1:RW - Physically detach device. Unset to attach

// Power Controller and Status
#define PLLCSR  REG(0x29)
#define PINDIV  BIT(4) // 0:RW - Unset for 8MHz ref clock, set for 16MHz
#define PLLE    BIT(1) // 0:RW - Enable PLL
#define PLOCK   BIT(0) // 0:R  - Lock clock. Set by hardware to indicate clock is configured


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
    // Attach USB device
    UDCON &= ~DETACH;

    return 0;
}
