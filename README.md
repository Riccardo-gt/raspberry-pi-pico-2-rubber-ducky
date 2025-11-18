This is a copy and modification of the hid_composite example from TinyUSB (https://github.com/hathach/tinyusb/tree/master/examples/device/hid_composite)
showing how to build with TinyUSB when using the Raspberry Pi Pico SDK

This modification allows the user to install whatever from a storage device and set it up to run on startup through HID rubber ducky type methods

plug in a storage device (first) and the raspberry pi pico 2.
press the BOOTSEL button.
done.

the filepath and appname macros must be defined to suit your stuff.

I know there exists a whole ecosystem around USB Rubber Ducky devices with DuckyScript etc. This does not make use of that or intergrate it at all. It is a standalone fun and educational project on the pico 2