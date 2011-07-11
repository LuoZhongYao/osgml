/*
   *    沉寂了这么久,一直不知道路在何方,一直都被女人困惑了心思,使我堕落很长一
   *    段时间,一直不敢动手写操作系统,其实没什么,以前一直有包袱,现在明白了
   *    我没有任何利益思想,有何患后患呢.崛起吧,亲爱的道长.
   *        (c) LuoZH   2011-06-12
   *        以前几次尝试写OS都过分考虑内核外的其他细节,导致很多心思话错了地方
   *    这次换一个角度,尽量将内核无关的部分足够精简,这样效果也许会更好吧.所以
   *    这个引导直接就将整个内核加载进内存,就目前来看,这是非常完美的.而内核是
   *    一个程序,所以就只是一个文件,这就避免了,上次是先写内存管理,还是文件系统
   *    那种无聊却有必须的问题上浪费掉不必要的心思.
   */
/*
   *        引导程序首先被加载到实模式下0:0x7c00,该段代码将setup加载到0x0x7e0:
   *    0处,然后将控制权交给setup
   */
#内核目前将其定位到100KB应该绰绰有余吧
SETUP_LEN = 0x4      #setup占用扇区数
SETUP_ADDR = 0x7e00    #内核加载地址
SETUP_SEC   =0x7e0
KER_LEN     =   0x126    #内核占用扇区数 
KER_SEC     =0x1000     #段
KER_START   =   18       #内核开始存放扇区(回到原始时期的直接读扇区了:-))
.code16
.text
.globl _start
_start:
/*
   *    得到控制权,先猖皮下先,打印Loading...字符,利用BIOS功能
   *    13号功能,01显示方式,
   *    当然,前提是先获得光标位置先
   */
    mov     $0x0300,%ax
    mov     $0x0,%bx
    int     $0x10
    inc     %dh
    xor     %dl,%dl
    mov     %cs,%ax
    mov     %ax,%es
    lea     msg,%bp
    mov     $0x1301,%ax
    mov     $0x0003,%bx
    mov     $0xa,%cx
    int     $0x10
/*
   *    读取4个扇区的SETUP,当然,现在的SETUP还木有4扇区那么大,:-)
   *    由于这4个扇区是小范围,又没有涉及超过一个磁道,所以直接调用int $0x13,这样
   *    快那么一点点
   */
    mov     $SETUP_SEC,%ax
    mov     %ax,%es
    xor     %bx,%bx
    mov     $0x0002,%cx
    mov     $0x0000,%dx
read_setup:
    mov     $0x02,%ah
    mov     $SETUP_LEN,%al
    int     $0x13
    #如果读取失败,继续读,换句话说就是在这之前的任何错误将导致死机
    jc      read_setup      
/*
   *    下面利用readfp加载内核进0x10000去,当前内核还很小,不超过一个段,所以简单处
   *    将来扩充吧.由于某些不为人知的原因(int $0x13一次最多能读18个扇区,也就是
   *    一个磁道,linux内核注释这本数里这么说的,好吧一知半解害死人)
   *        当然,由于我这是初级阶段,为了简单,数据的安排做了简单的特殊处理,即内
   *    核开始扇区刚好位于磁道开始处(1.44MB软盘映像),而且内核大小刚好是整取磁道
   *    也就是刚好沾满N个磁道
   *    
   */
read_sys:
    xor     %bx,%bx
    xor     %dx,%dx
    mov     $0x1000,%ax
    mov     %ax,%es
    mov     $KER_LEN,%di
    mov     $KER_START,%ax
0:
    mov     $18,%cx
    call    readfp
    add     $18,%ax
    add     $0x2400,%bx
    sub     $18,%di
    jc      0f

    /*
    push    $SETUP_SEC
    push    $0
    */
    push    $SETUP_ADDR
    ret
/*
   *    这个是去年的loi0.02的时候的逻辑扇区读取函数,本来该在写一个,但是叫我再去
   *    理解一遍软盘某些问题,就我这个急躁的心,估计怕是要出问题的.急躁这个问题很
   *    严重,就好像昨天才去第一见到女方,我今天就想结婚了,嗯,这个毛病由来已久了
   *    但是我还是不能保证,能够读很多扇区,看linux的代码注释好像说只能最多一次读
   *    18个扇区,好吧,我也这么办吧,这个函数还是花费了我不少心血,今天能重出江海
   *    也算是对我的努力的一种安慰吧,遗憾的是当时忘记这个函数的参考在哪里了,悲
   *    剧+1
   */
/*
   入口参数：
		%cx=扇区数
		%ax=逻辑扇区号
        %dl=驱动器号(这里是个bug,现在才发现)
		%es：%bx=缓冲区
	出口参数：
	*/
/*
   *        利用bios的0x13中断读取软盘内容(起初我是用虚拟软盘承载内核的,这很方
   *    便,为了了方便,软盘的驱动器号是0)
   *        bios 0x13中断读软盘功能接口如下
   *    int $0x13
   *        入口:ah=0x2功能号,读扇区
   *             al=读取扇区数
   *             cl=读取的起始扇区
   *             ch=磁道号
   *             dl=驱动器号
   *             dh=磁头号
   *             es:bx缓冲区
   */
readfp:
	pushw %bp
	movw %sp,%bp
	subw $0x10,%sp
#读取软盘参数
	pusha
	pushw %es
	movw $0xb800,%ax
	movw %ax,%es
0:
	xorw %di,%di
	xorw %dx,%dx
	movw $0x800,%ax
	int $0x13
	jc 0b					#失败则死循环
	movw %dx,-2(%bp)		#驱动器数,磁头数
	movw %cx,%ax
	movb %ch,%dl
	movb %cl,%ch
	movb $5,%cl
	shr %cl,%ch
	movb %ch,%dh		#柱面数
	movw %dx,-4(%bp)
	andw $0x1f,%ax
	movw %ax,-6(%bp)		#扇区数
	popw %es
	popa
#计算逻辑扇区
	pushw %dx
	pushw %bx 
	movw %cx,-8(%bp)
	divb -6(%bp)
	movb %ah,%cl
	incb %cl 
	movb %al,%dh
	andb $1,%dh
	shr $1,%al
	movb %al,%ch

	movw -8(%bp),%ax
0:
	movb $0x2,%ah
	popw %bx
	int $0x13
	jc 0b
	popw %dx
	movw %bp,%sp
	popw %bp
	ret
msg:
    .ascii  "Loading   "
    .org 510
    .short 0xaa55

