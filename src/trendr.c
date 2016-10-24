#include <stdio.h>
#include <string.h>

#include "bsp.h"
#include "logging.h"
#include "common.h"
#include "delay.h"
#include "esp8266_driver.h"
#include "uart.h"
#include "util.h"

#define CLI_VERSION "1.0"
#define CLI_PRELUDE "\r\n\n"                        \
                    "Trendr app\r\n"                \
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

    /* Init system */
    logging_init();
    bsp_init(); 
  
    /* Provide version information, esp8266 info, etc */
    report_prelude();

    while(1){
        delay_us(1000*1000);

        /* Send command to module and print response */
        dlog("\n\n= = = = = = = = = = = = = = = = = = = =\r\n");
    }

	return 0; 
}
