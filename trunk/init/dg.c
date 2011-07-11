/*
 *      内核临时调试用打印函数
 */
#include    <kernel.h>
#include    <stdarg.h>
#include    <string.h>
void write_ch(int ch){
    unsigned int addr=ker_data.cur_y*80+ker_data.cur_x;
    addr<<=1;
    switch(ch){
        case    '\n':ker_data.cur_y++;ker_data.cur_x=0;break;
        default:
            asm("add    $0xb8000,%1\n\t"
                "or     $0x0F00,%0\n\t"
                "mov    %%ax,(%1)\n\t"
                :
                :"a"(ch),"b"(addr)
                );
        ker_data.cur_x++;
    }
    ker_data.cur_y+=ker_data.cur_x/80;
    ker_data.cur_x%=80;
    addr=(ker_data.cur_y*80+ker_data.cur_x)*2;
    if(addr>80*25*2){
        memcpy((void *)0xb8000,(void *)(0xb8000+160),25*80*2);
        ker_data.cur_y--;
    }
}
void puthex(unsigned int num){
    if(num){
        puthex(num>>4);
        write_ch("0123456789ABCDEF"[num&0xF]);
    }
}
int printk(char *str,...){
    va_list ap;
    va_start(ap,str);
    while(*str){
        switch(*str){
            case    '\e':puthex(va_arg(ap,unsigned int));str++;break;
            default:write_ch(*str++); 
        }
    }
}
void clear(void){
    ker_data.cur_x=ker_data.cur_y=0;
    memset((void *)0xb8000,0,80*25*2);
}
