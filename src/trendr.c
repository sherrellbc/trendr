#include <stdio.h>
#include <string.h>

#include "bsp.h"
#include "logging.h"
#include "common.h"
#include "delay.h"
#include "esp8266_driver.h"
#include "uart.h"
#include "util.h"



int main(void){
    char buf[1024]; 
    int recvd_chars;

    bsp_init(); 
    logging_init();
    dlog("Beginning comms\r\n");

    while(1){
        delay_us(1000*1000);

        /* Send command to module and print response */
        memset(buf, ' ', sizeof(buf));
        dlog("\n\n= = = = = = = = = = = = = = = = = = = =\r\n");
        recvd_chars = esp8266_do_cmd("AT\r\n", buf, sizeof(buf));

        if(-1 == recvd_chars){
            dlog("\nAn error occurred .. \r\n");
            continue;     
        }

        dlog("\nESP8266 Responded:\r\n%s\n\n", buf);
    }

	return 0; 
}
