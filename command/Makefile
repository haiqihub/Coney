# commands/Makefile

#ENTRYPOINT	= 0x1000
HD		= ../80m.img

ASM		= nasm
DASM		= objdump
CC		= gcc -m32
LD		= ld -m elf_i386
ASMFLAGS	= -I ../include/ -f elf
CFLAGS		= -I ../include/ -c -fno-builtin -fno-stack-protector -Wall
LDFLAGS		= -Ttext 0x1000
DASMFLAGS	= -D
LIB		= ../lib/orangescrt.a
BIN		= cd mkdir rm killproc proclist ls TIM
BIN1		= echo jim cd mkdir rm killproc proclist tes
BIN2		= tes calc toc box
# All Phony Targets
.PHONY : everything final clean realclean disasm all install

# Default starting position
everything : $(BIN)

install : all clean
	cp ../boot/hd*.bin ./ -v
	cp ../kernel.bin ./ -v
	tar vcf inst.tar $(BIN)
	dd if=inst.tar of=$(HD) seek=`echo "obase=10;ibase=16;(\`egrep -e '^ROOT_BASE' ../boot/include/load.inc | sed -e 's/.*0x//g'\`+\`egrep -e '#define[[:space:]]*INSTALL_START_SECT' ../include/sys/config.h | sed -e 's/.*0x//g'\`)*200" | bc` bs=1 count=`ls -l inst.tar | awk -F " " '{print $$5}'` conv=notrunc

all : realclean everything

final : all clean

clean :
	rm -f *.o

realclean :
	rm -f $(BIN) *.o

kernel.bin :
	cp ../kernel.bin ./

start.o : start.asm
	$(ASM) $(ASMFLAGS) -o $@ $<

echo.o: echo.c ../include/type.h ../include/stdio.h
	$(CC) $(CFLAGS) -o $@ $<

echo : echo.o start.o $(LIB)
	$(LD) $(LDFLAGS) -o $@ $?

pwd.o: pwd.c ../include/type.h ../include/stdio.h
	$(CC) $(CFLAGS) -o $@ $<

pwd : pwd.o start.o $(LIB)
	$(LD) $(LDFLAGS) -o $@ $?

TIM.o:TIM.c ../include/type.h ../include/stdio.h
	$(CC) $(CFLAGS) -o $@ $<

TIM : TIM.o start.o $(LIB)
	$(LD) $(LDFLAGS) -o $@ $?

ls.o:ls.c ../include/type.h ../include/stdio.h
	$(CC) $(CFLAGS) -o $@ $<

ls : ls.o start.o $(LIB)
	$(LD) $(LDFLAGS) -o $@ $?

cd.o:cd.c ../include/type.h ../include/stdio.h
	$(CC) $(CFLAGS) -o $@ $<

cd : cd.o start.o $(LIB)
	$(LD) $(LDFLAGS) -o $@ $?

mkdir.o:mkdir.c ../include/type.h ../include/stdio.h
	$(CC) $(CFLAGS) -o $@ $<

mkdir : mkdir.o start.o $(LIB)
	$(LD) $(LDFLAGS) -o $@ $?

rm.o:rm.c ../include/type.h ../include/stdio.h
	$(CC) $(CFLAGS) -o $@ $<
     
rm : rm.o start.o $(LIB)
	$(LD) $(LDFLAGS) -o $@ $?

tes.o:tes.c ../include/type.h ../include/stdio.h
	$(CC) $(CFLAGS) -o $@ $<
     
shi : tes.o start.o $(LIB)
	$(LD) $(LDFLAGS) -o $@ $?

proclist.o: proclist.c ../include/type.h ../include/stdio.h
	$(CC) $(CFLAGS) -o $@ $<

proclist : proclist.o start.o $(LIB)
	$(LD) $(LDFLAGS) -o $@ $?

killproc.o: killproc.c ../include/type.h ../include/stdio.h
	$(CC) $(CFLAGS) -o $@ $<

killproc : killproc.o start.o $(LIB)
	$(LD) $(LDFLAGS) -o $@ $?

main.o: main.c  ../include/stdio.h
	$(CC) $(CFLAGS) -o $@ $<

main : main.o start.o $(LIB)
	$(LD) $(LDFLAGS) -o $@ $?

toc.o: toc.c  ../include/stdio.h
	$(CC) $(CFLAGS) -o $@ $<

toc : toc.o start.o $(LIB)
	$(LD) $(LDFLAGS) -o $@ $?

box.o: box.c  ../include/stdio.h
	$(CC) $(CFLAGS) -o $@ $<

box : box.o start.o $(LIB)
	$(LD) $(LDFLAGS) -o $@ $?

calc.o: calc.c  ../include/stdio.h
	$(CC) $(CFLAGS) -o $@ $<

calc : calc.o start.o $(LIB)
	$(LD) $(LDFLAGS) -o $@ $?

