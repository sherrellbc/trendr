#ifndef _BSP_H
#define _BSP_H

#define  LED_ON		GPIOC_PSOR=(1<<5)
#define  LED_OFF	GPIOC_PCOR=(1<<5)

#define DEBUG_UART_CHANNEL      0


/*
 * Initialize system peripherals, LEDs, etc
 */
void sys_init(void);



#endif /* _BSP_H */
