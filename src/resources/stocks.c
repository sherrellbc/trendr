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



float stocks_get(struct tcp_session *session, const char *company){
    char url_buf[128]; 
    char float_buf[10] = {0}; 
    int replylen; 
   
    /* Clear static buf before using it */ 
    memset(g_reply, 0, sizeof(g_reply));

    /* Create the query; +1 for nul-term */
    if( (strlen(g_finance_api_url) + strlen(company) + 1) > sizeof(url_buf) )
        return -1; 

    sprintf(url_buf, "%s%s", g_finance_api_url, company); 
    
    /* Download the JSON response from the remote source */
    if(-1 == http_json_get(session, url_buf, g_reply, sizeof(g_reply), &replylen))
        return -1;

    /* Set the nul-term and convert the string to a float directly */
    if(0 != json_value_get("l_fix", float_buf, g_reply))
        return -1; 
    
    return strtof(float_buf, NULL); /* Propagate error if one occurs */
}
