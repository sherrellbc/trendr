#include <stdio.h>
#include <string.h>
#include "util.h"
#include "delay.h"
#include "logging.h"
#include "esp8266_driver.h"
#include "HardwareSerial.h"

#define ESP8266_BAUD_RATE  115200

/* ESP8266 Command Set */
#define CMD_GET_VERSION_INFO    "AT+GMR\r\n"
#define CMD_CIPSTATUS           "AT+CIPSTATUS\r\n"
#define CMD_LIST_APS            "AT+CWLAP\r\n"
#define CMD_AT_TEST             "AT\r\n"
#define CMD_AT_ECHO_OFF         "ATE0\r\n"
#define CMD_AT_ECHO_ON          "ATE1\r\n"
#define CMD_GET_UART_CFG        "AT+UART_CUR\r\n"

/* Structure representing a terminating string mode for the esp8266 */
struct term_str{
    const char* str;
    int len; 
};

/* Structure containing various stopping sequences */ 
struct term_str g_term_str_array[] =    {
                                            {
                                                .str = "OK\r\n",
                                                .len = sizeof("OK\r\n")-1,
                                            },
                                            
                                            {
                                                .str = "ERROR\r\n",
                                                .len = sizeof("ERROR\r\n")-1,
                                            },

                                            {
                                                .str = "FAIL\r\n",
                                                .len = sizeof("FAIL\r\n")-1,
                                            },                               
                                        };
                                     
 
/* Pointer to implementation specific to the current configuration */
static void (*if_write)(const void *, unsigned int) = NULL;
static int (*if_read)(void) = NULL;
static int (*if_avail)(void) = NULL;


/* 
 * Instruct the echo mode of the esp8266 module
 *
 * @param mode : 0 off, 1 on
 *
 * @return : -1 on failure
 */
int esp8266_set_echo(int mode){
    char reply[64];
    int nbytes;
    int ret; 
   
    if(0 == mode)
        ret = esp8266_do_cmd(CMD_AT_ECHO_OFF, reply, sizeof(reply), &nbytes);
    else
        ret = esp8266_do_cmd(CMD_AT_ECHO_ON, reply, sizeof(reply), &nbytes);
    
    return ret; 
}



int esp8266_is_up(void){
   /* Delay and send "AT" command -- look for reply */ 

    return 1;
}



int esp8266_get_version_info(char *reply, size_t len){
    char buf[256] = {0};
    char *version_ptr, *reply_buf = reply;  
    int nbytes; 

    /* Sanity check */
    if(NULL == reply)
        return -1; 
    
    /*
     * Issue the command; use -1 bytes for simplicity in use of strstr and its associated
     * null-terminated requirements
     */
    if(-1 == esp8266_do_cmd(CMD_GET_VERSION_INFO, buf, sizeof(buf)-1, &nbytes))
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
    /* Initialize the iterface */
    serial2_begin(BAUD2DIV(ESP8266_BAUD_RATE));
    serial2_format(SERIAL_8N1);

    /* Set the global pointers to the proper interface */
    if_write = serial2_write; 
    if_read = serial2_getchar;
    if_avail = serial2_available;  
    
    //while !esp8266_is_up

    /* Configure the operating mode */ 
    esp8266_set_echo(0); 
}



int esp8266_get_avail_aps(struct access_point_node **node_list, int num_aps){
    char buf[1024];
    int nbytes; 

    if(-1 == esp8266_do_cmd(CMD_LIST_APS, buf, sizeof(buf), &nbytes))
        return -1; 

    return 0; 
}



//TODO: If recv'd chars > len then we never get recv "OK"
int esp8266_do_cmd(char const * const cmd, char *reply, size_t len, int *reply_len){
    size_t recvd_chars = 0;
    int term_str_idx[sizeof(g_term_str_array)/sizeof(struct term_str)] = {0}; 
    int ret = -1; 
    int i,j;

    /* Sanity check */
    if(NULL == reply)
        return -1;

#ifdef ESP8266_DEBUG
    dlog("[db] Sending command [%s]\r\n\n", cmd);
#endif

    /* Write the command to the esp8266; strlen means no null-term is sent */
    if_write(cmd, strlen(cmd));

    /* Loop and save the response in the buffer */
    //TODO: Implement timeout
    while(recvd_chars < len-1){

        if(if_avail() != 0){
            reply[recvd_chars] = if_read();
            recvd_chars++;
        }else
            continue;

        /* Check each of the possible termination strings for a match */
        for(i=0; i<sizeof(g_term_str_array)/sizeof(struct term_str); i++){
            if(reply[recvd_chars-1] == g_term_str_array[i].str[term_str_idx[i]]){

                /* Ensure a proper match against the termination sequence by comparing the previous bytes */
                for(j=0; j<term_str_idx[i]; j++){
                    if(reply[recvd_chars-1-j] == g_term_str_array[i].str[term_str_idx[i]-j])
                        continue;
                    else
                        term_str_idx[i] = 0; 
                }

                term_str_idx[i]++; //TODO: validation check for this statement 
                /* A true evaluation means we matched the entire termination string; exit */
                if(g_term_str_array[i].len == term_str_idx[i]){
                    reply[recvd_chars] = '\0';          //fixme: consider bounds as to not write outside them here
                    *reply_len = recvd_chars; 
                    ret = 0;
                    goto done;
                }
            }
        }
    }

done:
#ifdef ESP8266_DEBUG
    dlog("\n[db] Recv'd %d bytes; ret=%d; oci=%d; eci=%d\r\n", recvd_chars, ret, ok_char_index, error_char_index);
#endif
    return ret;
} 
