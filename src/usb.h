
/* Disconnect the USB controller from the host */
void usbDisconnect();

/* Initialise the USB controller. Must be called before any USB events can be handled */
void usbInit();

/* Handle USB SETUP packet */
void usbHandleSetupPacket();

/* Handle USB End of Reset events */
void usbHandleEndOfReset();
