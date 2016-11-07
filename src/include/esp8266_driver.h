#ifndef _ESP8266_DRIVER_H
#define _ESP8266_DRIVER_H

#include "stddef.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Enum representing the modes of encryption */
enum encryption_mode {
    OPEN = 0,
    WEP,
    WPA_PSK,
    WPA2_PSK,
    WPA_WPA2_PSK
};


/* Definition of AP meta-data structure */
struct ap_node{
    enum encryption_mode enc_mode; 
    char ssid[64];
    int rssi; 
    char mac[11];
    unsigned int channel; 
    int freq_offset;
    
    /* Used when connecting */
    char passwd[64];  
};



/* 
 * Initialize the ESP8266 
 */
void esp8266_init(void);



/* 
 * Send a command to the ESP8266
 *
 * @param cmd        : The null-terminated command to send
 * @param reply      : [OUT] A preallocated buffer for the response
 * @param len        : Length of the response buffer
 * @param reply_lend : [OUT] Number of characters received 
 *
 * @return : -1 on failure
 */
int esp8266_do_cmd(char const * const cmd, char *reply, size_t len, int *reply_len);



/* 
 * Extract the SDK version information from the esp8266 module
 *
 * @param reply : The pre-allocated buffer to which the reply will be saved
 * @param len   : The length of "reply" buffer
 *
 * @return      : -1 on failure
 */
int esp8266_get_version_info(char *reply, size_t len);



/*
 * Join an access point defined by "node"
 *
 * @param node  : A structure defining the node to connect to (need only ssid and passwd)
 *
 * @return      : -1 on failure
 */
int esp8266_ap_connect(struct ap_node *node);



/*
 * Disconnect from the currently associated access point (if any)
 * 
 * @return  : -1 on error
*/
int esp8266_ap_disconnect(void);



#ifdef __cplusplus
}
#endif

#endif /* _ESP8266_DRIVER_H */
