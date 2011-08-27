#include    <gmL/thread.h>
#include    <gmL/mm.h>
#include    <gmL/system.h>
/*
 *      本来fork我打算创建新的空间,这样来完成新进程,也就是复制一份PDBR,但是不得
 *  不解决的一个问题是,复制PDBR应该是mm的事情,mm我又是以一个进程的方式实现的,这
 *  ,fork是内核的一个系统调用,怎么能使用mm的功能呢.这个问题纠结了我很久,今天想到
 *  这个解决方案,就是fork并不分配PDBR,而仅仅是复制一份进程表,这样两个进程完全一
 *  致,设置页目录也是共享的,这样,当子进程或者父进程谁先访问内存的时候由于页写保
 *  护而调用mm来处理.问题还是有点纠结,设置页面写保护还是mm的事情,这又该如何是好.
 *  这很想某位前辈写的windows DLL HELL问题啊,解决老问题带来了新的问题,但是还是同
 *  一个问题.解决方案就是fork发送一个请求让mm来设置共享.这还跟前面那个问题又是同
 *  一个问题,就是内核依赖进程,很矛盾.
 */
pid_t fork(void){
    //锁定防止此时被调度出去, 这个完善的时候将被放入进程状态,而不是这样使用
    pid_t   pid;
    pid=get_free_thread();
    thread_list[pid]=thread_list[cur_thread];
    thread_list[pid].state = WAIT;
    //下面应该是发送申请空间的请求,等待实现
    thread_list[pid].pid=pid;
    thread_list[pid].fpid=thread_list[cur_thread].pid;
    //为新进程设置上下文
    thread_list[pid].PDBR=create_space();
    asm(
        "mov    %%esp,%%ecx\n\t"
        "mov    %1,%%cr3\n\t"
        "push   %%ss\n\t"
        "push   %%ecx\n\t"
        "pushfl\n\t"
        "push   %%cs\n\t"
        "push   $0f\n\t"
        "pushal\n\t"
        "push   %%ds\n\t"
        "push   %%es\n\t"
        "push   %%fs\n\t"
        "push   %%gs\n\t"
        "mov    %2,%%cr3\n\t"
        "mov    %%esp,%0\n\t"
        "mov    %%ecx,%%esp\n\t"
        "jmp    1f\n\t"
        "0:\n\t"
        "mov    $0,%%eax\n\t"
        "leave\n\t"
        "ret\n\t"
        "1:"
        :"=a"(thread_list[pid].esp)
        :"r"(thread_list[pid].PDBR),"r"(thread_list[cur_thread].PDBR)
        :"ecx");
    //printk("fork.c->fork:show\n");
    //这个不是很给力
    thread_list[pid].state = ACT;
    return  pid;
}
