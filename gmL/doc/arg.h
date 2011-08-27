��器的不同,所以定义的宏也有所不同,下面以VC++中stdarg.h里x86平台的宏定义摘录如下(’"’号表示折行): 
typedef char * va_list; 
#define _INTSIZEOF(n) \ 
((sizeof(n)+sizeof(int)-1)&~(sizeof(int) - 1) ) 
#define va_start(ap,v) 