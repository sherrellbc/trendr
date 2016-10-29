#include <stdio.h>
#include <string.h>
#include "logging.h"
#include "stdarg.h"
#include "HardwareSerial.h"
//#include "usb_serial.h"

#define DEBUG_UART_CHANNEL      0
#define DEBUG_BAUD_RATE    115200


void logging_init(void){
    serial_begin(BAUD2DIV(DEBUG_BAUD_RATE));
    serial_format(SERIAL_8N1);
}



void print_char(char c){
    //static char buf[32]; 
    //int uart_prior; 
    //
    ///* Use debug UART interface */
    //uart_prior = UARTAssignActiveUART(DEBUG_UART_CHANNEL);
    //sprintf(buf, "Char '%c': 0x%x\n", c, c);
    //UARTWrite(buf, strlen(buf));

    //UARTAssignActiveUART(uart_prior);
}



int dlog(const char *fmt, ...){
    static char buf[512]; 
    int num_bytes; 
    //int uart_prior;    

    va_list args;
    va_start(args, fmt);
    num_bytes = vsprintf(buf, fmt, args); 
    va_end(args);
    
    //uart_prior = UARTAssignActiveUART(DEBUG_UART_CHANNEL);
    //UARTWrite(buf, num_bytes); 
    //UARTAssignActiveUART(uart_prior);
    serial_write(buf, num_bytes);
    return num_bytes;
}



void dputchar(char c){
    serial_putchar(c);
}



void dputs(char const * const str){
    //int nbytes;    
    //int uart_prior; 
    int len;
                                                              
    //uart_prior = UARTAssignActiveUART(DEBUG_UART_CHANNEL);
    len = strlen(str);
    //nbytes = UARTWrite(str, len); 
    //UARTAssignActiveUART(uart_prior);
    //return (nbytes == len) ? 0 : -1;
    serial_write(str, len);
}




