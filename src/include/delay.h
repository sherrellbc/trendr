#ifndef _DELAY_H_
#define _DELAY_Y


static inline void delay_us(uint32_t usec){
	uint32_t n = usec*24;
	
    if (0 == n) 
        return;
    
    /* 3 clock cycles? */
	__asm__ volatile(   "L_%=loop:"		    "\n\t"
                		"subs   %0, #1"		"\n\t"          // {S}, Update condition codes -- %0 = %0 - 1
		                "bne    L_%=loop"	"\n"            // if(!zbit) pc=loop
                		: "+r" (n) :                );      //"n" as input/output -- "+" constraint modifier means operand is read and written, "r" means keep in register
}


#endif /* _DELAY_Y */
