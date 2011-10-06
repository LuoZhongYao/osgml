
/*
 *  (c) LuoZH   2011-06
 *      不得不承认,linux那种将内核映射到3GB开始处很有效,这样内核就属于其他进程的
 *  一个子部分,这我也在setup里面提到,但是现在我才意识到这种方法对我这个系统不是
 *  很有效,我决定采用自己的方法,就是将内核当着一个进程来处理,她独立于其他进程.
 *  不过它有别于其他的进程,内核进程的地址空间就是整个物理内存,其他进程有内核分配
 *  ,换句话说,其他进程只是内核的一个数据而言.
 *
 */
#include    <gmL/kernel.h>
#include    <gmL/thread.h>
#include    <gmL/sched.h>
#include    <gmL/sys.h>
#include    <gmL/system.h>
#include    <gmL/mm.h>
#include    <x86/trap.h>
#include    <gstd.h>
#include    <x86/io.h>
#include    <string.h>
#include    <dgio.h>
/*
 *  首先感谢setup同学为我们将要出来表演的哥们准备好了一切,那么,现在的工作看起来
 *  很有趣了,不仅仅是因为我们可以使用C语言,更重要的是我们把几个不是那么对胃口的
 *  东西都让setup完美解决了.
 *      内核从0X200000开始存放,但是她的空间再次被收刮了,我不是贪官污吏,而是她
 *  闺密要和她一起玩,虽然这个闺密很简单,很单纯,不过体积却简单.她要使用1MB的空间
 *  也就是kernel又被压缩了1MB,不过还够用.这个闺密就是mem_map,主内存位图.她存放于
 *  内核4MB空间中的最后一MB,不是说内核有1GB的空间吗,为什么只有4MB,额,这个...
 *      由于策略的改变,内核拥有了整个物理内存空间,不过他并不是丧心病狂的完全使用
 *  而是合理分配给其他人使用,由于setup只是映射了前面4MB,现在我们要进行计算,必要
 *  要的时候在将内存映射.
 *      各个进程可以是独立的互不干扰的,但是有个进程却是特殊的.那就是内核,它需要
 *  和其他进程交换一些数据,所以我们将部分页面完全共享,并且对等映射
 */
kerT    ker_data;
//系统滴答
int     tick;
void test(void);
void dea(void);
int main(void){
    ker_data.cur_x=(*(unsigned short *)0x100800)&0xFF; 
    ker_data.cur_y=((*(unsigned short *)0x100800)>>8)&0xFF; 
    tick=0;
    printk("gmL start...\n");
    trap_init();     //初始化中断,系统调用
    mm_init();      //内存管理模块初始化
    sched_init();
    //sti();
    move_user_mode();
    if(fork()!=0){
        if(fork()!=0){
            printk("This sever reg is \e!", reg_msg(cur_thread,1));
            int i=0;
            while(1){
                for(int sleep=100;sleep;sleep--);
                i=receiv();
                printk("MSG:->\e\n",i);
            }
        }
        printk("clent!");
        int i=0;
        while(1){
            i++;
            send(1);
            if(i%10)
                send(2);
        }
    }
    int i=0;
    reg_msg(cur_thread,2);
    while(1){
        i=receiv();
        printk("MSG:\e\n",i);
    }
    asm("wait");
}
