/*
 *   (c) 2011-07-12 LuoZH
 *   终于开始写调度相关的代码了,按照我的标准,只要调度和内存管理这部分处理完善,系
 *  统就可以很好运行了,而且后继部分就可以很轻松完成.只是现在对调度部分不像内存管
 *  理部分一样心里有一个很清晰的轮廓,所以结构就先不说了.
 */
#include    <gmL/kernel.h>
#include    <gmL/sched.h>
#include    <gmL/thread.h>
#include    <dgio.h>
#define     TSS_ADDR    0x100800
/*
 *      这里以后的很多地方都要特别注意了,因为没有malloc函数,这个进程是直接操作
 *  内存的,本来可以用数组代替malloc,但是这样会增加system模块的长度.这些地方我会
 *  打上特殊标记,等malloc规划好了在过来修改掉.
 *      进程调度模块暂时不会超过4KB,把她当着一个任务,开销大得很啊.
 */
/*
 *  看起来有点问题,如果进程队列里面没有进程怎么办呢?,好吧,设置一个I'm dea!进程.
 *  不过我考虑能不能将dea进程设置成一个后台优化进程,如果CPU实在太闲就进行内存,
 *  文件系统之类的优化操作.看起来不错,不过还是先简单的I'm dea!吧.
 */
/*
 *  由于几个原因,进程切换使用软件来实现吧.
 *  进程的切换没有什么了不起的,仅仅只是cr3和esp的更新而已,仅此而已
 *  不过这里却很难处理,我不想用汇编,但是又不能在用栈了.纠结啊
 */
/*
 *  2011-08-07
 *      进程调度和fork花费了我很长时间,但是一点进展都没有.现在又得继续更新了.
 *  另外可能要面临失恋了.
 *      好吧,我记得我跟我兄弟说过,失恋这种事每个月都有几次,习惯就好.失恋又如割
 *  包皮,虽然很痛,但是还是有好处.不过能不能失恋还是不确定,只是感觉女朋友对我印
 *  象不是很给力.
 */
/*
 *      感觉她逐渐在疏远我,我这个人着实是一个悲剧,当好的时候,什么都号,差的时候
 *  什么都差,关系变差了,gmL一点进展都没有,如果感情的事情我无法操控,gmL的进度我不
 *  会放弃,1.0版一定会发布的.
 */
//run_space本来是作为sched的一个返回值来使用的,但是sched的上层是汇编,除了cr3
//要返回,还有esp要返回,如果返回两个值,对齐问题在汇编代码里处理很不方便,所以使用
//这个全局变量.
unsigned    run_space=0xABCEDF;
unsigned sched(unsigned esp){
    while(1){
        next_thread++;
        next_thread%=thread_size;
        if(thread_list[next_thread].state==ACT)
            break;

    }
    //printk("sched.c->sched:next_thred=\e\n",next_thread);
    thread_list[cur_thread].esp=esp;
    run_space=thread_list[next_thread].PDBR;
    //很神奇,如果不添加这句,某些语句就会出错,比如上面,这个问题以前页出现过,但是
    //这种问题最纠结,调试都不便
//    printk("\e\n",run_space);
    cur_thread=next_thread;
    return  thread_list[next_thread].esp;
}
void sched_init(void){
    unsigned int tr=0x28;
    pid_t   pid;
    pid=get_free_thread();
    printk("sched.c:sched_init:\e\n",pid);
    thread_list[pid].PDBR=KPDR;
    thread_list[pid].pid=pid;
    thread_list[pid].state=ACT;
    cur_thread=pid;
    next_thread = pid;
    tssTp   tss=(tssTp)TSS_ADDR;
    tss->SS0=0x08;
    tss->ESP0=0x3FFFFF;
    tss->LDT=0;
    tss->IO=0xFFFF0000;
    tss->EFLAGS=0x200;
    asm( "ltr %0\n\t"::"m"(tr));
}
