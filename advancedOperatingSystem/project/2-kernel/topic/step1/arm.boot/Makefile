
QEMU=/usr/bin/qemu-system-arm
QEMU=/homex/opt/qemu.git/arm-softmmu/qemu-system-arm
TOOLCHAIN=/homex/opt/gcc-arm-none-eabi-4_8-2014q3/bin

ASFLAGS= -mcpu=arm926ej-s -g 
CFLAGS= -c -mcpu=arm926ej-s -g 
LDFLAGS= -T test.ld 

all: startup.o test.o
	$(TOOLCHAIN)/arm-none-eabi-ld $(LDFLAGS) test.o startup.o -o test.elf
	$(TOOLCHAIN)/arm-none-eabi-objcopy -O binary test.elf test.bin

clean: 
	rm -f startup.o test.o test.elf

startup.o: startup.s
	$(TOOLCHAIN)/arm-none-eabi-as $(ASFLAGS) startup.s -o startup.o

test.o: test.c
	$(TOOLCHAIN)/arm-none-eabi-gcc $(CFLAGS) test.c -o test.o

run: all
	$(QEMU) -M versatilepb -m 128M -nographic -kernel test.bin -serial mon:stdio

debug: all
	$(QEMU) -M versatilepb -m 128M -nographic -kernel test.bin -serial mon:stdio -s -S

run.telnet:
	$(QEMU) -M versatilepb -m 128M -nographic -kernel test.bin -serial telnet:localhost:5555,server -monitor stdio

debug.telnet:
	$(QEMU) -M versatilepb -m 128M -nographic -kernel test.bin -serial telnet:localhost:5555,server -monitor stdio -s -S 

#
# This is to launch the right debugger, the one from your chosen toolchain.
# It will read in the .gdbinit file, so it will automatically to a Qemu instance,
# waiting on the port localhost:1234. 
# This means that you must have started qemu before launching gdb, 
# with the "-s -S" options, as illustrated with the makefile target "debug" above.
gdb:
	$(TOOLCHAIN)/arm-none-eabi-gdb test.elf

kill:
	pkill qemu-system-arm

