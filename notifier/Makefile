IP430_DIR=../net/ip430
IP430_A=$(IP430_DIR)/ip430.a
MCU=-mmcu=msp430g2553
CFLAGS=$(MCU) -std=gnu99 -Os -DHAVE_TCP -I$(PWD) -I$(IP430_DIR) -DUDP_LOG
OBJS=enc28j60.o  main.o  mem.o  spi.o  spi_mem.o  uart.o cpu_asm.o
SRC=enc28j60.c  main.c  mem.c  spi.c  spi_mem.c  uart.c cpu_asm.S
CC=msp430-gcc
AR=msp430-ar

notifier.elf: $(IP430_A) $(SRC)
	$(CC) $(CFLAGS) -o $@ $(SRC) $(IP430_A)
	#$(CC) -o $@ $(OBJS) $(IP430_A)

$(IP430_A): force
	$(MAKE) CC=$(CC) AR=$(AR) INCLUDE_PATH=-I$(PWD) -C $(IP430_DIR) CFLAGS="$(CFLAGS)"

.c.o :
	$(CC) $(CFLAGS) -c $<

.S.o :
	$(CC) $(CFLAGS) -c $<

force:
	

clean:
	$(MAKE) -C $(IP430_DIR) clean
	rm -f *.o notifier.elf
