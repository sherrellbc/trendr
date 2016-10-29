#ifndef _BSP_H
#define _BSP_H

#define  LED_ON		GPIOC_PSOR=(1<<5)
#define  LED_OFF	GPIOC_PCOR=(1<<5)

#define DEBUG_UART_CHANNEL      0

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Initialize system peripherals, LEDs, etc
 */
void bsp_init(void);



#ifdef __cplusplus
}
#endif

#endif /* _BSP_H */
