IMG=gmL.img
.PHONY:clear start make
LD_OPT = --oformat binary -m elf_i386 
LD = ld
SYS=system
SYS_O=init/init.o init/trap.o mm/mem.o init/int.o init/dg.o
#下面链接screen.o是为了测试我的任务能不能启动,最终是要被分离出来的
SETUP_O = init/setup.o
SETUP = setup
#标准C库
C_O = cstd/string.o
start:make $(IMG)
make:
	(cd boot;make)
	(cd init;make)
	(cd mm;make)
	(cd task;make)
	(cd cstd;make)
$(IMG):$(SETUP) $(SYS)
	ctags -R
$(SYS):$(SYS_O) $(C_O)
	$(LD) $^ -o $@ $(LD_OPT) -e main -Ttext 0x300000
	dd if=$@ of=$(IMG) seek=18 bs=1b conv=notrunc 
$(SETUP):$(SETUP_O)
	$(LD) $^ -o $@ $(LD_OPT) -Ttext 0x7e00
	dd if=$@ of=$(IMG) seek=1 bs=1b conv=notrunc 

clear:
	rm -f $(SETUP) $(SYS) tags
	(cd boot;make clear)
	(cd task;make clear)
	(cd init;make clear)
	(cd mm;make clear)
	(cd cstd;make clear)
