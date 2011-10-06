/*
 *      (c) LuoZH   2011
 *  进程(任务)相关数据结构定义,现在仅仅是为了实现时钟中断任务而设计
 *      本来我在考虑,是不是该把所有有关描述符相关的处理到单独用一个文件来处理,
 *  像现在这样各个部分散落民间是不是很不好,不过,仔细考虑,还是各种描述符放在各自
 *  负责的区域去,就像媳妇还是要住在家里,而不是娘家
 */
#ifndef THREAD_H
#define THREAD_H
#include    <gmL/msg.h>
/*
 *      LDT描述符应该是内核的问题,这里我又需要,所以必须在定义,我不知道当初把描述
 *  符交给使用的程序自己处理是不是一个明智的选择.
 */
/*
typedef struct{
    unsigned    short   b0;
    unsigned    short   b1;
    unsigned    short   b2;
    unsigned    short   b3;
}ldtT,*ldtTp;
*/
/*
 *  ldt中的段属性
 */
/*
#define     LDT_R0  0x8000
#define     LDT_R1  0xA000
#define     LDT_R2  0xC000
#define     LDT_R3  0xE000
#define     CODE_G  0x0080
#define     CODE_D  0x0040
#define     CODE_R  0x9A00
#define     CODE_C  0x9C00
#define     DATA_G  0x0080
#define     DATA_B  0x0040
#define     DATA_E  0x9400
#define     DATA_W  0x9200
*/
/*
 *  本来这个描述符的各个部分应该单独出来各自处理,但是考虑到我对gnu处理字节对齐
 *  问题不是很清楚,为了防止对齐破坏了描述,只能使用相同大小的类型了,所以描述符
 *  需要特殊函数访问,不知道这叫不叫面对对象呢?
 */
/*
typedef struct{
    unsigned    short   limit;       //段限长低16位 
    unsigned    short   base;        //段基址低16位
    unsigned    short   type;       //这部分包括基址的中8位,和一部分表示描述符\
                                  特性的一些位,不过我还是将就使用type这个变量名
    unsigned    short   base1;      //这是个更复杂的混血儿,拜兼容所赐
}tss_secT,*tss_secTp;
*/
//属性相关
//TSS描述符跟其他文件里的描述符不同,该描述符必须指定R0~R3中的一个,不指定将会出错
/*
#define     TSS_G   0x0080      //颗粒度   
#define     TSS_R0  0x8900      //
#define     TSS_R1  0xA900  
#define     TSS_R2  0xC900      
#define     TSS_R3  0xE900
*/
//TSS数据结构
typedef struct{
    int BLACK_LINK;      //上一任务链接,只使用低16位
    int ESP0;           //
    int SS0;
    int ESP1;
    int SS1;
    int ESP2;
    int SS2;
    int PDBR;
    int EIP;
    int EFLAGS;
    int EAX;
    int ECX;
    int EDX;
    int EBX;
    int ESP;
    int EBP;
    int ESI;
    int EDI;
    int ES;
    int CS;
    int SS;
    int DS;
    int FS;
    int GS;
    int LDT;
    int IO;
}tssT,*tssTp;
/*
 *      我不确定这么搞是否恰当,毕竟有很多进程可能需要在IDT里面存放一些其他类型的
 *  描述符,比如门之类的,不过,这些都是引用GDT里面的项,为什么不直接引用呢?上面说LD
 *  T是3项,现在我另外加一项,该项用来处理进程通信,因为文件系统之类的都被我T出内核
 *  ,将来我打算用共享内存处理通信问题,预先留一个项给他.
 */
//#define MSG_LEN 1
typedef int pid_t;
//进程状态
typedef enum{
    ACT     =   0,      //活动状态
    SLEEP   =   1,      //大瞌睡
    WAIT    =   2,      //等待资源
    NEW, 
}pstateT;
typedef struct{
    //进程ID
    pid_t   pid;
    pid_t   fpid;
    //进程空间
    unsigned int PDBR;
    //进程状态
    pstateT state;
    int tick;
    //进程命令系列
    msgT msg;
    //进程栈
    unsigned    int esp;
}threadT,*threadTp;
#define THREAD_LEN    0x200
extern  threadT thread_list[THREAD_LEN];
extern  pid_t   cur_thread;
extern  pid_t   next_thread;
extern  int   thread_size;
pid_t   get_free_thread(void);
#endif
