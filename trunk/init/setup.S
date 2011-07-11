/*
   *    由于代码用到了C语言的宏,也就是,我们在编译前调用cpp处理代码,所以注释不能
   *    使用#开头了,请使用/* *\/
   *        (c) 2011-06-12 LuoZH
   *    嗯嗯...,正式开始啦,程序开头将GDT安装到0x0:0地址处,所以bios木能使用了,然
   *    初始化进入保护模式的相关数据,进入保护模式并调用init.s开始初始化程序 
   *        程序进入保护模式后首先将自己移动到0x1M处,1M以下的内存留着内核使用的
   *    数据结构,和一些全局变量使用.
   *        思前想后,还是决定将setup.S模块独立处system模块,由etup模块将system
   *    模块放置到1M处,不然由会很麻烦,当然这样又要直接读扇区一次喽,另外setup.S
   *    模块我假设它不会超过1KB,不然没有文件系统的软盘如何读取system模块呢?
   *        每一个代码段,也许背后就是一个故事.昨天晚上,我破天慌的去了马红连家,
   *    和我弟,这种事情发生在我上算得上是奇迹.不过话有说回来,我身上什么事情都会
   *    发生,这已经有21年的历史验证了.说到21,我今天查了下我的年龄是刚满20周岁,
   *    8月号就满21岁,也就是说我要到明年的8月8号才到法定结婚年龄.本来不该提这件
   *    事的,只是刚才,我以前的顾客来我门口这里等车,聊了下,原来他们是去政府打算
   *    告乡政府,因为哪位大妈被揍了,嗯...,身上全是清一块紫一块的.我看来不禁打起
   *    了哆嗦.当然凶手,(没错,应该就是凶手这个词,不过凶手是形容人的,打她的那些应
   *    该不能称为人吧.)就是我们"亲爱的"计生办相关畜生(对不起畜生了,将它们归为
   *    你们这类有点侮辱你们),而原因是他们的儿子还差几天才22岁,但是他结婚了.
   *    这让我很是为难,理论上我已经是喜欢上了马红连了,理由没有,如果成的话也许没
   *    多久我们就会结婚,当然这是最理想状态,因为我不知道她是否能看上我,嗯嗯...
   *    不过话又说回来,她今年24岁,按她父母的意思,今年是她的本命年,所以应该会是明
   *    年结婚,这么说来,这也许就是缘分吧.望老爸在天保佑.
   *        好吧,我承认直接,我更喜欢在代码里面讲故事,直接叫我写一篇blog我却写不
   *    出来.
   *        写了这么多,其实我是想写,我打算将8259的初始化部分放到setup模块里面,这
   *    个主要是参考了linux0.12的源码.嗯,不过我留了很大一部分空间个setup,如果不
   *    放点什么在里面很对不起它.至于8259我尽量保持很linux相一直,另外,其实我很
   *    希望上层接口也能跟linux一致,这样可以可以和方便的使用linux的软件.这绝对
   *    很诱惑.另外,其实我主要参考了loi0.02
   */
/*
   *        再次的,我又必须得修改setup模块了,以前一直将setup模块弄的尽量简洁,
   *    现在看来这个计划又要落空了,这次修改的原因是,我准备现在就开启分页机制,
   *    把内核部分映射到3GB开始处,这样做主要参考了linux现在的处理方式,好处就是
   *    内存处理会很方便,另外BIOS处理上也会带来不错的收益.
   *        既然内核被映射到3GB开始处,那么链接的时候就得指定-Ttext参数为3GB,
   *    而如果将启动分页机制部分也放入system模块,那么没有开启分页机制之前就必须
   *    把SYSTEM模块放入物理内存3GB处,这不现实,另一种就是单独出一块来,这样就还
   *    要多一次软盘读写操作,这很麻烦,所以最佳第六人就是setup,辛苦你了,哥们.
   */
#define     ASM_SRC
#include    "section.h" 
#include    <kernel.h>
CODE32_START    =   pe
/*
   *    是模式下获取的系统需要的数据保存的线性地址.
   */
CONF_DATA   =   0x100800
CONF_START  =   conf_start
CONF_LEN    =   conf_end-conf_start
/*
   *    内核使用的页表开始地址
   *    内核页目录地址(也就是虚拟地址中3GB这一项在页目录中的位置的物理地址)
   *    好吧这里非常迷糊,如果谁看了有不理解的联系我
   *    Email:LuoZhongYao@gmail.com
   */
/*KPDR    =   0x211000  */
KPDE  =   KPDR+0x300*4   
GDT_LEN=gdt_end-gdt_start
.text
.code16
.globl _start
_start:
/*
   *    上手就初始化8259似乎和符合我的个性---急
   */
/*
   *    先获得内存大小的信息,本来应该使用另一个复杂一点的中断,不过,那太复杂了
   *    我不喜欢.获得的内存大小存放在men_size里面,进入保护模式后转移到IDT和GDT
   *    中间那2KB的空隙里,这2KB的空隙在下面的注释里面有说明,将来或许还会有
   *    更多的数据放在那里面
   */
/*
   *    int     $0x15
   *    入口:
   *        ax=0xe081
   *    出口:
   *        ax=低于16MB的扩展内存,单位是1KB
   *        bx=高于16MB的扩展内存,单位64KB
   *        cx<=>ax
   *        bx<=>dx
   *    到底是使用ax,bx还是cx,dx取决于bios
   */
/*
   *    ??bois不支持就死机,这样太霸道了点吧
   *    另外这部分是抄linux的,对是抄,抄,腾讯?
   */
mem:
    clc
    xor     %edx,%edx
    xor     %ecx,%ecx
    mov     $0xe801,%ax
    int     $0x15
    jc      mem
    cmp     $0x0,%edx
    jne     mem_ok
    cmp     $0x0,%ecx
    jne     mem_ok
    mov     %eax,%ecx
    mov     %ebx,%edx
mem_ok:
    shl     $6,%edx
    addl    %ecx,%edx
    addl    $1024,%edx
    mov     %edx,mem_size
/*
   *    取光标位置
   */
    mov     $0x0300,%ax
    xor     %bx,%bx
    int     $0x10
    mov     %dx,cur
/*
   *    引导结束了,现在我们可以将0x7c00~0x7E00这512字节用来存放内存分布的数据结
   *    构了.
   */
/*
mem_0xE820:
    mov     $0x0,%ax
    mov     %ax,%es
    xor     %di,%di
    xor     %ebx,%ebx
0:
    mov     $20,%ecx
    mov     $0x534d4150,%edx
    mov     $0xe820,%ax
    int     $0x15
    jc      1f
    add     $20,%di
    cmp     $0x0,%ebx
    jne     0b
1:
    mov     $0x0,mem_size
    */
     
    /*
       *    根据linux里面的设置ICW1是0x11,这个我想大部分也该是这个值
       *    主ICW2是0x20表示中断范围是0x20~0x27.而从片是0x28
       *    从片接在主片的IR2脚上,所以ICW3分别为0x04,而从片是0x02,这里我稍微说明
       *    下,主片的ICW3中置一的为表示相应引脚接有从片所以主片位2置1,十六进制
       *    就是0x4,从片的ICW3只使用低3位,相应的值就表示接主片的相应脚,所以从片
       *    的置为0x2,接主片IR2,这里做说明的原因是我以前在这里掉进过陷阱,所以有
       *    必要提醒下.
       */
/*
   *    IO延时
   */
    cli
.macro  io_delay
    jmp     .+2
    jmp     .+2
.endm
    mov     $0x11,%al
    out     %al,$0x20
    io_delay
    out     %al,$0xa0    
    io_delay

    mov     $0x20,%al
    out     %al,$0x21
    io_delay
    mov     $0x28,%al
    out     %al,$0xa1
    io_delay   

    mov     $0x04,%al
    out     %al,$0x21
    io_delay
    mov     $0x2,%al
    out     %al,$0xa1
    io_delay

    mov     $0x1,%al
    out     %al,$0x21
    io_delay
    out     %al,$0xa1
    io_delay
/*
   *    屏蔽所有外部中断先,以后由各自的中断安装程序开启
   */
    mov     $0xFF,%al
    out     %al,$0x21
    io_delay
    mov     %al,0x21
    io_delay
/*
   *    下面开始干正事,不过什么是正事你的?
   *    :-),装GDT,加载,不进入保护模式
   */
    /*
    mov     %cs,%ax
    mov     %ax,%ds
    mov     $0x0,%ax
    mov     %ax,%es
    mov     $GDT,%di
    lea     gdt_start,%si
    mov     $GDT_LEN,%cx
    shr     $2,%cx
    cld
    rep movsl
    */
/* 加载gdt */
    lgdt     gdt 
/* 加载idt */
    lidt     idt 
/* 开A20 */
/* 开A20 */
    mov     $2,%ax
    outb    %al,$0x92
/* 置PE位 */
    mov     %cr0,%eax
    orl     $0x10001,%eax
    mov     %eax,%cr0
    ljmp    $KER_CODE_SEC,$pe

.code32
pe:
/*
   *    临时堆栈
   */
    mov     $KER_DATA_SEC,%eax
    mov     %eax,%ss
    mov     %eax,%ds
    mov     %eax,%es
    mov     $0x3FFFFF,%esp
    /*
       *    将获得的配置信息转移到CONF_DATA处
       */
    mov     $CONF_DATA,%edi
    mov     $CONF_LEN,%ecx
    mov     $CONF_START,%esi
    rep     movsb
    /*
       *    现在正式将临时GDT转正,当然,她并没有得到涨工资的待遇
       *    那个临时GDT的确很悲剧,在没有进入保护模式的时候,他就是亲爹亲娘,进入保
       *    模式她就被无情的抛弃了,但不是不得不承认我是一个很讲情面的人,所以在
       *    gmL里她并没有被遗弃,之前是因为没车没房只能委屈她和setup合租一间房子
       *    现在好了,在她的支持下,我们拥有了4GB的别墅,是该给她个交代了,把她从
       *    出租屋接出来,住进别墅,并且给她一个名分.从女朋友升级位老婆了.所以
       *    还是跟我混好,跟linus混就是被无情的抛弃,甚至连出租屋都被linus无情的
       *    占用.太不厚道了
       */
   mov      $0x101000,%edi 
   mov      $gdt_start,%esi
   mov      $GDT_LEN,%ecx
   shr      $2,%ecx
   rep      movsl

   /*
      *     下面就是给他给名分,这里我利用了一点技巧,(将来说不定是个bug)因为我
      * setup在实模式下段是0,而保护模式下是0~4GB的段,所以实际这段程序不管在保护
      * 模式还是实模式下,地址都是正确的,很强大吧.
      */
   mov      $gdt,%ebx
   mov      $0xFFFF,%eax
   mov      %ax,(%ebx)
   mov      $0x101000,%eax
   mov      %eax,2(%ebx)
    lgdt    gdt
/*
   *    不知道什么原因,栈上有点问题,这里再次加载段寄存器试试
   *    问题解决了,段没有设置B位,导致使用的是16的栈,好生纠结额
   */
    /*
    mov     $0x8,%eax
    mov     %eax,%ss
    */
/*
   *    转移内核进入3M内存处
   */
    mov     $0x10000,%esi
    mov     $0x300000,%edi
    mov     $0x10000,%ecx
    shr     $2,%ecx
    rep     movsl
/*
   *    好,我们现在来为内核开启分页机制,这本是内核自己的事,不过setup模块也是内核
   *    这里只是使用简单的几个表项,目的仅仅是让内核以为自己在3GB处看风景
   *    虽然内核线性地址是3GB,但实际上它还是存在于物理内存的前面4MB,其中前1MB不
   *    使用,目的是保留回到实模式时还能继续使用中断之类的数据,对我们来说丢掉1MB
   *    内存的确算不上什么浪费.第2MB开始,前256*8B用来存放IDT表,随后是GDT,GDT大约
   *    是占用2的13次方乘上8字节,另外为了对齐4KB内存边界,我们不直接从IDT后面就
   *    存放GDT,而是空2KB从4KB处开始存放GDT,IDT+2KB+GDT=68KB(刚好也内存边界4KB
   *    对齐),而后就是我们的内核的页目录表和页
   *    表项了,虽然空间很小,对内核那几个表项来说还是绰绰有余.内核代码段,数据段
   *    从3MB开始只到
   *    4MB处向下来的栈,也就是实际内核才拥有2MB的空间,也许你觉得很小,那你就错了
   *    实际,由于内核的精简性,内核未必能用到100KB的内存.所以我很仁慈的.4MB的内存
   *    实际只需要一个页表就可以了,一个页表1024个页面,每个页面4KB,刚好4MB
   *    
   */
    /*
       *    将整个1024个表项都设置成不存在,然后将0x300项设置存在,并指向2MB72KB处
       *    好吧,换种说法就是将3GB开始的4MB的表项在页目录中的项初始化,表项放在
       *    2MB72KB处,这个位置上面注释里面说过的,随后初始化页表,使3GB~3GB4MB映射
       *    到物理内存的0~4MB
       */
/*
    mov     $KPDE,%ebx
    mov     $1024,%ecx
    xor     %eax,%eax
0:
    mov     %eax,(%ebx)
    add     $4,%ebx
    loop    0b

    mov     $KPDR,%ebx
    mov     $0x112001,%eax
    mov     %eax,(%ebx)

    mov     $1024,%ecx
    mov     $0x112000,%ebx
    mov     $1,%eax
0:
    mov     %eax,(%ebx)
    add     $4,%ebx
    add     $0x1000,%eax
    loop    0b
    */
/*
   *    如果你看过下面有几个单词哪里的注释你会明白这段的作用,首先,我们必须得让程
   *    在开启分页机制后还能继续运行,那么开启分页和没有开启的情况下,这段代码都必
   *    都有相同的地址,不管是虚拟地址还是物理地址.否则,由于地址映射错误就不能
   *    正确运行,好的,那么我们就将前4MB对等映射,页表项表就放在内核表项后面
   *    1MB76KB处,由于启动第一个进程的时候还没有启动分页进程,不能进行内存映射,我
   *    在这里先提前将它映射了,为了处理方便,这个表项存放在1MB80KB处
   */
/*
   *    先将所有页表都初始化为不存在
   */
    mov     $KPDR,%ebx
    mov     $0x0,%eax
    mov     $1024,%ecx
0:
    mov     %eax,(%ebx)
    add     $4,%ebx
    loop    0b

    mov     $KPDR,%ebx
    mov     $0x112007,%eax
    mov     %eax,(%ebx)
    add     $4,%ebx
    mov     $0x113007,%eax
    mov     %eax,(%ebx)
    mov     $1024,%ecx
    shl     $1,%ecx
    mov     $0x112000,%ebx
    mov     $0x7,%eax
0:
    mov     %eax,(%ebx)
    add     $0x1000,%eax
    add     $0x4,%ebx
    loop    0b
       
    /*
       *    NEW,我们加载CR3,修改CR0,正式享受分页的乐趣吧,或者又是一次疯狂的调试
       */
    mov     $0x111000,%eax
    mov     %eax,%cr3
    mov     %cr0,%eax
    or      $0x80000000,%eax
    mov     %eax,%cr0
/*
   *    天那,那下面的怎么办呢?what?why?
   */
  /*
   *    我很无辜,因为我的编译器默认认为所有段都是是一样的,当它需要用栈里面的数据
   *    但是又没有用esp,ebp寄存器的时候它省略了段前缀.
   */
    /*  .byte   0x66    */
    /*
       *    为了部落的繁荣,我们先将esp,ebp处理一下先
       *    本来他们应该从4GB开始才对,不过,这样还要进行几次小规模的映射,为了简单
       *    暂且就直接利用映射了的最后部分吧,毕竟这些都是临时栈.
       */
    mov     $0x3FFFF0,%esp
    mov     %esp,%ebp
    mov     $0x300000,%eax
    jmp     *%eax
conf_start:
cur:
    .short  0
    .short  0
mem_size:
    .int   0
conf_end:
/*  GDT */
gdt:
    .word   GDT_LEN
    .long   gdt_start
idt:
    .word   256*8-1
    .long   0x100000
gdt_start:
/*
   *    下面这段改动最多的地方了,而且牵涉很广,每次改一下都要修改好几个宏
   *    虽然如此,不过这次我最欣赏,理由就是简单明了.
   *        最后那个TSS描述符作为所有进程共享使用的TSS,所有任务的切换经采用该
   *    描述符,不过在这之前进行了页目录的切换,所以实际上使用的是进程自己的TSS
   *    由于缓冲的原因,我们必须还得刷新TR;
   */
CreateSection(0xAA,0,0XBB)
CreateSection(0x0,0xFFFFF,DATA_WRITE|DATA_G|R0|DATA_B);
CreateSection(0x00000,0xFFFFF,CODE_READ|CODE_D|CODE_G)
CreateSection(0x0,0xFFFFF,DATA_WRITE|DATA_G|R3|DATA_B);
CreateSection(0x00000,0xFFFFF,CODE_READ|CODE_D|CODE_G|R3)
CreateSection(0x100800,TSS_SIZE,TSS|R3)
CreateSection(TSS_START,TSS_SIZE,TSS|R3)
gdt_end:
