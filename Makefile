# File:   Makefile
# Author: Noah Irving (nji29), David Frost (djf99)
# Date:   15/10/2020
# Descr:  Makefile for game

# Definitions.
CC = avr-gcc
CFLAGS = -mmcu=atmega32u2 -Os -Wall -Wstrict-prototypes -Wextra -g -I. -I../../utils -I../../fonts -I../../drivers -I../../drivers/avr
OBJCOPY = avr-objcopy
SIZE = avr-size
DEL = rm
DRIVER_PATH = ../../drivers/
AVR_PATH = $(DRIVER_PATH)avr/
UTILS_PATH = ../../utils/


# Default target.
all: game.out


# Compile: create object files from C source files.
game.o: game.c paddle.h ball.h scoring.h communicate.h $(AVR_PATH)system.h $(DRIVER_PATH)display.h $(DRIVER_PATH)navswitch.h ../../fonts/font3x5_1.h $(UTILS_PATH)font.h $(UTILS_PATH)pacer.h $(UTILS_PATH)tinygl.h
	$(CC) -c $(CFLAGS) $< -o $@

scoring.o: scoring.c scoring.h $(AVR_PATH)system.h $(UTILS_PATH)tinygl.h $(UTILS_PATH)pacer.h
	$(CC) -c $(CFLAGS) $< -o $@

paddle.o: paddle.c paddle.h communicate.h $(AVR_PATH)system.h $(AVR_PATH)pio.h $(DRIVER_PATH)navswitch.h $(DRIVER_PATH)ledmat.h
	$(CC) -c $(CFLAGS) $< -o $@

ball.o: ball.c ball.h paddle.h scoring.h $(AVR_PATH)system.h $(AVR_PATH)timer.h $(DRIVER_PATH)ledmat.h $(UTILS_PATH)pacer.h
	$(CC) -c $(CFLAGS) $< -o $@

communicate.o: communicate.c communicate.h ball.h $(AVR_PATH)system.h $(AVR_PATH)ir_uart.h $(AVR_PATH)system.h $(DRIVER_PATH)navswitch.h
	$(CC) -c $(CFLAGS) $< -o $@

ir_uart.o: ../../drivers/avr/ir_uart.c ../../drivers/avr/ir_uart.h ../../drivers/avr/pio.h ../../drivers/avr/system.h ../../drivers/avr/timer0.h ../../drivers/avr/usart1.h
	$(CC) -c $(CFLAGS) $< -o $@

pio.o: ../../drivers/avr/pio.c ../../drivers/avr/pio.h ../../drivers/avr/system.h
	$(CC) -c $(CFLAGS) $< -o $@

prescale.o: ../../drivers/avr/prescale.c ../../drivers/avr/prescale.h ../../drivers/avr/system.h
	$(CC) -c $(CFLAGS) $< -o $@

system.o: ../../drivers/avr/system.c ../../drivers/avr/system.h
	$(CC) -c $(CFLAGS) $< -o $@

timer.o: ../../drivers/avr/timer.c ../../drivers/avr/system.h ../../drivers/avr/timer.h
	$(CC) -c $(CFLAGS) $< -o $@

timer0.o: ../../drivers/avr/timer0.c ../../drivers/avr/bits.h ../../drivers/avr/prescale.h ../../drivers/avr/system.h ../../drivers/avr/timer0.h
	$(CC) -c $(CFLAGS) $< -o $@

usart1.o: ../../drivers/avr/usart1.c ../../drivers/avr/system.h ../../drivers/avr/usart1.h
	$(CC) -c $(CFLAGS) $< -o $@

display.o: ../../drivers/display.c ../../drivers/avr/system.h ../../drivers/display.h ../../drivers/ledmat.h
	$(CC) -c $(CFLAGS) $< -o $@

ledmat.o: ../../drivers/ledmat.c ../../drivers/avr/pio.h ../../drivers/avr/system.h ../../drivers/ledmat.h
	$(CC) -c $(CFLAGS) $< -o $@

navswitch.o: ../../drivers/navswitch.c ../../drivers/avr/delay.h ../../drivers/avr/pio.h ../../drivers/avr/system.h ../../drivers/navswitch.h
	$(CC) -c $(CFLAGS) $< -o $@

font.o: ../../utils/font.c ../../drivers/avr/system.h ../../utils/font.h
	$(CC) -c $(CFLAGS) $< -o $@

pacer.o: ../../utils/pacer.c ../../drivers/avr/system.h ../../drivers/avr/timer.h ../../utils/pacer.h
	$(CC) -c $(CFLAGS) $< -o $@

tinygl.o: ../../utils/tinygl.c ../../drivers/avr/system.h ../../drivers/display.h ../../utils/font.h ../../utils/tinygl.h
	$(CC) -c $(CFLAGS) $< -o $@


# Link: create ELF output file from object files.
game.out: game.o scoring.o paddle.o ball.o scoring.o communicate.o ir_uart.o pio.o prescale.o system.o timer.o timer0.o usart1.o display.o ledmat.o navswitch.o font.o pacer.o tinygl.o ledmat.o
	$(CC) $(CFLAGS) $^ -o $@ -lm
	$(SIZE) $@


# Target: clean project.
.PHONY: clean
clean:
	-$(DEL) *.o *.out *.hex


# Target: program project.
.PHONY: program
program: game.out
	$(OBJCOPY) -O ihex game.out game.hex
	dfu-programmer atmega32u2 erase; dfu-programmer atmega32u2 flash game.hex; dfu-programmer atmega32u2 start


