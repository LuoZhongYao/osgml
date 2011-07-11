/*
 *  (c) LuoZH   2011-06
 *      不得不承认,linux那种将内核映射到3GB开始处很有效,这样内核就属于其他进程的
 *  一个子部分,这我也在setup里面提到,但是现在我才意识到这种方法对我这个系统不是
 *  很有效,我决定采用自己的方法,就是将内核当着一个进程来处理,她独立于其他进程.
 *  不过它有别于其他的进程,内核进程的地址空间就是整个物理内存,其他进程有内核分配
 *  ,换句话说,其他进程只是内核的一个数据而言.
 *
 */
#include    <kernel.h>
#include    <asm/io.h>
#include    <string.h>
#define     LATCH   1193180
/*
 *  首先感谢setup同学为我们将要出来表演的哥们准备好了一切,那么,现在的工作看起来
 *  很有趣了,不仅仅是因为我们可以使用C语言,更重要的是我们把几个不是那么对胃口的
 *  东西都让setup完美解决了.
 *      内核从0X200000开始存放,但是她的空间再次被收刮了,我不是贪官污吏,而是她
 *  闺密要和她一起玩,虽然这个闺密很简单,很单纯,不过体积却简单.她要使用1MB的空间
 *  也就是kernel又被压缩了1MB,不过还够用.这个闺密就是mem_map,主内存位图.她存放于
 *  内核4MB空间中的最后一MB,不是说内核有1GB的空间吗,为什么只有4MB,额,这个...
 *      由于策略的改变,内核拥有了整个物理内存空间,不过他并不是丧心病狂的完全使用
 *  而是合理分配给其他人使用,由于setup只是映射了前面4MB,现在我们要进行计算,必要
 *  要的时候在将内存映射.
 *      各个进程可以是独立的互不干扰的,但是有个进程却是特殊的.那就是内核,它需要
 *  和其他进程交换一些数据,所以我们将部分页面完全共享,并且对等映射
 */
kerT    ker_data;
//请求内存管理进程的PDBR
unsigned int *tpdr;
//内存管理进程的PDBR
unsigned int *mpdr;
void test(void);
void test_t(void);
int main(void){
    tssTp   tss=(tssTp)0x100800;    
    int     tr=0x28;
    tss->EIP=(int)test-0x300000+0x401000;
    tss->CS=0x10;
    tss->PDBR=KPDR;
    tss->ES=tss->DS=tss->SS=0x8;
    tss->ESP=0x7FFFFF;
    tss->SS0=0x08;
    tss->ESP0=0x3FFFFF;
    tss->LDT=0;
    tss->IO=0xFFFF0000;
    tss->EFLAGS=0x200;
    ker_data.cur_x=(*(unsigned short *)0x100800)&0xFF; 
    ker_data.cur_y=((*(unsigned short *)0x100800)>>8)&0xFF; 
    tpdr=(unsigned int *)KPDR;
    printk("gmL start...\n");
    /*
     *  安装默认中断
     */
    for(int i=0;i<256;i++){
        install_int(i,null_int,KER_CODE_SEC,INT_GATE|IDT_R0);
    }
    install_int(0,int_0,KER_CODE_SEC,INT_GATE|IDT_R0);
    install_int(1,int_1,KER_CODE_SEC,INT_GATE|IDT_R0);
    install_int(2,int_2,KER_CODE_SEC,INT_GATE|IDT_R0);
    install_int(3,int_3,KER_CODE_SEC,INT_GATE|IDT_R0);
    install_int(4,int_4,KER_CODE_SEC,INT_GATE|IDT_R0);
    install_int(5,int_5,KER_CODE_SEC,INT_GATE|IDT_R0);
    install_int(6,int_6,KER_CODE_SEC,INT_GATE|IDT_R0);
    install_int(7,int_7,KER_CODE_SEC,INT_GATE|IDT_R0);
    install_int(8,int_8,KER_CODE_SEC,INT_GATE|IDT_R0);
    install_int(9,int_9,KER_CODE_SEC,INT_GATE|IDT_R0);
    install_int(10,ts_int,KER_CODE_SEC,INT_GATE|IDT_R0);
    install_int(11,int_11,KER_CODE_SEC,INT_GATE|IDT_R0);
    install_int(12,int_12,KER_CODE_SEC,INT_GATE|IDT_R0);
    install_int(13,int_13,KER_CODE_SEC,INT_GATE|IDT_R0);
    install_int(15,int_15,KER_CODE_SEC,INT_GATE|IDT_R0);
    install_int(16,int_16,KER_CODE_SEC,INT_GATE|IDT_R0);
    install_int(17,int_17,KER_CODE_SEC,INT_GATE|IDT_R0);
    install_int(18,int_18,KER_CODE_SEC,INT_GATE|IDT_R0);
    install_int(19,int_19,KER_CODE_SEC,INT_GATE|IDT_R0);
    /*
     *  将时钟中断的设置为10ms一次
     */
    outb_p(0x36,0x43);
    outb_p(LATCH&0xFF,0x40);
    outb_p(LATCH>>8,0x40);
    /*
     *  安装时钟中断,并开启中断允许
     */
    install_int(20,null_int,KER_CODE_SEC,INT_GATE|IDT_R0);
    outb_p(inb_p(0x21)&~0x1,0x21);
    outb_p(inb_p(0x21)&~0x1,0x21);
    mem_init();
 //   clear();
//    *(unsigned int *)0x900000=0;
    memcpy((void *)0x401000,(void *)0x300000,0x10000);
    asm(
        "ltr %0\n\t"
        "pushfl\n\t"
        "mov    $0xffffbfff,%%eax\n\t"
        "and %%eax,(%%esp)\n\t"
        "popfl\n\t"
        "pushl  $0x8\n\t"
        "pushl  $0x3FFFFF\n\t"
        "pushfl\n\t"
        "pushl  $0x10\n\t"
        "pushl  %1\n\t"
        "sti\n\t"
        "iret\n\t"
        :
        :"m"(tr),"r"(test-0x300000+0x401000)
        :"%eax"
        );

    for(;;);
}
void test(void){
    static int tmp=0;
    if(tmp)
        goto    task_tow;
    tmp=1;
 //   *(unsigned *)0x900000=0xABCDEF;
    tssTp   tss=(tssTp)TSS_START;
    tss->EIP=(int)test-0x300000+0x401000;
    tss->CS=0x23;
    tss->ES=tss->DS=tss->SS=0x1b;
    tss->ESP=0x4FFFFF;
    tss->SS0=0x08;
    tss->ESP0=0x3FFFFF;
    tss->LDT=0;
    tss->IO=0xFFFF0000;
    tss->EFLAGS=0x200;
    tss->PDBR=create_space();
   // *(unsigned *)0x900000=0xEFBAEC;
    o:
    printk("This Task One! \e cr3 = \e\n",*(unsigned *)0x900000,tss->PDBR);
    //asm("mov %0,%%cr3\n\t"::"a"(tss->PDBR));
    asm("ljmp   $0x30,$0");
    //for(int i=0xffff;i--;);
    goto o;
task_tow:
    printk("This Task Two! \e\n",*(unsigned *)0x900000);
    asm("ljmp   $0x28,$0");
   // for(int i=0xffff;i--;);
    goto    task_tow;
}
