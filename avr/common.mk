# Makefile

.SUFFIXES: .c .o .S .s

.c.o:
	${CC} ${CFLAGS} -c -o $@ $<

.c.s:
	${CC} ${CFLAGS} -S -o $@ $<

.S.o:
	${CC} ${CFLAGS} -c -o $@ $<

.s.o:
	${CC} ${CFLAGS} -c -o $@ $<

all: ${PROG}.elf ${PROG}.hex

${PROG}.elf: ${OBJS}
	${CC} ${CFLAGS} -o $@ ${OBJS}
	LANG=C ${OBJDUMP} -d $@ > ${PROG}.dump
	@echo >> ${PROG}.dump
	LANG=C ${SIZE} $@ >> ${PROG}.dump

${PROG}.hex: ${PROG}.elf
	${OBJCOPY} -I elf32-avr -O ihex ${PROG}.elf ${PROG}.hex

init:
	if [ ! -f config.mk ]; then \
		cp default.mk config.mk; \
	fi

clean:
	rm -f *.s *.o *.elf *.dump *.hex
