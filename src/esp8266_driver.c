#include <stdio.h>
#include <string.h>
#include "uart.h"

#define ESP8266_UART_CHANNEL    1 
#define ESP8266_UART_BAUD_RATE  115200

/* ESP8266 Command Set */
#define CMD_VIEW_VERSION_INFO   "AT+GMR\r\n"
#define CMD_CIPSTATUS           "AT+CIPSTATUS\r\n"
#define CMD_LIST_APS            "AT+CWLAP\r\n"
#define CMD_AT_TEST             "AT\r\n"
#define CMD_AT_ECHO_OFF         "ATE0\r\n"
#define CMD_AT_ECHO_ON          "ATE1"
#define CMD_GET_UART_CFG        "AT+UART_CUR\r\n"



void esp8266_init(void){
    UARTInit(ESP8266_UART_CHANNEL, ESP8266_UART_BAUD_RATE);
}



//TODO: If recv'd chars > len then we never get recv "OK"
int esp8266_do_cmd(char const * const cmd, char *reply, size_t len){
    static const char esp8266_ok_str[] = {'O', 'K', '\r', '\n'}; 
    static const char esp8266_error_str[] = {'E', 'R', 'R', 'O', 'R', '\r', '\n'}; 
    size_t recvd_chars = 0;
    int uart_prior, ret = -1; 
    int ok_char_index = 0;
    int error_char_index = 0;

    /* Sanity check */
    if(NULL == reply)
        return -1; 

    /* Grab the current UART channel so it may be restored */
    uart_prior = UARTAssignActiveUART(ESP8266_UART_CHANNEL);

    /* Write the command to the esp8266; strlen means no null-term is sent */
    UARTWrite(cmd, strlen(cmd));

    /* Loop and save the response in the buffer */
    //TODO: Implement timeout
    while(recvd_chars < len-1){

        if(UARTAvail() != 0){
            UARTRead(&reply[recvd_chars], 1);
            recvd_chars++;
        }else
            continue;
       
        /* Check if the byte-stream has terminated successfully */ 
        if(esp8266_ok_str[ok_char_index] == reply[recvd_chars-1]){
            ok_char_index++; 

            /* A true evaluation means we matched the entire termination string; exit */
            if(sizeof(esp8266_ok_str)/sizeof(char) == ok_char_index){
                reply[recvd_chars] = '\0';          //FIXME: Consider bounds as to not write outside them here
                ret = recvd_chars; 
                break;
            }
        }

        /* Check if the byte-stream has terminated with error */ 
        if(esp8266_error_str[error_char_index] == reply[recvd_chars-1]){
            error_char_index++; 

            /* A true evaluation means we matched the entire termination string; exit */
            if(sizeof(esp8266_error_str)/sizeof(char) == error_char_index){
                reply[recvd_chars] = '\0';          //FIXME: Consider bounds as to not write outside them here
                ret = recvd_chars; 
                break;
            }
        }
        
        /* Do the same as above but check for ERROR? First to complete is representative of state? */
    }

    UARTAssignActiveUART(uart_prior);           
    char tbuf[256]; 
    UARTWrite(tbuf, sprintf(tbuf, "\nRecv'd %d bytes; ret=%d; oci=%d; eci=%d\r\n", recvd_chars, ret, ok_char_index, error_char_index));
    return ret;
} 
