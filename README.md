# arduino-as3933
Arduino interface to the AS3933 3D LF wakeup receiver

**Warning:** The AS3933 is a 3.3V component and is *NOT* 5V tolerant. You should use a 3.3V MCU or provide level shifters between the MCU and AS3933. While the "Absolute Maximum Ratings" state max VDD of 5V, this is an absolute max so even 5.1V may fry the chip (USB is spec'd at 4.75-5.25V).
