/*
 *      (c) LuoZH   2011-07
 *  内存管理主要的程序.
 *  本程序包含了物理内存的初始化,内存占用分配,内存映射的处理.当然,我还没有写,所
 *  开头这段注释很难下手
 */
/*
 *      初始化物理内存位图,程序首先根据setup模块里面获得的扩展内存大小,将mem_map
 *  数组初始化为未占用,也就是位图字节赋值为0,这个参照linux0.12的处理方式,程序假
 *  设,由setup获得的内存均可供OS使用,这虽然不是很好的假设,但是处理内存空洞的确很
 *  麻烦,初期还是不要考虑了,针对这个问题将来可能造成严重的影响,我尽力将这这个问
 *  控制在初始话部分,这样修改就只要修改初始化部分,可以最大化减小损失.而后,程序将
 *  根据内核的占用情况将mem_map中相应位置置占用.另外,分页的中断也在这里安装
 */
/*
 *      昨天应该很有趣的一天,在初中同学建的初中群里面.其中一个老同学问我们以前
 *  班主任的QQ是多少,依我一向的行事风格,很有必要弄一个QQ糊弄他一下.我的确这么做
 *  了,当然,纯粹的申请一个QQ是很难有什么成就的,所以我也没抱多大的希望,可是,没想
 *  到,那家伙竟然马上就猜测是我现申请个QQ糊弄他,靠,这么容易就被发现,我还混个毛
 *  啊.所以必须弄点照片什么的.不过毕业就没有跟那个老玩童联系了,怎么才能搞到照片
 *  呢?本着试试的心态,用google搜索一下他的姓名,我没有抱任何希望,因为我们能那个穷
 *  乡癖壤,加上他也算古董级别的人物,应该很难在互联网上有所作为吧.不过,我一向主张
 *  试试更健康.不试不知道,一试偷偷笑,原来老家伙也是个新潮人士,竟然在网易有个blog
 *  ,更为重要的是还是用自己的照片做头像,相册还有一张7寸照,不过他竟然用自己的真实
 *  姓名做昵称,一点隐私安全的意识都没有,这还是充分证明了他还是老了.既然有了相片,
 *  速度将照片换到QQ头像上,并且根据blog的里面的签名什么的搞到QQ上,copy几篇blog到
 *  QQ日志.o(∩∩)o...哈哈,够淫荡吧.一切准备就绪,我什么也没有说,那家伙果断就行了.
 *  并且跟我预料的一样,群里的兄弟姐妹父老乡亲们速度的就加我QQ了,更为重要的是.管
 *  理员还迅速将我拉进群.太他妈对得起我啦.为了不露馅,果断先下线.值得注意的是,
 *  我个人对30MB的QQ没有什么好感,所以一直使用的是WEBQQ,这次也是这样.所以柯老万
 *  截我在线的图的时候前面有个WEBQQ的logo.就整个群里来说,只有我一个人使用WEBQQ,
 *  我以为他们应该很容易就根据截图看出破绽,不过,我高估他们了.到了晚上,某最加拍当
 *  ,我最好的兄弟,锋哥上线,作为最好的兄弟,这等好事当然要跟他分享啦.锋哥跟我可谓
 *  臭味相投啊,马上就上道.遗憾的是在计算机方面他始终没有跟上,理想状态,我是期望我
 *  们能一起在计算机里面一起发挥我们的才能.好的,废话不多说,晚上,当该在线的都在线
 *  部分不该在线的也在线的情况下,该我这个班主任登场啦,鞭炮掌声在哪里?oh yeah.之
 *  前,由于一时高兴,拿班主任开了点玩笑,所以当"班主任"上线的时候,我就可以借这个
 *  理由"逃跑",当然,这不是必须的,只是一个人同时扮演2个角色虽然对我来说没什么难度
 *  ,但是做人还是低调点好.接下来当然就是一阵的老师好这种形式主义,看得我都心烦.接
 *  下来,先找几个同学来开刷一下,当然,为了顾全大局,不至于伤害其他老同学的心,我只
 *  能拿我另一个好兄弟开刀了,馒头辛苦啦.当然,锋哥,馒头,还有我,三个作为失败的典型
 *  简直太成功的三个"好学生"来说,很有必要拿来开刷一下,看来只能牺牲自己娱乐大众了
 *  所以我自己也被自己开刷了一下下.后来大家提到什么时候聚聚,有的提议去贵阳,这
 *  部分人多数在贵阳,所以他们提议去贵阳,可见,人自私是一种本能.还有部分提议去昆明
 *  作为一个 长期以搞笑为拿手好戏的我当然不会错过我进初一那天班主任就说过的带我
 *  们去春游,但是到现在,我都快结婚的人了还没实现的这个承诺,我自然要拿来TIAO TAN
 *  一番.哈哈,火候快到了,不能聊得太多,不然会露馅的,不然以后没得玩,所以果断借故
 *  下线,换正统QQ进去.
 *      不过话又说回来,我看到部分老同学的签名,表示看到班主任能进群,很惊讶,对于她
 *  们在外面打工,生活本来就很无趣,有很无奈,突然能看到自己的老师在群里,心里自然有
 *  些惊喜,不知道知道真相后会是怎样的失落.在这里先像你们道歉先.当然当作你们的面
 *  我是不会道歉的,不然那就不是尧哥我的风格了.
 */
/*
 *      由于我现在将内核独立成一个进程,并且各个进程拥有自己的页目录,这意味着.这
 *  里的页跟实际要处理的页并不是同一个页,所以我们要在设置一个变量保存发生故障的
 *  进程的页目录,也就是我们要保存进程的CR3.
 *
 */
#include    <kernel.h>
#include    <string.h>
#define     TRAP_PDBR   0x1000
#define     TRAP_PDER   0x2000
#define     CHILD_PDBR  0x3000
#define     CHILD_PDER  0x4000
//高端内存
unsigned int HMEM    =   0;
unsigned int LMEM    =   0;
unsigned char *mem_map;
unsigned int  mem_size;
static int get_free_page(void);
static int put_page(unsigned int addr,int page);
static int mm_put_page(unsigned int addr,int page);
int map_addr(unsigned int addr);
//在init/int.S里面
void    page_fault(void);
void    do_no_page(unsigned int error,unsigned int addr){
    if((addr>LMEM)&&(addr<HMEM))
        map_addr(addr);
}
void    mem_init(void){
    //由于受内核大小的影响,mem_map数组不能像linux0.12那样直接包含在内核数据段里
    //面,这里我将mem_map放在内核空间的第4MB处,占用1MB空间.从程序设计的角度这是个
    //糟糕的做法,但对于我来说,这是个有效的方法.
    mem_size=*(unsigned*)0x100804;
    mem_map=(unsigned char *)MEM_MAP_ADDR;
    mpdr=(unsigned int *)KPDR;
    //初始化内存占用位图
    //将所有内存设置位占用,占用方式山寨LINUX
    for(int i=0;i<0x100000;i++){
        mem_map[i]=0x1;
    }
    //将主内存区设置为未占用
    //内存前面4MB已经被内核使用了,所以从4MB开始设置
    unsigned int mem_len=mem_size>>2;
    for(unsigned int i=0x800;i<mem_len;i++){
        mem_map[i]=0x0;
    }
    //暂时将高端内存假设为4GB-1MB吧.什么,这个也能假设的?呵呵呵..这个以后再谈,
    //不要在问了,别问.马科长就是这么说的
    HMEM=mem_size<<12;
    LMEM=0x100000;
    install_int(14,page_fault,KER_CODE_SEC,INT_GATE|IDT_R0);
}
/*
 *      程序扫描mem_map,从中查找为零的页,如果找到,返回页号,没有找到返回-1.返回-1
 *  这里还是有点小虚的,因为万一溢出怎么办?不过,32为系统上,机器字宽是2^32,扣除一
 *  半,也能表示2GB,而每个页是4KB,4KB*2GB,嗯,这个内存够大的了吧.而它的老想好就要
 *  幸苦点了,她还得判断是不是内核页面,还有高端内存.然后判断该页是不是是不是为0.
 *  为0则说明释放一个空页,内核上有错误,本来应该死机的.但是 死机我还不会
 */
int get_free_page(void){
    int mem_page_size=mem_size>>2;
    for(int i=0;i<mem_page_size;i++){
        if(!mem_map[i]){
            mem_map[i]=1;
            return i;
        }
    }
    return  -1;
}
/*
 *      上面哪位同志的老相好,负责页面的释放,这里的页面指的是物理内存,程序首先
 *  判断释放的页面是不是高端内存,至于高端内存有多高这个问题还有有待商榷,另外低
 *  1MB的内存并不在内存映射位图里面,所以不用判断.至于如果释放的是内核页面怎么办
 *  呢?这个问题实际上被我弄得上次去了,这也该是上层的问题,上层指的是释放线性地
 *  址哪里,把判断是否释放的是内核页面提升到释放线性地址去判断,处理方便,并且逻辑
 *  上更合理,至于为什么,有兴趣的可以私下跟我讨论,当然最好还是带点礼物什么的
 */
int free_page(int   page){
    if((page>=(HMEM<<12))||(page<=(LMEM<<12))){
        //死机
        return  -1;
    }
    else if(!mem_map[page]){
        //死机
        return  -1;
    }
    else 
        mem_map[page]--;
    return  0;
}
//这个是早期的管理,现在又暂时的不能删除它,所以为了防止冲突先条件编译它
#ifdef  MM
int put_table(unsigned int addr){
    int page;
    unsigned int table;
    page=get_free_page();
    if(page==-1)
        return  -1;
}
/*
 *  程序将线性地址addr映射到物理页page上
 *  嗯程序里面存在一个问题,就是当页表项不在内存的时候,我们要为它分配一个页表项
 *  但是这里面有个问题,分配的空间并没有映射,当将页信息写入页表项的时候,就会再次
 *  产生页异常,从而进入死循环,这里我考虑先用线性地址0~4KB这段做为临时映射用,因为
 *  第0个页目录是已经存在了的,所以不会出现问题.这个方法不是很合理,但是用来测试这
 *  页问题还是比较好.
 */
int put_page(unsigned int addr){
    unsigned    int tmp,*page_table;
    int page;
    //如果页目录项不存在,为其分配
    if(!ker_data.kpdr[addr>>22]&0x1){
        if(-1==(tmp=get_free_page()))
            return  -1;
        ker_data.kpdr[addr>>22]=tmp<<12|7;    
    }
    page=get_free_page();
    if(page==-1)
        return  -1;
    //界限判断
    if((page>=(HMEM>>12))||(page<=(LMEM>>12)))
        return  -1;
    ((unsigned int *)(ker_data.kpdr[0]&0xFFFFF000))[0]
            =ker_data.kpdr[addr>>22];
    ((unsigned int *)0)[(addr>>12)&0x3FF]=page<<12|7;
    return  0;    
}
#else
/*
 *      该函数的功能就是释放所有映射了的内存,用于进程结束时的内存释放.
 *  函数扫描传入的页目录项,找到所有映射的内存,进行释放操作,然后释放相应的页表占
 *  用空间,最后释放页目录占用空间
 *      开启分页后有很多问题的确很复杂,比如现在,这个page是物理地址,可能没有映射
 *  那怎么能够读取它的数据呢?我了X,速度将线性0挂接上.
 */
int free_mm(unsigned int *page){

    for(int i=0;i<1024;i++){
        if(page[i]&0x1){
            unsigned    int *table;
            table=(unsigned int *)(page[i]&0xFFFFF000);
            for(int j=0;j<1024;j++){
                if(table[j]&0x1){
                    free_page(table[j]>>12);
                }
            }
            free_page(page[i]>>12);
        }
    }
    free_page((int)page>>12);
}
/*
 *      重新考虑下内存管理.
 *  首先,0~4MB内存用来存放内存管理的数据,包括PDBR,GDT,IDT,并且中断程序也在这部分
 *  .这部分数据说所有进程共享的,但是只有运行在系统级的时候可以写.并且这段地址是
 *  对等映射.进程从0X401000开始存放,只到4GB,而0X400000~0x401000这段用来存放进程
 *  自己的TSS.第一个进程是内存管理进程,由于内存管理进程没有运行,所以第一个进程的
 *  空间也是对等映射的.并且这个进程一直运行于系统级,所以它只有一个栈.当第一个内
 *  存管理进程运行后,其他进程的地址就有内存管理动态分配.由于进程使用个各自的页目
 *  录,他们之间是相互隔离的,但是进程管理部分却要操作其他进程的页目录和页表,所以
 *  人为规定内存管理的线性地址0~4KB映射了,请求管理进程的页目录,4KB~8KB映射请求
 *  管理进程当前请求的页表,当前请求请求进程进程的PDBR由中断加载到0~4MB范围内的
 *  tpdr变量里面供内存管理使用.8KB~12KB用来在进程创建的时候存放子进程的PDBR
 *      这里对上面描述做一些改变:
 *      0~4KB留作mm自己使用.
 *      4~8KB供申请进程存放PDBR.
 *      8~12KB存放申请进程的PDER.
 *      12~16KB存放申请进程子进程的PDBR.
 *      16KB~20KB存放申请进程子进程的PDER.
 */
/*
 *      申请页内存,将请求进程的页目录复制到申请的内存页面,达到内存共享.
 *  出错返回-1;否则返回创建空间页目录的物理地址页
 *  
 */
//
int create_space(void){
    int page;
    unsigned int *tmp;
    unsigned int *table;
    //由于内存管理模块的前8MB是对等映射,所以这里不会出错
    page=get_free_page();
    if(page==-1)
        return  -1;
    mm_put_page(TRAP_PDBR,((int)tpdr)>>12);
    mm_put_page(CHILD_PDBR,page);
    tmp=(unsigned int *)TRAP_PDBR;
    for(int i=1;i<1024;i++){
        if(tmp[i]&0x1){
            mem_map[tmp[i]>>12]++;
            tmp[i]&=0xfffffff9;
            unsigned int *pder;
            mm_put_page(TRAP_PDER,tmp[i]>>12);
            pder=(unsigned int *)TRAP_PDER;
            for(int j=0;j<1024;j++)
                if(pder[j]&0x1){
                    mem_map[pder[j]>>12]++;
                    pder[j]&=0xfffffff9;
                }
        }
    }
    memcpy((void *)CHILD_PDBR,(void *)TRAP_PDBR,0x1000);
    return  page<<12;
}
/*
 *  将指定线性地址映射到内存管理进程空间的物理页
 *  额,好久没有晚上碰代码了,现在脑袋完全是一片空白,白天的代码思路完全不能全部放
 *  在大脑里面,纠结了.
 */
int put_page(unsigned int addr,int page){
    unsigned int *tmp;
    if((page>=(HMEM<<12))||(page<=(LMEM<<12)))
        return  -1;
    if(mem_map[page]!=1)
        return  -1;
    if((addr>=(HMEM))||(addr<=(LMEM)))
        return  -1;
    if(mm_put_page(TRAP_PDBR,(int)tpdr>>12)==-1)
        return -1;
    tmp=(unsigned *)TRAP_PDBR;
    if(!(tmp[(addr>>22)]&0x1)){
        int page_tmp;
        page_tmp=get_free_page();
        if(page_tmp==-1){
            free_page(page);
            return -1;
        }
        tmp[addr>>22]=(page_tmp<<12)|3;
    }
    mm_put_page(TRAP_PDER,tmp[(addr>>22)]>>12);
    tmp=(unsigned *)TRAP_PDER;
    tmp[(addr>>12)&0x3ff]=(page<<12)|7;
    return  0;
}
//linux0.12里面get_empty_page相同功能的函数,不过还是自己写的好
int map_addr(unsigned int addr){
    int page;
    page=get_free_page();
    if(put_page(addr,page)==-1){
        free_page(page);
        return  -1;
    }
    return  0;
}
/*
 *  取消页面保护,由于创建新的内存空间的时候仅仅复制了页目录,也就是页表也是共享的
 *  所以我们在这里要先判断页表共享没有,共享就分配新的的页表,并将老的页表复制到新
 *  的页表上.
 */
//判断页表是否被共享,如果被共享,分配新的页表,复制老页表到新页表,并返回新的页表
//物理地址
/*
 *      写时复制这部分的确很费劲,花了很长时间但只到现在2011-07-10-21:36还在没有
 *  按预期的运行.本来很久晚上我都没有碰代码了,但是我怕时间不多了,为什么我这么说
 *  ,我父亲就是活生生的例子.我不知道那天我也会离开这个世界.死对于我来说跟拉屎是
 *  一个级别的事,但是我却害怕死亡,理由是我还有很多事没有做,比如结婚.当然这么看来
 *  实际上我只有两件事没做,一是结婚,二是我还没有实现我的梦想.gmL就是我梦想的一部
 *  .现在gmL已经有点模样了,我不能像以前做的程序一样都半途而废.所以我必须抓紧我能
 *  预知的时间来完成它.我最大的心愿就是,如果有一天,我父亲的确太想我了,我希望有
 *  人能看到我的这份劳动成果,如果有人能继续帮我完成我没完成的部分.那我想我就是最
 *  幸运的人了,我一向很幸运,但是跟我很亲密的人都会不太幸运.当然,如果我在一天,我
 *  就会好好呵护gmL一天,只到它能达到我心目中的样子.
 *      人大致有两种人,重利益和中名声.我更倾向于后一种.不过大多数人们看到的却是
 *  我淡薄名利,甚至有时候都在毁灭自己的形象.多数时候我对这些看法只是什么安知什么
 *  之志哉.我在乎的名声跟你们不一样而已.这也是我在注释里很多地方都在写我的故事的
 *  原因.我希望有人能够了解我的故事.
 *      很久没有那么严肃的写东西了,呵呵.
 *      今天,下午,小红连带几个老表一起来我家玩,我还是很高兴的.虽然仅仅不过几分钟
 *  她们就要离开,我还是很高兴的.而我母亲却天真的以为他们会一起吃过饭什么的,以至
 *  于,没有问我下就叫我弟去买了牛肉,结果不言而喻.讽刺的是却跟她们一起去她们到她
 *  大妈家去吃饭.这过程也暴露出了我致命的弱点-----人际交往.对于这个问题,其实对我
 *  生活只有一个影响,那就是处对象问题.所以我很怀疑小红连会喜欢我多久,喜欢这个词
 *  是我猜想的.也许也有人跟我有同样的痛苦吧,很想跟自己喜欢的人一起,但是却害怕在
 *  一起.因为在一起自己不知道说些什么而导致感情的疏散.所以我是一个向往恋爱却及其
 *  讨厌恋爱的人.恋爱对我一直都是痛苦的,没有快乐过,哪怕一丁点.所以我更倾向于于大
 *  家见面就谈婚事,互相觉得还行就结婚的人,我对我对象基本没什么要求,除了原则上的.
 *      和他们一起玩到天看不见,我说的了大约20句话吧.这时候刚好他们终于被考虑回去
 *  了.我心里正盘算着路灯刚好到我家哪里,我就可有主动借电筒给她了.但是我立马意识
 *  到.这一定会泡汤,我所有的打算都被意外粉碎了,没有一次例外.所以我像这次一定是在
 *  有路灯到我家这段路上突然蹦出个人来给她电筒.所以我盘算下从脚下到我家这段路上
 *  可能会出现电筒的地方.首先,可能性最高的就是她大妈家,我们必过她大妈家,并且她堂
 *  弟也跟我一块出去玩的,现在也在路上.然后就没有了.当我们经过她大妈家的时候,的确
 *  还和她交谈一会儿,奇怪的是只到我们走了也没谈电筒的事.难道真的要从路上蹦出个人
 *  来送她电筒.时间一点点过去,我们也也来越接近我家,但是电筒还没有出现,难道这次终
 *  于要破例了.时间还在走着,电筒却蹦出来了.的确是蹦出来的.我开始怀疑我的人生.
 */
int un_wp_page(int table){
    int page;
    if(mem_map[table]>1){
        page=get_free_page();
        
        if((mm_put_page(CHILD_PDER,page)==-1)||(mm_put_page(TRAP_PDER,table)==-1)){
            free_page(page);
            return -1;
        }
        mem_map[table]--;
        memcpy((void *)CHILD_PDER,(void *)TRAP_PDER,0x1000);
        table=page;
    }
    return  table;
}
void do_wp_page(unsigned int error,unsigned int addr){
    unsigned int *table;
    int     page;
    if(addr<0x400000){
        printk("Kernel page!\n");
        return;
    }
    //将申请页目录放入TRAP_PDBR
    page=(int)tpdr>>12;
    if(mm_put_page(TRAP_PDBR,page)==-1)
        return;
    table=(unsigned int *)TRAP_PDBR;
    page=table[addr>>22]>>12;
    page=un_wp_page(page);
    if(page==-1)
        return;
    table[addr>>22]=page<<12|7;
    //将申请页放入TRAP_PDER
    if(mm_put_page(TRAP_PDER,page)==-1)
        return;
    table=(unsigned *)TRAP_PDER;
    page=table[(addr>>12)&0x3ff]>>12;
    page=un_wp_page(page);
    if(page==-1)
        return;
    table[(addr>>12)&0x3ff]=page<<12|7;
}
/*
 *      将物理页加载到mm进程的addr地址上.
 *  这个函数跟put_page很相似,但是那个跟这个区别就是,那个加载的是申请进程的线性
 *  地址,而这个加载的是mm线性地址,其实put_page也使用该函数.开始这个函数都是被我
 *  直接在其他函数里面,但是天知道还有多少函数会用到这些呢.还是实现一个函数吧.
 *      另外,mm的前4MB的页表和目录必须一直在内存中,这个其实不用考虑,理由是前4MB
 *  是所有进程共享,而mm的这几项刚好在前4MB里面
 *      出错返回-1,否则返回原线性地址映射的页面,当然如果没有映射页页返回线性地址
 *  对应的页表项值,这两个其实是一个意思
 */
int mm_put_page(unsigned addr,int page){
    unsigned int *table;
    table=(unsigned int *)(mpdr[0]&0xfffffc00);
    //判断page的有效性,mm模块比较特殊,有可能她映射的页已经是映射了的,所以我只判0
    if(mem_map[page]==0)
        return -1;
    //不用判断线性地址了,因为mm是内核态进程,整个4GB就是它的家,理论上内核都被它管
    if(!(mpdr[(addr>>22)]&0x1)){
        int page_tmp;
        page_tmp=get_free_page();
        if(page_tmp==-1){
            free_page(page);
            return -1;
        }
        mpdr[addr>>22]=(page_tmp<<12)|3;
        asm("mov %0,%%cr3\n\t"::"a"((unsigned int)mpdr));
    }
    table[0]=mpdr[addr>>22];
    ((unsigned int *)0)[(addr>>12)&0x3ff]=(page<<12)|3;
    //刷新CR3
    asm("mov %0,%%cr3\n\t"::"a"((unsigned int)mpdr));
    return  0;
}
#endif
