#include    <gmL/thread.h>
#include    <dgio.h>
threadT   thread_list[THREAD_LEN];
char    thread_map[THREAD_LEN]={0};
pid_t cur_thread=0;
pid_t next_thread=0;
int  last_thread=0;
int thread_size=0;
pid_t   get_free_thread(void){
    thread_size++;
    if(last_thread>=THREAD_LEN)
        last_thread=0;
    for(;last_thread<THREAD_LEN;last_thread++){
        //    printk("thread.c:get_free_thread:\e\n",(unsigned)(&thread_map));
        if(!thread_map[last_thread]){
            thread_map[last_thread]=1;
            return  last_thread++;
        }
    }
    return  -1;
}
