NET430_DIR=../net/net430
IP430_DIR=../net/ip430
include $(NET430_DIR)/net430.mk
PROG=receiver
MCU=-mmcu=msp430g2553
DEFINES=-DHAVE_TCP \
	-DUDP_LOG
CFLAGS=$(MCU) -std=gnu99 -Os -I$(PWD) -I$(NET430_DIR) -I$(IP430_DIR) $(DEFINES)
OBJS=main.o
CC=msp430-gcc
AR=msp430-ar

$(PROG).elf: $(NET430_DEPS) $(IP430_DEPS) $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(NET430_OBJS) $(IP430_OBJS)

prog: $(PROG).elf
	mspdebug rf2500 "prog $(PROG).elf"

.c.o :
	$(CC) $(CFLAGS) -c $<

.S.o :
	$(CC) $(CFLAGS) -c $<

force:
	

clean:
	rm -f *.o $(PROG).elf
