/*
 *      LuoZH/init/init.c
 *      如kernel.h文件里描述,我不擅长处理文件名,这个文件名就是借用linux的,但是
 *  内容没有用linux的,原因很简单,我看不懂linux的,:-)
 *      (c) 2011    LuoZH
 */
#include    <kernel.h>
//#include    <io.h>
#include    <asm/io.h>
//IDT表指针
const idtTp idt =   (idtTp) IDT_ADDR;
/*
 *  默认中断C处理函数
 */
void    nop(void){
    ;
}
void    print_error(int error){
    printk("This error is \e\n",error);
}
/*
 *  时钟中断处理函数
 */
void    clock(void){
    printk("Clock\n");
    outb(0x20,0x20);
    outb(0x20,0xA0);
}
void install_int(unsigned int num,int_codeTp  func,unsigned int section,unsigned short attr){
    idt[num].addr1=(unsigned short)((unsigned int)func)&0xFFFF;
    idt[num].addr2=(unsigned short)(((unsigned int)func)>>16)&0xFFFF;
    idt[num].section=section;
    idt[num].attr=attr;
}
