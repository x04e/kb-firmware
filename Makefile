arch = atmega32u4
prog = avr109

default:
	avr-gcc -mmcu=$(arch) -Wall -Os -o build/firmware.elf src/main.c
	avr-objcopy -j .text -j .data -O ihex build/firmware.elf build/firmware.hex
	rm build/firmware.elf

flash: default
	./await-avr.sh
	avrdude -P /dev/ttyACM0 -p $(arch) -c $(prog) -U flash:w:build/firmware.hex

watch:
	./watch.sh \
		--watch-cmd 'make' \
		--flash-cmd 'make flash'
