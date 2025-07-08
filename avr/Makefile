# Makefile

MCU?=attiny2313
PROG=p32burner-${MCU}
CFLAGS=-Os -mmcu=${MCU} -Wall -DF_CPU=9216000L

SRCS=main.c
OBJS=${SRCS:%.c=%.o}

-include config.mk
include common.mk

write:
	${AVRDUDE} -P /dev/dtyU0 -c ft232serbb -p ${MCU} -U flash:w:${PROG}.hex

fuse-attiny2313:
	${AVRDUDE} -P /dev/dtyU0 -c ft232serbb -p ${MCU} \
		-U efuse:w:0xFF:m \
		-U hfuse:w:0xDF:m \
		-U lfuse:w:0x6F:m \

main.o: main.c tty.c pin.c icsp.c jtag.c prog.c cmddef.h tapdef.h
