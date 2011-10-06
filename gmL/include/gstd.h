/*
 *      gmL系统标准头文件,主要定义系统调用号和调用宏
 */
#ifndef GMLSTD_H
#define GMLSTD_H
#define REG_MSG     0   //进程绑定消息
#define SEND_MSG    1   //发送消息
#define RECEIV_MSG  2   //从消息队列取得消息
#define FORK        3   //不解释
extern  void sys_call(void);    //kernel/sys_call.s
#define syscall(nr,arg1,arg2,arg3) ({\
    int _reg;           \
    asm(                \
        "int $0x80\n\t" \
        :"=a"(_reg)     \
        :"b"(arg1),"c"(arg2),"d"(arg3),"a"(nr)\
       );       \
    _reg;       \
})
#define fork()              (syscall(FORK,0,0,0))
#define send(msg)           (syscall(SEND_MSG,msg,0,0))
#define receiv()            (syscall(RECEIV_MSG,0,0,0))
#define reg_msg(thread,msg) (syscall(REG_MSG,thread,msg,0))
#endif
