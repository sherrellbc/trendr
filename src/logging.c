#include <stdio.h>
#include "logging.h"
#include <string.h>
#include "common.h"
#include "uart.h"
#include "stdarg.h"

#define DEBUG_UART_CHANNEL      0



void print_char(char c){
    static char buf[32]; 
    int uart_prior; 
    
    /* Use debug UART interface */
    uart_prior = UARTAssignActiveUART(DEBUG_UART_CHANNEL);
    sprintf(buf, "Char '%c': 0x%x\n", c, c);
    UARTWrite(buf, strlen(buf));

    UARTAssignActiveUART(uart_prior);
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



int dputchar(char c){
    int nbytes;    
    int uart_prior; 

    uart_prior = UARTAssignActiveUART(DEBUG_UART_CHANNEL);
    nbytes = UARTWrite(&c, sizeof(char)); 
    UARTAssignActiveUART(uart_prior);
    
    return (nbytes == 1) ? 0 : -1;
}



int dputs(char const * const str){
    int nbytes;    
    int uart_prior; 
    int len;
                                                              
    uart_prior = UARTAssignActiveUART(DEBUG_UART_CHANNEL);
    len = strlen(str);
    nbytes = UARTWrite(str, len); 
    UARTAssignActiveUART(uart_prior);

    return (nbytes == len) ? 0 : -1;
}
