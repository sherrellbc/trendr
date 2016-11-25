#ifndef _STOCKS_H
#define _STOCKS_H

#include "esp8266_driver.h"


#ifdef __cplusplus
extern "C" {
#endif



/*
 * Return the current value for the specified stock
 *
 * @param session   : An active TCP session
 * @param remote    : A null-terminated string representing a stock to query
 *
 * @return  : -1 on failure 
 */
float stocks_get(struct tcp_session *session, const char *company);



#ifdef __cplusplus
}
#endif


#endif  /* _STOCKS_H */
