#ifndef _HTTP_HELPER_H
#define _HTTP_HELPER_H

#include "stdint.h"
#include "esp8266_driver.h"


#ifdef __cplusplus
extern "C" {
#endif


/*
 * Query a remote host and expect a JSON response
 *
 * @param session   : An active TCP session
 * @param remote    : The remote host + query/parameter null-terminated string
 * @param reply     : Preallocated buffer to which the response will be written
 * @param len       : Length of "reply"
 * @param str       : Character set indicating a termination of the JSON response
 * @param replylen  : [OUT] Number of bytes recieved from the remote host
 *
 * @return  : -1 on failure 
 */
int http_json_get(struct tcp_session *session, char *remote, char *reply, size_t len, const struct term_str *str, int *replylen);


#ifdef __cplusplus
}
#endif


#endif  /* _HTTP_HELPER_H */
