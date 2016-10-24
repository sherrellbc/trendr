#include <stdio.h>
#include <string.h>
#include "uart.h"
#include "util.h"
#include "delay.h"
#include "logging.h"
#include "esp8266_driver.h"

#define ESP8266_UART_CHANNEL    1 
#define ESP8266_UART_BAUD_RATE  115200

/* ESP8266 Command Set */
#define CMD_GET_VERSION_INFO    "AT+GMR\r\n"
#define CMD_CIPSTATUS           "AT+CIPSTATUS\r\n"
#define CMD_LIST_APS            "AT+CWLAP\r\n"
#define CMD_AT_TEST             "AT\r\n"
#define CMD_AT_ECHO_OFF         "ATE0\r\n"
#define CMD_AT_ECHO_ON          "ATE1\r\n"
#define CMD_GET_UART_CFG        "AT+UART_CUR\r\n"


/* Buffer containing the expected stopping sequences for success and error, respectively */ 
static const char g_esp8266_ok_str[] = {'O', 'K', '\r', '\n'}; 
static const char g_esp8266_error_str[] = {'E', 'R', 'R', 'O', 'R', '\r', '\n'}; 



/* 
 * Instruct the echo mode of the esp8266 module
 *
 * @param mode : 0 off, 1 on
 *
 * @return : -1 on failure
 */
int esp8266_set_echo(int mode){
    char reply[64];
    int ret; 
   
    if(0 == mode)
        ret = esp8266_do_cmd(CMD_AT_ECHO_OFF, reply, sizeof(reply));
    else
        ret = esp8266_do_cmd(CMD_AT_ECHO_ON, reply, sizeof(reply));
    
    dlog("Returning\r\n");
    return ret; 
}



int esp8266_is_up(void){
   /* Delay and send "AT" command -- look for reply */ 

    return 1;
}



int esp8266_get_version_info(char *reply, size_t len){
    char buf[256] = {0};
    char *version_ptr, *reply_buf = reply;  

    /* Sanity check */
    if(NULL == reply)
        return -1; 
    
    /*
     * Issue the command; use -1 bytes for simplicity in use of strstr and its associated
     * null-terminated requirements
     */
    if(-1 == esp8266_do_cmd(CMD_GET_VERSION_INFO, buf, sizeof(buf)-1))
        return -1; 
    
    version_ptr = strstr(buf, "SDK version:");
    if(NULL == version_ptr)
        return -1; 

    /* Advance the pointer to the start of the actual version number */
    version_ptr += sizeof("SDK version:")-1; 

    /* Copy the contents of the version string into the reply buffer until \r\n */
    while(*version_ptr != '\r'){
        *(reply_buf++) = *(version_ptr++);
    }
    
    /* Properly terminate */ 
    *(reply_buf++) = '\0';
    return (reply_buf - reply); 
}



void esp8266_init(void){
    int uart_prior; 

    uart_prior = UARTAssignActiveUART(ESP8266_UART_CHANNEL);
    UARTInit(ESP8266_UART_CHANNEL, ESP8266_UART_BAUD_RATE);
    
    //while device is not up, wait for some delayed time
    esp8266_set_echo(0); //FIXME: Why does echo disable here break the cli? 

    UARTAssignActiveUART(uart_prior);
}



//TODO: If recv'd chars > len then we never get recv "OK"
int esp8266_do_cmd(char const * const cmd, char *reply, size_t len){
    size_t recvd_chars = 0;
    int uart_prior, ret = -1; 
    int ok_char_index = 0;
    int error_char_index = 0;

    /* Sanity check */
    if(NULL == reply)
        return -1; 

#ifdef ESP8266_DEBUG
    dlog("[db] Sending command [%s]\r\n\n", cmd);
#endif

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
        if(reply[recvd_chars-1] == g_esp8266_ok_str[ok_char_index]){

            /* Ensure a proper match against the termination sequence by comparing the previous bytes */
            int i; 
            for(i=0; i<ok_char_index; i++){
                if(reply[recvd_chars-1-i] == g_esp8266_ok_str[ok_char_index-i])
                    continue;
                else
                    ok_char_index = 0; 
            }


            ok_char_index++; 
            /* A true evaluation means we matched the entire termination string; exit */
            if(sizeof(g_esp8266_ok_str)/sizeof(char) == ok_char_index){
                reply[recvd_chars] = '\0';          //FIXME: Consider bounds as to not write outside them here
                ret = recvd_chars; 
                break;
            }
        }

        /* Check if the byte-stream has terminated with error */ 
        if(reply[recvd_chars-1] == g_esp8266_error_str[error_char_index]){

            int i; 
            for(i=0; i<error_char_index; i++){
                if(reply[recvd_chars-1-i] == g_esp8266_error_str[error_char_index-i])
                    continue;
                else
                    error_char_index = 0; 
            }

            error_char_index++; 
            /* A true evaluation means we matched the entire termination string; exit */
            if(sizeof(g_esp8266_error_str)/sizeof(char) == error_char_index){
                reply[recvd_chars] = '\0';          //FIXME: Consider bounds as to not write outside them here
                ret = recvd_chars;
                break;
            }
        }
    }

#ifdef ESP8266_DEBUG
    dlog("\n[db] Recv'd %d bytes; ret=%d; oci=%d; eci=%d\r\n", recvd_chars, ret, ok_char_index, error_char_index);
#endif

    UARTAssignActiveUART(uart_prior);           
    return ret;
} 



