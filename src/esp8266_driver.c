#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "delay.h"
#include "logging.h"
#include "esp8266_driver.h"
#include "HardwareSerial.h"

#define ESP8266_BAUD_RATE  115200


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
    int replylen, ret; 
   
    if(0 == mode)
        ret = esp8266_do_cmd("ATE0\r\n", reply, sizeof(reply), &replylen);
    else
        ret = esp8266_do_cmd("ATE1\r\n", reply, sizeof(reply), &replylen);
    
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
    if(-1 == esp8266_do_cmd("AT+GMR\r\n", buf, sizeof(buf)-1, &nbytes))
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

    /* Set the device to station mode */
    esp8266_setmode(ESP_STA_MODE);

    /* TCP Session Config */
    esp8266_set_connection_mode(ESP_SINGLE_CONNECTION);

    /* Echo mode (at output) */ 
    esp8266_set_echo(0); 
}



int esp8266_get_avail_aps(struct ap_node **node_list, int num_aps){
    char reply[1024];
    int replylen; 

    if(-1 == esp8266_do_cmd("AT+CWLAP\r\n", reply, sizeof(reply), &replylen))
        return -1; 

    return 0; 
}



int esp8266_ap_connect(struct ap_node *node){
    char cmdbuf[128], reply[128]; 
    int replylen; 

    sprintf(cmdbuf, "AT+CWJAP=\"%s\",\"%s\"\r\n", node->ssid, node->passwd);
    if(-1 == esp8266_do_cmd(cmdbuf, reply, sizeof(reply), &replylen))
        return -1; 

    /* Check for error */
    if(NULL == strstr(reply, "CONNECTED"))
        return -1;

#ifdef ESP8266_DEBUG                                    
    dlog("[db; %s] Connected to AP=\"%s\"; IP acquired", __FUNCTION__, node->ssid);
#endif
    return 0; 
}



int esp8266_ap_disconnect(void){
    char reply[128]; 
    int replylen; 

    //TODO: Bounds checkiung
    if(-1 == esp8266_do_cmd("AT+CWQAP\r\n", reply, sizeof(reply), &replylen))
        return -1; 

    /* Check for error */
    if(NULL != strstr(reply, "ERROR"))
        return -1;

 #ifdef ESP8266_DEBUG                                    
    dlog("[db; %s] Disconnected from AP\r\n", __FUNCTION__);
 #endif
    return 0; 
}



/* FIXME: This works for only the first IP listed, but STA/SOFTAP mode returns two */
int esp8266_get_ipv4_addr(uint8_t (*ipbuf)[4]){
    char *ip_delim[] = {"\"", ".", ".", ".", "\""}; 
    char reply[128], *ip_idx; 
    int replylen, i;

    if(-1 == esp8266_do_cmd("AT+CIFSR\r\n", reply, sizeof(reply), &replylen))
        return -1;

    /* Quick check to ensure we succeeded in our query */
    if(NULL == strstr(reply, "OK\r\n"))
        return -1; 

    /* (Crudely) parse the response and extract the IP address */
    ip_idx = strtok(reply, ip_delim[0]);
    for(i=1; i<sizeof(ip_delim)/sizeof(char*); i++){
        ip_idx = strtok(NULL, ip_delim[i]);
        if(NULL != ip_idx)
            (*ipbuf)[i-1] = (uint8_t) atoi(ip_idx);
    }

#ifdef ESP8266_DEBUG                                    
    dlog("[db; %s] Local Station IP: %d.%d.%d.%d\r\n", __FUNCTION__, (*ipbuf)[0], (*ipbuf)[1], (*ipbuf)[2], (*ipbuf)[3]);
#endif
    return 0; 
} 



int esp8266_setmode(enum esp_module_mode mode){
    char cmdbuf[128];
    char reply[128]; 
    int replylen; 
    
    switch(mode){
        case ESP_STA_MODE:
        case ESP_SOFTAP_MODE:
        case ESP_SOFTAP_STA_MODE:
            /* Send the mode directly; it's valid */
            sprintf(cmdbuf, "AT+CWMODE_CUR=%d\r\n", mode);     
            if(-1 == esp8266_do_cmd(cmdbuf, reply, sizeof(reply), &replylen))
                return -1;
            break; 
        
        default:
#ifdef ESP8266_DEBUG
            dlog("[db; %s] Unknown mode[%d] specified\r\n", __FUNCTION__, mode);
#endif                                                       
            return -1; 
    }

    return 0; 
}



int esp8266_set_connection_mode(enum esp_conn_mode mode){
    char reply[128], cmdbuf[128]; 
    int replylen;

    switch(mode){
        case ESP_SINGLE_CONNECTION:
        case ESP_MULTI_CONNECTION:
            /* Send the mode directly; it's valid */
            sprintf(cmdbuf, "AT+CIPMUX=%d\r\n", mode);     
            if(-1 == esp8266_do_cmd(cmdbuf, reply, sizeof(reply), &replylen))
                return -1;
            break; 
        
        default:
#ifdef ESP8266_DEBUG
            dlog("[db; %s] Unknown mode[%d] specified\r\n", __FUNCTION__, mode);
#endif                                                      
        return -1; 
    } 

    /* Quick check to ensure we succeeded in our query */
    if(NULL == strstr(reply, "OK\r\n"))
        return -1; 

#ifdef ESP8266_DEBUG                                    
    dlog("[db; %s] Set mode %d\r\n", __FUNCTION__, mode);
#endif
    return 0; 
}



/* TODO: ? Add support for UDP */
/* FIXME: Assumes CIPMUX=0 */
int esp8266_tcp_open(struct tcp_session *session){
    char cmdbuf[128], reply[128]; 
    int replylen;

    sprintf(cmdbuf, "AT+CIPSTART=\"TCP\",\"%d.%d.%d.%d\",%d\r\n", session->ipaddr[0], session->ipaddr[1], 
        session->ipaddr[2], session->ipaddr[3], session->port);

    if(-1 == esp8266_do_cmd(cmdbuf, reply, sizeof(reply), &replylen))
        return -1; 

    /* Check for error */
    if(NULL == strstr(reply, "OK\r\n"))
        return -1;

#ifdef ESP8266_DEBUG                                    
    dlog("[db; %s] TCP session opened\r\n", __FUNCTION__);
#endif
    return 0; 
}



/* FIXME: Currently closes ALL open TCP sessions */
int esp8266_tcp_close(struct tcp_session *session){
    char cmdbuf[128], reply[128]; 
    int replylen;

    /* TODO: Check which operating mode and send variant of command appropriate */
    sprintf(cmdbuf, "AT+CIPCLOSE\r\n"); 
    if(-1 == esp8266_do_cmd(cmdbuf, reply, sizeof(reply), &replylen))
        return -1; 

    /* Check for error */
    if(NULL == strstr(reply, "OK\r\n"))
        return -1;

#ifdef ESP8266_DEBUG                                    
    dlog("[db; %s] TCP session opened\r\n", __FUNCTION__);
#endif
    return 0; 
}



int esp8266_tcp_send(struct tcp_session *session, uint8_t *data, size_t len){
    struct term_str tcp_send_term = { .str = ">", .len = 1};
    char cmdbuf[128], reply[128]; 
    int replylen;

    sprintf(cmdbuf, "AT+CIPSEND=%d\r\n", len); 
    if(-1 == esp8266_do_cmd(cmdbuf, reply, sizeof(reply), &replylen))
        return -1; 

    /* Check for error */
    if(NULL == strstr(reply, "OK\r\n"))
        return -1;

    /* Extract the ">" response from above */
    esp8266_read(reply, sizeof(reply), &replylen, &tcp_send_term);

    /* FIXME: len/cmdbuf length check */
    snprintf(cmdbuf, len+1, "%s", data); /* snprintf takes only len-1 so nul-term may fit */
    if(-1 == esp8266_do_cmd(cmdbuf, reply, sizeof(reply), &replylen))
        return -1; 

    /* Check for error */
    if(NULL == strstr(reply, "OK\r\n"))
        return -1;

#ifdef ESP8266_DEBUG                                    
    dlog("[db; %s] TCP data sent\r\n", __FUNCTION__);
#endif
    return 0; 
}



int esp8266_read(char *reply, size_t len, int *replylen, const struct term_str *str){
    size_t recvd_chars = 0; 
    int term_str_idx = 0; 
    int i,ret = -1; 

 #ifdef ESP8266_DEBUG 
     dlog("[db; %s] Reading incoming data from esp8266 ... \r\n", __FUNCTION__);
 #endif


    /* Read from the FIFO as data arrives */
    while(recvd_chars < len-1){
                                                                                                                     
        if(if_avail() != 0){                                                                                           
            reply[recvd_chars] = if_read();
            recvd_chars++;
        }else
            continue;

//        dlog("Got %c[%d]; Comparing against %c[%d]\r\n", reply[recvd_chars-1], reply[recvd_chars-1], str->str[term_str_idx], str->str[term_str_idx]);

        /* Check if the input stream has provided the specified termination string */
        if(reply[recvd_chars-1] == str->str[term_str_idx]){

            /* Ensure a proper match against the termination sequence by comparing the previous bytes */
            for(i=0; i<term_str_idx; i++){
                if(reply[recvd_chars-1-i] == str->str[term_str_idx-i])
                    continue;
                else
                    term_str_idx = 0; /* This "just works" for now, but needs to properly continue to next i */ 
            }

            term_str_idx++; //TODO: validation check for this statement 
            /* A true evaluation means we matched the entire termination string; exit */
            if(str->len == term_str_idx){
                reply[recvd_chars] = '\0';          //fixme: consider bounds as to not write outside them here
                *replylen = recvd_chars; 
                ret = 0;
                break; 
            }
        }
    }

 #ifdef ESP8266_DEBUG 
     dlog("[db; %s] Recd'd %d bytes; tci=%d\r\n", __FUNCTION__, recvd_chars, term_str_idx);
 #endif
    return ret;
}



//TODO: If recv'd chars > len then we never get recv "OK"
int esp8266_do_cmd(char const * const cmd, char *reply, size_t len, int *replylen){
    size_t recvd_chars = 0;
    int term_str_idx[sizeof(g_term_str_array)/sizeof(struct term_str)] = {0}; 
    int ret = -1; 
    int i,j;

    /* Sanity check */
    if(NULL == reply)
        return -1;

#ifdef ESP8266_DEBUG
    dlog("[db; %s] Sending command [%s]\r\n", __FUNCTION__, cmd);
#endif

    /* Write the command to the esp8266; strlen means no null-term is sent */
    if_write(cmd, strlen(cmd));

    /* Loop and save the response in the buffer */
    //TODO: Implement timeout
    //TODO: Failsafe: if module sends > len then truncate and spin on incoming data 
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
                        term_str_idx[i] = 0; /* This "just works" for now, but needs to properly continue to next i */ 
                }

                term_str_idx[i]++; //TODO: validation check for this statement 
                /* A true evaluation means we matched the entire termination string; exit */
                if(g_term_str_array[i].len == term_str_idx[i]){
                    reply[recvd_chars] = '\0';          //fixme: consider bounds as to not write outside them here
                    *replylen = recvd_chars; 
                    ret = 0;
                    goto done;
                }
            }
        }
    }

done:
#ifdef ESP8266_DEBUG
    dlog("[db; %s] Reply[%d]:\r\n[\r\n%s\r\n]\r\n", __FUNCTION__, recvd_chars, reply);
    dlog("[db; %s] Recv'd %d bytes; ret=%d; oci=%d; eci=%d; fci=%d\r\n\n\n", __FUNCTION__, recvd_chars, ret, term_str_idx[0], term_str_idx[1], term_str_idx[2]);
#endif
    return ret;
} 
