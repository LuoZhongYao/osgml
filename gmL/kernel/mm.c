/*
 *      2011-07-29  LuoZH
 *      好几天没有碰gmL,主要是遇见一个纠结问题,内存管理和进程管理的协调问题,这个
 *  问题跟以前遇见的C语言头文件包含问题如出一则,进程创建的时候要创建新的空间,所
 *  以需要mm的协助,但是进程管理是内核,mm是一个进程,内核需要进程的协作来完成,这有
 *  有多纠结,可想而知.为此我还是决定牺牲一点系统的纯洁性来完成系统的协调和简单性
 *  ,狠心将将mm归入内核.虽然我很讨厌这样,但是这是个好主意.good idea.毕竟意淫一个
 *  美丽的系统跟很一个系统确实发生关系,我还是倾向于后者.
 */
/*
 *      对4GB内存的映射做以下这些规定
 *      0~4KB固定映射PDE
 *      4~8KB固定映射前4MB的PTE
 *      8~12KB mm内部使用
 *      12~32kb特殊用途保留供内核使用
 */
/*
 *      mm部分模型很简单,但是涉及到物理地址的访问,所以比较难理解,如果对这部分有
 *  很好的实现,欢迎联系我
 *  Email:LuoZhongYao@gmail.com
 */
#include    <gmL/kernel.h>
#include    <x86/trap.h>
#include    <gmL/mm.h>
#include    <gmL/system.h>
#include    <dgio.h>
#include    <string.h>
#include    <stddef.h>
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
    for(int i=0x400;i<mm_size;i++)
        mm_map[i] = 0;
    HMEM = mm_size<<12;
    LMEM = 0x400000;
    //安装页中断
    install_int(14,page_fault,KER_CODE_SEC,TRA_GATE|IDT_R0);
}
/*
 *  分配物理页,如果分配成果返回物理页地址,失败则执行页面交换,并重新查找,也就是
 *  该函数一定返回正确,如果不返回则系统崩盘
 */
unsigned get_free_page(void){
    while(1){
       for(unsigned i=0;i<mm_size;i++)
         if(mm_map[i]==0){
             mm_map[i]=1;
             return  (i<<12);
         }
       //页面用完了,则进行页面交换处理,由于现在还没有实现,所以暂时将使系统停止
       //----------页面交换处理----------------
    }
}
void free_page(unsigned page){
    if((page<HMEM)&&(page>LMEM)&&mm_map[page]){
        mm_map[page>>12]--; 
    }
}
/*
 *  进程空间的前面4KB永远映射到页目录
 *  4KB~8KB永远映射前4MB的PTE
 */
int put_page(unsigned addr,unsigned page){
    unsigned    *const pde=(unsigned *)0;   //指向PDE
    unsigned    *const pte=(unsigned *)0x1000;  //指向前4MB的PTE
    unsigned    *const opt_page=(unsigned *)0x2000; //指向当前操作页
    //如果没有分配物理页,返回错误
    if(mm_map[page>>12]==0)
        return  -2;
    //如果映射覆盖了PTE,PDE,返回出错
    if(addr<0x2000)
        return  -1;
    //判断是否分配了该线性地址的PTE,如果没有,则分配,并设置相应的PTE
    if(!(pde[addr>>22]&0x1)){
        unsigned    tmp;
        tmp=get_free_page();
        //在PDE中设置PTE
        pde[addr>>22]=tmp|7;
        //将PTE映射到8~12KB处才能对其进行读写
        pte[2]=tmp|7;//pde[addr>>22];
        //将该PTE中的页面都设置为不存在
        for(int i=0;i<1024;i++)
            opt_page[i]=0;
        refpage();
    }
    //将操作页放入0x2000处
    pte[2]=pde[addr>>22];
    refpage();
    opt_page[(addr>>12)&0x3ff]=page|7;
    refpage();
    return  0;
}
/*
 *  P:Hi,老板,来4GB money 用用呗.
 *  K:拿,先拿支票去,用的时候去银行取.
 *  P:谢谢老板.
 */
/*
 *  线性地址前面8KB留着mm使用,因为mm要操作不存在的地址,只能把他们送救助站,然后
 *  赫赫...
 *  0~4KB:当前进程目录
 *  4~8KB:新目录
 *  前面4MB最后一个页面用做内核栈,不共享
 */
/*
 *  复制页目录
 *  返回新目录的物理地址
 *  复制指定页面到新页面
 */
unsigned    copy_page(unsigned old_page){
    unsigned    new_page;
    void    *const opt_page=(void *)0x3000;
    new_page=get_free_page();
    put_page(0x3000,new_page);
    put_page(0x4000,old_page);
    memcpy(opt_page,(void *)0x4000,0x1000);
    return  new_page;
}
/*
 *  函数比较另类,如果成功,返回新的页目录地址,否则返回0
 */
unsigned    create_space(void){
    unsigned    *const pde=(unsigned*)0;
    unsigned    *const pte=(unsigned*)0x1000;
    unsigned    *const opt_page=(unsigned *)0x3000; //12KB~32KB内核可以自由映射
    unsigned    child_pde,child_pte,child_stack;
    //将原来的页面共享都增加1,并设置写保护
    for(int i=1;i<1024;i++){
        if(pde[i]&0x1){
            //将PTE放置到opt_page这样才能读取它
            //由于put_page会自动屏蔽低12位,所以这里我们不用自己屏蔽
            put_page(0x3000,pde[i]);
            for(int j=0;j<1024;j++){
                if(opt_page[j]&0x1){
                    mm_map[opt_page[j]>>12]++;
                    opt_page[j]&=0xfffffffd;
                }
            }
            mm_map[pde[i]>>12]++;
            pde[i]&=0xfffffffd;
        }
    }
    //进行页目录的复制,并设置第一项的PTE
    child_pde=copy_page(pte[0]);
    child_pte=copy_page(pte[1]);
    child_stack=copy_page(pte[1023]);
    put_page(0x3000,child_pde);
    opt_page[0]=child_pte|7;
    put_page(0x3000,child_pte);
    opt_page[0]=child_pde|3;
    opt_page[1]=child_pte|3;
    opt_page[1023]=child_stack|3;
    return  child_pde;
}
/*
 *  页不存在处理
 */
void do_no_page(unsigned error,unsigned addr){
    int page;
    page=get_free_page();
    put_page(addr,page);
}
unsigned un_wp_page(unsigned page){
    if(mm_map[page>>12]>1){
        mm_map[page>>12]--;
        //复制老页面到新页面
        page=copy_page(page);
    }
    return page;
}
//页写保护
//do_wp_page的当前操作页本来使用的是0x3000,但是cpoy_page也使用这页,为了防止冲突,使用
//了0x4000,以后这里可能还要进行一次大规模的调整
void do_wp_page(unsigned error,unsigned addr){
//    printk("mm.c->do_wp_page:Write!\n");
    unsigned *const pde=(unsigned *)0;
    unsigned *const opt_page=(unsigned *)0x5000;
    unsigned page;
    if(addr<LMEM)
        return;
    //如果页表项也写保护,取消写保护
    if(!(pde[addr>>22]&2)){
        page=pde[addr>>22];
        page=un_wp_page(page);
        pde[addr>>22]=page|7;
        refpage();
    }
    put_page(0x5000,pde[addr>>22]);
    if(!(opt_page[(addr>>12)&0x3ff]&0x2)){
        page=opt_page[(addr>>12)&0x3ff];
        printk("mm.c:do_wp_page:\npage:\e\n",page);//pde[addr>>22]);
        page=un_wp_page(page);
        opt_page[(addr>>12)&0x3ff]=page|7;
    }
    refpage();
}
void do_sys_page(unsigned error,unsigned addr){
    printk("mm.c->do_sys_page:Don't access system page!\n");
}
