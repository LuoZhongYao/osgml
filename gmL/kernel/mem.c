/*
 *      (c) LuoZhongYao@gmail.com  2011-08-22 
 */
#include    <gmL/kernel.h>
#include    <gmL/mm.h>
#include    <x86/trap.h>
#include    <dgio.h>
#include    <stddef.h>
#include    <string.h>
char    *mm_map;
unsigned int HMEM = 0;
unsigned int LMEM = 0;
//内存大小4KB为单位
unsigned int mm_size;
void    page_fault(void);
void mm_init(void){
    //内存大小在setup里面获取了,保存在0x100804里面.
    mm_size = *(unsigned *)0x100804>>2;
    mm_map = (char *)MEM_MAP_ADDR;
    for(int i=0;i<0x100000;i++)
        mm_map[i] = 100;
    //mem_size是内存大小KB为单位,而页是4KB为单位,所以
    for(int i=0x800;i<mm_size;i++)
        mm_map[i] = 0;
    HMEM = mm_size<<12;
    LMEM = 0x400000;
    //安装页中断
    install_int(14,page_fault,KER_CODE_SEC,TRA_GATE|IDT_R0);
}
void *get_free_page(void){
    for(unsigned i=0;i<mm_size;i++){
        if(!mm_map[i]){
            mm_map[i]=1;
            return  (void *)(i<<12);
        }
    }
    return  NULL;
}
void free_page(unsigned int addr){
    if(addr>LMEM||addr<HMEM)
        mm_map[addr>>12]--;
}
void *put_page(void *addr,)
