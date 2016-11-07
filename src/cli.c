#include <string.h>
#include <ctype.h>
#include "util.h"
#include "esp8266_driver.h"
#include "logging.h"
#include "bsp.h"
#include "HardwareSerial.h"


#define CLI_PROMPT  ">>> "
#define CLI_VERSION "1.0"
#define CLI_PRELUDE "\r\n\n"                        \
                    "Trendr cli\r\n"                \
                    "Version: "CLI_VERSION"\r\n"    \
                    "Build Date: "__DATE__" "__TIME__"\r\n"



/* Pointer to implementation specific to the current configuration */
static int (*if_read)(void) = serial_getchar;
static int (*if_avail)(void) = serial_available;
static void (*if_putc)(uint32_t) = serial_putchar;



static void report_prelude(void){
    char version_buf[64];

    /* Collect data on the connected esp8266 */
    if(-1 == esp8266_get_version_info(version_buf, sizeof(version_buf))){
        dlog("Error getting version information from ESP8266\r\n");
        return; 
    }

    dlog(CLI_PRELUDE);
    dlog("Esp8266 vSDK: %s\r\n", version_buf);   
}



int main(void){
    char cmd_buf[128]; 
    char resp_buf[1024];
    int recvd_chars, nbytes;
    
    /* Init system */
    logging_init();
    esp8266_init(); /* Nominally in bsp.c, but we do not need the eeprom/i2c dependency */

    /* Provide version information, esp8266 info, etc */
    report_prelude();

    while(1){
        recvd_chars = 0; 
        dputs(CLI_PROMPT);
        
        /* Read from input until RETURN (0x0D) */
        while(1){
            
            /* Received too many chars -- no room for \r\n\0 */ 
            if(recvd_chars >= sizeof(cmd_buf)-2){
                dlog("\r\nCmd too long\r\n");
                break; 
            }

            if(if_avail() != 0){
                cmd_buf[recvd_chars] = if_read(); 
                if_putc(cmd_buf[recvd_chars]);
    
                //TODO: Fix .. toupper breaks certain commands (like APs with pwds)
                cmd_buf[recvd_chars] = (char)toupper((int)cmd_buf[recvd_chars]);
                recvd_chars++;
                
                /* User pressed RETURN */
                if('\r' == cmd_buf[recvd_chars-1]){ //FIXME: Bounds checking
                    dputs("\n");
                    cmd_buf[recvd_chars++] = '\n';
                    cmd_buf[recvd_chars++] = '\0';  /* So we can determine length */

                    if(-1 == esp8266_do_cmd(cmd_buf, resp_buf, sizeof(resp_buf), &nbytes)){
//                        dlog("Command [%s] not sent\r\n", cmd_buf);
//                        break; 
                    }
                        
                    resp_buf[nbytes-1] = '\0';
                    dlog("Reply[%d]:\r\n[\r\n%s\r\n]\r\n\n", nbytes, resp_buf);
                    break;  
                }
            }else                                
                continue;
        }
    }
}
