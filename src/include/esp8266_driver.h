#ifndef _ESP8266_DRIVER_H
#define _ESP8266_DRIVER_H

#include "stddef.h"
#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Define for debug mode */
//#define ESP8266_DEBUG


/* Enum representing the modes of encryption */
enum encryption_mode {
    OPEN = 0,
    WEP,
    WPA_PSK,
    WPA2_PSK,
    WPA_WPA2_PSK
};


/* Operating modes supported by the module */
enum esp_module_mode{
    ESP_STA_MODE=1,         /* Device connects to an AP */
    ESP_SOFTAP_MODE,        /* Device acts as an AP */
    ESP_SOFTAP_STA_MODE
};


/* Connection modes (TCP sessions) */
enum esp_conn_mode{
    ESP_SINGLE_CONNECTION=0,
    ESP_MULTI_CONNECTION 
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


/* Structure representing a TCP session */
struct tcp_session {
//    uint8_t linkid;
    uint8_t ipaddr[4];  /* Remote IP address */
    unsigned int port;  /* Remote port */
    uint8_t status;     /* 1 Open, 0 Closed */
};


/* Structure representing a terminating string mode for the esp8266 */
struct term_str{
    const char* str;
    int len; 
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
 * @param replylen   : [OUT] Number of characters received 
 * @param strs       : Options array of termination strings to use; Pass NULL for defaults
 *
 * @return : -1 on failure
 */
//int esp8266_do_cmd(char const * const cmd, char *reply, size_t len, int *reply_len, struct term_str *strs);
int esp8266_do_cmd(char const * const cmd, char *reply, size_t len, int *replylen);



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



/*
 * Query the device for the current IP address 
 * NB: Never call this function unless following a call to *ap_connect
 *
 * @param ipbuf : Buffer to contain the 4 octets of an IP address in decimal format
 *
 * @return      : -1 on failure
 */
int esp8266_get_ipv4_addr(uint8_t (*ipbuf)[4]);



/*
 * Set the multiplex configuration with respect to the TCP/IP stack. 
 * Specifically, set the number of allowed ongoing TCP sessions
 *
 * @param mode  : The desired connection mode (single or multi); see enum esp_conn_mode 
 *
 * @return      : -1 on failure
 */
int esp8266_set_connection_mode(enum esp_conn_mode mode);



/* 
 * Start a new TCP session
 *
 * @param session   : Pointer to a non-NULL structure to be populated with link meta data
 * 
 * @return          : -1 on failure
 */
int esp8266_tcp_open(struct tcp_session *session);



/* 
 * Close a currently open TCP session
 *
 * @param session   : The TCP session to close; assumed non-NULL
 *
 * @return          : -1 on failure
 */
int esp8266_tcp_close(struct tcp_session *session); 


/*
 * Send arbitrary data over an open TCP connection 
 *
 * @param session   : An open TCP session
 * @param data      : Pointer to buffer containing data to send
 * @param len       : Length of "data"
 *
 * @return          : -1 on failure
 */
int esp8266_tcp_send(struct tcp_session *session, uint8_t *data, size_t len);



/*
 * Set the operating mode of the module. The only accepted values are 
 * of the set defined by the module_mode enum
 * 
 * @param mode  : The desired mode
 *
 * @return      : -1 on failure
 */
int esp8266_setmode(enum esp_module_mode mode);



/*
 * Read from the esp8266 up to len bytes or if str is encountered,
 * whichever occurs first
 *
 * @param reply     : Buffer to store the response
 * @param len       : Number of bytes to read
 * @param replylen  : [OUT] Number of characters received 
 * @param str       : Pointer to optional termination string; NULL if not needed
 *
 * @return          : -1 on failure
 */
int esp8266_read(char *reply, size_t len, int *replylen, const struct term_str *str);



#ifdef __cplusplus
}
#endif

#endif /* _ESP8266_DRIVER_H */
