#include <stdio.h>
#include <string.h>

#include "common.h"
#include "delay.h"
#include "esp8266_driver.h"
#include "uart.h"
#include "util.h"

#define DEBUG_UART_CHANNEL      0

#define  LED_ON		GPIOC_PSOR=(1<<5)
#define  LED_OFF	GPIOC_PCOR=(1<<5)


extern void trendr_cli(void);


void sys_init(void){
	PORTC_PCR5 = PORT_PCR_MUX(0x1); // LED is on PC5 (pin 13), config as GPIO (alt = 1)
	GPIOC_PDDR = (1<<5);			// make this an output pin
	LED_OFF;						// start with LED off

    esp8266_init(); 
    UARTInit(DEBUG_UART_CHANNEL, 115200);  
}



int main(void){
    char buf[1024]; 
    int recvd_chars;

    sys_init(); 
    trendr_cli();
    dlog("Beginning comms\r\n");

    while(1){
        delay_us(1000*1000);

        /* Send command to module and print response */
        memset(buf, ' ', sizeof(buf));
        dlog("\n\n= = = = = = = = = = = = = = = = = = = =\r\n");
        recvd_chars = esp8266_do_cmd("AT\r\n", buf, sizeof(buf));

        if(-1 == recvd_chars){
            dlog("\nAn error occurred .. \r\n");
            continue;     
        }

        dlog("\nESP8266 Responded:\r\n");
        
        UARTWrite(buf, recvd_chars);
    }

	return 0; 
}
