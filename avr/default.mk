# builder configuration

# for NetBSD, tune this
TOOLDIR=/usr/cross
# for Fedora Linux with avr-gcc packages
#TOOLDIR=/usr

CC=${TOOLDIR}/bin/avr-gcc
OBJDUMP=${TOOLDIR}/bin/avr-objdump
OBJCOPY=${TOOLDIR}/bin/avr-objcopy
SIZE=${TOOLDIR}/bin/avr-size
AVRDUDE=${TOOLDIR}/bin/avrdude
