/*
   *    (C) Mr.gmL
   *    2011-08-31
   *    Email:LuoZhongYao@gmail.com
   *    由于之前的boot读写在超过一个磁道后读取的内容会出现问题,修复这个问题很麻
   *    烦,而现在软盘这种设备已经不存在了,导致我想在真机上测试下都得改来该去,所
   *    以,这次直接用硬盘,并且使用LBA寻址方式
   */
SETUP_LEN   =   0x4     #setup  占用扇区数
SETUP_ADDR  =   0x7e00  #SETUP模块加载地址
SETUP_SEC   =   0x7e0   #16位模式下的SETUP加载段地址
SETUP_START =   1
KER_LEN     =   128   #内核占用扇区数
KER_SEC     =   0x1000
KER_START   =   18      #内核开始存放扇区,后面有文件系统后修改
.code16
.text
.globl  _start
_start:
    /*
       *    获得控制权,猖皮下先,打印Loading...字符,利用BIOS功能
       */
    mov $0x300,%ax
    mov $0,%bx
    int $0x10
    inc %dh
    xor %dl,%dl
    mov %cs,%ax
    mov %ax,%es
    lea msg,%bp
    mov $0x1301,%ax
    mov $0x3,%bx
    mov $0xa,%cx
    int $0x10
/*
   *    如果扩展功能不存在,死机给你看
   *    读取4扇区的SETUP到指定位置,扩展的INT 0x13 google之
   */
    mov $0x41,%ah
    mov $0x55aa,%bx
    mov $0x80,%dl
    int $0x13
    jc  error
    mov $0x0,%ax    #读SETUP
    mov %ax,%ds
    lea DAP,%si
    mov $0x42,%ah
    mov $0x80,%dl
    int $0x13
    jc  error
    mov $KER_LEN,%ax
    mov %ax,BlockCount
    mov $KER_SEC,%eax
    shl $16,%eax
    mov %eax,BufferAddr
    mov $KER_START,%eax
    mov %eax,Block
    mov $0x80,%dl
    mov $0x42,%ah
    int $0x13
    jc  error
#进入SETUP执行
#    push    $SETUP
#    push    $0
    jmp 0x7e00
error:
    jmp .
DAP:
PacketSize:     .byte   16          #sizeof(DAP)
Reserved:       .byte   0           #信息标志,恒0
BlockCount:     .short  SETUP_LEN   #SETUP占用扇区块数
BufferAddr:     .int    SETUP_SEC<<16
Block:          .quad   SETUP_START #读取绝对扇区地址
msg:
    .string "Loading..."
    .org    510
    .short 0xaa55
