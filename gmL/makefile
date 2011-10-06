IMG=gmL.hd
.PHONY:clear start make
LD_OPT = --oformat binary -m elf_i386 
LD = ld
SYS=system
SYS_O=kernel/init.o kernel/trap.o kernel/int.o kernel/dg.o kernel/sys_call.o kernel/sys.o kernel/sched.o kernel/mm.o kernel/fork.o kernel/thread.o
#下面链接screen.o是为了测试我的任务能不能启动,最终是要被分离出来的
SETUP_O = kernel/setup.o
SETUP = setup
#标准C库
C_O = cstd/string.o
start:make $(IMG)
make:
	ctags -R
	(cd boot;make)
	(cd kernel;make)
	(cd cstd;make)
$(IMG):$(SETUP) $(SYS)
$(SYS):$(SYS_O) $(C_O)
	$(LD) $^ -o $@ $(LD_OPT) -e main -Ttext 0x300000
	dd if=$@ of=$(IMG) seek=18 bs=1b conv=notrunc 
$(SETUP):$(SETUP_O)
	$(LD) $^ -o $@ $(LD_OPT) -Ttext 0x7e00
	dd if=$@ of=$(IMG) seek=1 bs=1b conv=notrunc 
create:
	(cd boot;make create)

clear:
	rm -f $(SETUP) $(SYS) tags
	(cd boot;make clear)
	(cd kernel;make clear)
	(cd cstd;make clear)
