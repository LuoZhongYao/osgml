/*
 */
#ifndef     MEM_H
#define     MEM_H
void    mm_init(void);
void    do_no_page(unsigned int error,unsigned int addr);
void    do_wp_page(unsigned int error,unsigned int addr);
unsigned     create_space(void);
#endif
