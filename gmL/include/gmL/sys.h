/*
 *  系统调用底层处理和系统调用列表
 */
typedef int (*fnTp)();
extern  int sys_reg_serve();
extern  int sys_send();
extern  int sys_receiv();
extern  pid_t fork();
fnTp sys_call_table[] = {
    sys_reg_serve,sys_send,sys_receiv,fork
};
int NR_sys_calls = 4;//sizeof(sys_call_table)/sizeof(fnTp);
