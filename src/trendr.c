#include "common.h"
#include "delay.h"
#include "uart.h"
#include <stdio.h>
#include <string.h>

#define DEBUG_UART_CHANNEL      0
#define ESP8266_UART_CHANNEL    1 

/* ESP8266 Command Set */
#define CMD_VIEW_VERSION_INFO "AT+GMR\r\n"


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
    uart_prior = UARTAssignActiveUART(DEBUG_UART_CHANNEL);
    sprintf(buf, "Char '%c': 0x%x\n", c, c);
    UARTWrite(buf, strlen(buf));

    UARTAssignActiveUART(uart_prior);
}


void local_memset(void *dst, uint8_t c, size_t n){
    int i = 0;
    for(; i<n; i++)
        ((uint8_t *)dst)[i] = (uint8_t)c;
}


//TODO: If recv'd chars > len then we never get recv "OK"
int esp8266_do_cmd(char const * const cmd, char *reply, size_t len){
    size_t recvd_chars = 0;
    int uart_prior, ret = -1; 

    if(NULL == reply)
        return -1; 
    
    /* Grab the current UART channel so it may be restored */
    uart_prior = UARTAssignActiveUART(ESP8266_UART_CHANNEL);

    /* Write the command to the esp8266; strlen means no null-term is sent */
    UARTWrite(cmd, strlen(cmd));
//    goto exit;

    /* Loop and save the response in the buffer */
    //TODO: Implement timeout
    while(recvd_chars < len-1){

        if(UARTAvail() != 0){
            UARTRead(&(reply[recvd_chars]), 1);
            recvd_chars++;
        }else
            continue;

        if(21 == recvd_chars){
            ret = 21;
            goto exit;
        }

        /* If last recv'd char was a newline then all has been transferred */
        if( ('\n' == reply[recvd_chars-1]) || ('\r' == reply[recvd_chars-1]) ){
            
            /* Properly terminate the response buffer */            
            if(recvd_chars >= len)
                reply[len-1] = '\0';
            else
                reply[recvd_chars] = '\0';
            

            /* If "OK" present in response then success, else fail (for now) */
            if(NULL == strstr(reply, "OK"))
                ret = 0;
            else 
                ret = 0;
 
            break; 
        }
    }

exit:    
    UARTAssignActiveUART(uart_prior);           
    return ret;
} 



void sys_init(void){
	PORTC_PCR5 = PORT_PCR_MUX(0x1); // LED is on PC5 (pin 13), config as GPIO (alt = 1)
	GPIOC_PDDR = (1<<5);			// make this an output pin
	LED_OFF;						// start with LED off

    UARTInit(ESP8266_UART_CHANNEL, 115200);
    UARTInit(DEBUG_UART_CHANNEL, 115200);   
}



/* Needs some serious optimization */
void buf_fill(uint8_t *buf, char *watermark, size_t wm_len, size_t dest_len){
    int num_times = dest_len/wm_len;
    int index = 0; 
    
//    for(; num_times>=0; num_times--){
//        memcpy((void *) (&buf[index]), (void *)watermark, wm_len); 
//        memcpy(&buf[index], "DEADBEEF", 8);
        memset((void*)buf, (int) 'Z', 256);
        index += wm_len;        
//    }
}



int main(void){
    char buf[256]; 
    int recvd_chars; 

    sys_init(); 
    UARTWrite("Beginning comms\r\n", 17);

    while(1){
        LED_ON;
        delay_us(500*1000);
        LED_OFF;
        delay_us(500*1000);

        /* Insert watermark for debugging */
//        buf_fill( (uint8_t *)buf, "DEADBEEF", 8, sizeof(buf));
//        local_memset(buf, (int) 'g', 10);

        /* Send command to module and print response */
        recvd_chars = esp8266_do_cmd(CMD_VIEW_VERSION_INFO, buf, 32);

        if(-1 == recvd_chars){
            UARTWrite("An error occurred .. \r\n", sizeof("An error occurred .. \r\n")-1);
            UARTWrite(buf, 256);
            continue;     
        }

        UARTWrite("\nESP8266 Responded: ", sizeof("ESP8266 Responded: ")-1);
        UARTWrite(buf, recvd_chars);
    }

	return 0; 
}
