#include <string.h>
#include <ctype.h>
#include "common.h"
#include "uart.h"
#include "util.h"
#include "esp8266_driver.h"
#include "logging.h"
#include "bsp.h"


#define CLI_PROMPT  ">>> "
#define CLI_VERSION "1.0"
#define CLI_PRELUDE "\r\n\n"                        \
                    "Trendr cli\r\n"                \
                    "Version: "CLI_VERSION"\r\n"    \
                    "Build Date: "__DATE__" "__TIME__"\r\n"



void report_prelude(void){
    char version_buf[64]; 

    /* Collect data on the connected esp8266 */
    if(-1 == esp8266_get_version_info(version_buf, sizeof(version_buf)/sizeof(char))){
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
    sys_init();

    /* Provide version information, esp8266 info, etc */
    report_prelude();

    while(1){
        recvd_chars = 0; 
        dputs(CLI_PROMPT);
        
        /* Clear buffers before receiving data */
        memset(cmd_buf, '\0', sizeof(cmd_buf));
        memset(resp_buf, '\0', sizeof(resp_buf));
        //*cmd_buf = '\0';
        //*resp_buf = '\0';

        /* Read from input until RETURN (0x0D) */
        while(1){
            
            /* Received too many chars -- no room for \r\n\0 */ 
            if(recvd_chars >= sizeof(cmd_buf)-2){
                dlog("\r\nCmd too long\r\n");
                break; 
            }

            if(UARTAvail() != 0){
                UARTRead(&cmd_buf[recvd_chars], 1);
                UARTWrite(&cmd_buf[recvd_chars], 1);
                cmd_buf[recvd_chars] = toupper(cmd_buf[recvd_chars]);
                recvd_chars++;
                
                /* User pressed RETURN */
                if('\r' == cmd_buf[recvd_chars-1]){ //FIXME: Bounds checking
                    dputs("\n");
                    cmd_buf[recvd_chars++] = '\n';
//                    cmd_buf[recvd_chars++] = '\0';

                    nbytes = esp8266_do_cmd(cmd_buf, resp_buf, sizeof(resp_buf));
                    if(-1 == nbytes){
                        dlog("Command [%s] not sent\r\n", cmd_buf);
                        break; 
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
