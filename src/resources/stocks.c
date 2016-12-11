#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "logging.h"
#include "http_helper.h"
#include "json_parser.h"


/* API url (and partial parameter) for querying Google for a current stock value */
static const char * g_finance_api_url = "http://finance.google.com/finance/info?q=";

/* Static buffer in application DATA section; too large for stack */
static char g_reply[2048]; 

/* The termination string sent from the API source */
static const struct term_str gjson_termstr = {
                                                .str = "]",
                                                .len = 1
                                             };

/* Remote TCP connection information */
//struct tcp_session stock_remote =   {
//                                        .ipaddr = {172, 217, 2, 206},
//                                        .port = 80,
//                                        .status = 0
//                                    };

struct tcp_session stock_remote =   {
                                        .ipaddr = {192, 168, 0, 105},
                                        .port = 80,
                                        .status = 0
                                    };
 
//FIXME: Check first to ensure we are connect to an AP (when writing WiFi abstraction layer */
float stocks_get(const char *company){
    char url_buf[128]; 
    char float_buf[10] = {0}; 
    int replylen; 

    
    if(-1 == esp8266_tcp_open(&stock_remote))
        return -1;
   
    /* Clear static buf before using it */ 
    memset(g_reply, 0, sizeof(g_reply));

    /* Create the query; +1 for nul-term */
    if( (strlen(g_finance_api_url) + strlen(company) + 1) > sizeof(url_buf) )
        return -1; 

    sprintf(url_buf, "%s%s", g_finance_api_url, company); 
    
    /* Download the JSON response from the remote source */
    if(-1 == http_json_get(&stock_remote, url_buf, g_reply, sizeof(g_reply), &gjson_termstr, &replylen))
        return -1;

    /* Set the nul-term and convert the string to a float directly */
    if(0 != json_value_get("l_fix", float_buf, g_reply))
        return -1; 
    
    return strtof(float_buf, NULL); /* Propagate error if one occurs */
}
