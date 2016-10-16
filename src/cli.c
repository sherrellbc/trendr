#include <string.h>
#include "common.h"
#include "uart.h"
#include "esp8266_driver.h"


#define CLI_PROMPT  ">>> "
#define CLI_VERSION "1.0"
#define CLI_PRELUDE "\r\n\n"                        \
                    "Trendr cli\r\n"                \
                    "Version: "CLI_VERSION"\r\n"    \
                    "Build Date: "__DATE__"\r\n"



void trendr_cli(void){
    char cmd_buf[128]; 
    char resp_buf[1024];
    int recvd_chars;
    int response_chars;    

    /* Prelude */
    UARTWrite(CLI_PRELUDE, sizeof(CLI_PRELUDE)-1);

    while(1){
        recvd_chars = 0; 
        UARTWrite(CLI_PROMPT, sizeof(CLI_PROMPT)-1);
        
        /* Clear buffers before receiving data */
        memset(cmd_buf, '\0', sizeof(cmd_buf));
        memset(resp_buf, '\0', sizeof(resp_buf));

        /* Read from input until RETURN (0x0D) */
        while(recvd_chars < sizeof(cmd_buf)-2){
            if(UARTAvail() != 0){
                UARTRead(&cmd_buf[recvd_chars], 1);
                UARTWrite(&cmd_buf[recvd_chars], 1);    /* Echo to terminal */
                recvd_chars++;
                
                /* User pressed RETURN */
                if('\r' == cmd_buf[recvd_chars-1]){ //FIXME: Bounds checking
                    cmd_buf[recvd_chars] = '\n';
                    response_chars = esp8266_do_cmd(cmd_buf, resp_buf, sizeof(resp_buf));
                    UARTWrite(resp_buf, response_chars); 
                    break;  
                }
            }else                                
                continue;
        }
    }
}
