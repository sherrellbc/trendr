#include <stdio.h>
#include <string.h>
#include "common.h"
#include "uart.h"
#include "stdarg.h"



void print_char(char c){
    static char buf[32]; 
    int uart_prior; 
    
    /* Use debug UART interface */
    uart_prior = UARTAssignActiveUART(DEBUG_UART_CHANNEL);
    sprintf(buf, "Char '%c': 0x%x\n", c, c);
    UARTWrite(buf, strlen(buf));

    UARTAssignActiveUART(uart_prior);
}



/* Needs some serious optimization */
void buf_fill(uint8_t *buf, char *watermark, size_t wm_len, size_t dest_len){
    int num_times = dest_len/wm_len;
    int index = 0; 
    
    for(; num_times>=0; num_times--){
        memcpy((void *) (&buf[index]), (void *)watermark, wm_len); 
        index += wm_len; 
    }
}



int dlog(const char *fmt, ...){
    static char buf[512]; 
    int num_bytes; 
    int uart_prior;    

    va_list args;
    va_start(args, fmt);
    num_bytes = vsprintf(buf, fmt, args); 
    va_end(args);
    
    uart_prior = UARTAssignActiveUART(DEBUG_UART_CHANNEL);
    UARTWrite(buf, num_bytes); 
    UARTAssignActiveUART(uart_prior);
    
    return num_bytes;
}
