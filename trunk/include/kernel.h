/*
 *      很讽刺,我经常遇见的问题,竟然是文件命名,函数名之类的问题.这个文件号称内
 *  核数据定义,但是像描述符之类的都扔在这里.当然他们也是内核数据
 *      (c) 2011    LuoZH
 */
#ifndef     KERNEL_H
#define     KERNEL_H
#define     KER_DATA_SEC    0x8
#define     KER_CODE_SEC    0x10
#define     MEM_MAP_ADDR    0x200000
#define     KPDR            0x111000
//进程开始地址
#define     P_START         0x401000
#define     TSS_START       0x400000
#define     TSS_SIZE        0x68
/*
 *  嗯,由于下面这些内容在汇编代码中会出现问题,所以他们加上了使用条件,当然汇编代
 *  码还是有自己的事要处理,就是#define  ASM_SRC
 */

#ifndef ASM_SRC
#include    <mem.h>
#include    <task.h>
#include    <trap.h>
#include    <io.h>
typedef struct{
    unsigned    char    cur_x;
    unsigned    char    cur_y;
    unsigned    int     mem_size; 
    /*
     *  OK这里犯了个大错误,由于早期printk很简单,所以只能传递4字节的参数,
     *  而这里tick是8字节,导致栈错误,导致我找了好久才找到这个问题.
     *  嗯,委屈点吧,先用int,不过到处到留下这种祸害,不知道后果如何,将来规范话的
     *  时候是不是会有很多问题呢,好吧,段正淳也到处洒下种子,后果很严重
     */
    //long long  tick;    //系统开机到现在的滴答数 
    unsigned    int     tick;
    //主内存位图,linux是直接内嵌到内核源码里面,我却不能.因为我打算现在就处理
    //4GB内存的支持,每个页面一字节,4GB就是1MB,如果嵌入内核,那么boot就不能加载
    //内核了.
//    unsigned    char    *mem_map;
}kerT,*kerTp;
//内核数据结构指针
extern  kerT   ker_data;
//请求内存管理进程的PDBR
extern  unsigned    int *tpdr;
//内存管理进程的PDBR
extern  unsigned    int *mpdr;
#endif
#endif
