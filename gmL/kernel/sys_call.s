/*
 *      (c) LuoZH 2011-07
 *      gmL已经有点小复杂了,由于我功力不够,对她的驾驭略显不足了.
 *      系统调用,采用了linux的处理方式.
 *      gmL的系统调用暂时直接收一个参数,但是为了和linux兼容,保持了一直当方式
 */
#include    <gmL/sys.h>
.globl  sys_call
.globl  sys_call_table,NR_sys_calls
ENOSYS  =   -1
bad_sys_call:
    push    $ENOSYS
    jmp     0f
sys_call:
    push    %ds
    push    %es
    push    %fs
    push    %eax
    /*
       *    系统调用最多传3个参数,分别用edx,ecx,ebx传递,这里入栈是为了让
       *    DS,ES指向内核段,FS指向用户段
       */
    push    %edx
    push    %ecx
    push    %ebx
    mov     $0x8,%edx
    mov     %edx,%ds
    mov     %edx,%es
    mov     $0x1b,%edx
    mov     %edx,%fs
    
    /*
       *    检测系统调用范围
       */
    cmp     NR_sys_calls,%eax
    jae     bad_sys_call

    /*
       *    执行相应的系统调用C程序
       *    系统调用只返回一个值,用eax传递
       */
    call    *sys_call_table(,%eax,4)
    push    %eax
0:
    pop     %eax
    pop     %ebx
    pop     %ecx
    pop     %edx
    addl    $4,%esp
    pop     %fs
    pop     %es
    pop     %ds
    iret
