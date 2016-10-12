#include "common.h"
#include "delay.h"
#include "uart.h"
#include <stdio.h>
#include <string.h>


#define  LED_ON		GPIOC_PSOR=(1<<5)
#define  LED_OFF	GPIOC_PCOR=(1<<5)


/* A debug helper function to print the character and hex value of a byte
 * 
 * @param c : The character to print
 */
void print_char(char c){
    static char buf[32]; 
    int uart_prior; 
    
    /* Use debug UART interface */
    uart_prior = UARTAssignActiveUART(0);
    sprintf(buf, "Char '%c': 0x%x\n", c, c);
    UARTWrite(buf, strlen(buf));

    UARTAssignActiveUART(uart_prior);
}



int main(void){
//    char c; 
//    char buf[20]; 
	PORTC_PCR5 = PORT_PCR_MUX(0x1); // LED is on PC5 (pin 13), config as GPIO (alt = 1)
	GPIOC_PDDR = (1<<5);			// make this an output pin
	LED_OFF;						// start with LED off

    UARTInit(1, 115200);
    UARTInit(0, 115200);   
 
    UARTWrite("Beginning comms\r\n", 17);

//    int done = 0;
//    int count = 0;
    while(1){
        delay_us(1000*1000);
        print_char('Z');

//        count = 0;
//        delay_us(1000*1000);
//        //UARTAssignActiveUART(1);
//        UARTWrite("AT\r\n", 4);
//
//        //UARTAssignActiveUART(0);
//        UARTWrite("Send command!\r\n", 15);
//        //UARTAssignActiveUART(1);
//        
//         while (UARTAvail() != 0 || done != 1 || count > 5){                
//            LED_ON; 
//            UARTRead((buf+count), 1);    
//            count++; 
// 
//            if (c == '\r'){ 
//                //UARTAssignActiveUART(0);
//                UARTWrite(&buf, 5);
//                UARTWrite("\n", 1);
//                done = 1;
//                break;
//             }
//         }
//
//        LED_OFF;
    }

	return  0;						// should never get here!
}
