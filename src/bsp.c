#include "common.h" 
#include "uart.h"
#include "esp8266_driver.h"
#include "logging.h"

#define  LED_ON		GPIOC_PSOR=(1<<5)
#define  LED_OFF	GPIOC_PCOR=(1<<5)


void bsp_init(void){
	PORTC_PCR5 = PORT_PCR_MUX(0x1); // LED is on PC5 (pin 13), config as GPIO (alt = 1)
	GPIOC_PDDR = (1<<5);			// make this an output pin
	LED_OFF;						// start with LED off

    esp8266_init(); 
}
