#include <string.h>
#include <stdio.h>

#include "http_helper.h"
#include "logging.h"
#include "util.h"


/* When querying a JSON source, the following termination sequence is used to parse the response */
static const struct term_str gjson_termstr = {
                                                .str = "]",
                                                .len = 1
                                             };

int http_json_get(struct tcp_session *session, char *remote, char *reply, size_t len, int *replylen){
    char buf[256] = {0}; 
    int cmdlen; 
   
    cmdlen = sprintf(buf, "GET %s\n", remote); 
    if(-1 == esp8266_tcp_send(session, (uint8_t*)buf, cmdlen))
        return -1; 
    
    if(-1 == esp8266_read(reply, len, replylen, &gjson_termstr))
        return -1;

    /* Lazy checker for now .. check if we did not fail */
    if(NULL == strstr(reply, "200 OK\r\n"))
        return -1; 

	return 0;
}





//TODO: WiFi abstraction layer overtop ESP8266-specific code
