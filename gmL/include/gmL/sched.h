/*
 *  (c) 2011-07-12  LuoZH
 *  sched是一个独立的进程块,不过它比较特殊,她被编译到了system模块,然后作为第一个
 *  进程来使用,并且她的TSS段也比较特殊,她自己拥有自己的一个TSS,其他任务共同拥有
 *  一个TSS,也就是系统里面只有2个TSS,其他任务的TSS通过分页而独立,这样就不会导致
 *  冲突,但是我们必须要使用一个不同的TSS,不然会发生异常,所以sched拥有一个独立的
 *  TSS,所以在任务切换的时候要提前加载页目录,由于这个原因,sched进程要被其他进程
 *  共享,这本来是个糟糕的设计,如果可以的话,可能考虑将sched做为系统的一部分来实现
 *  而不是一个进程
 */
#ifndef SCHED_H
#define SCHED_H
unsigned sched(unsigned esp);
//void init_thread(unsigned,unsigned,void (*)(void));
void sched_init(void);
//#define do_switch(PDBR) asm("mov %0,%%cr3\n\t"::"a"(PDBR));
#endif
