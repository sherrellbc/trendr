#ifndef _ESP8266_DRIVER_H
#define _ESP8266_DRIVER_H

#include "stddef.h"



/* Initialize the ESP8266 
 */
void esp8266_init(void);



/* Send a command to the ESP8266
 *
 * @param cmd   : The command to send
 * @param reply : A preallocated buffer for the response
 * @param len   : Length of the response buffer
 *
 * @return : -1 on failure
 */
int esp8266_do_cmd(char const * const cmd, char *reply, size_t len);



int esp8266_get_version_info(char *reply, size_t len);


#endif /* _ESP8266_DRIVER_H */
