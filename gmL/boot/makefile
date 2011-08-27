#引导扇区编译写入管理
.PHONY:create clear
AS = as
LD = ld
AS_OPT = --32
LD_OPT = --oformat binary -Ttext 0x7c00 -m elf_i386
#写映像的命令
WR = dd
#创建映像的选项
CREATE_IMG_OPT = bs=1440K conv=sync count=1
#写引导命令
WR_BOOT_OPT = conv=notrunc
IMG = ../gmL.img
$(IMG):bootload
	$(WR) if=$^ of=$@ $(WR_BOOT_OPT)
bootload:boot.o
	$(LD) $^ -o $@ $(LD_OPT)
%.o:%.s
	$(AS) $^ -o $@ $(AS_OPT)
create:
	$(WR) if=/dev/zero of=$(IMG) $(CREATE_IMG_OPT)
clear:
	-rm *.o -f
	-rm bootload -f
