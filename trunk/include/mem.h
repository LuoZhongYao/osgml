/*
 */
#ifndef     MEM_H
#define     MEM_H
void    mem_init(void);
void    do_no_page(unsigned int error,unsigned int addr);
void    do_wp_page(unsigned int error,unsigned int addr);
int     create_space(void);
#endif
