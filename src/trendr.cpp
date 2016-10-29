#include <stdio.h>
#include <string.h>
#include "i2c_t3.h"

#include "bsp.h"
#include "logging.h"
#include "delay.h"
#include "esp8266_driver.h"
#include "util.h"

#define APP_VERSION "1.0"
#define APP_PRELUDE "\r\n\n"                        \
                    "Trendr app\r\n"                \
                    "Version: " APP_VERSION "\r\n"    \
                    "Build Date: " __DATE__ " " __TIME__ "\r\n"



static void report_prelude(void){
    char version_buf[64]; 

    /* Collect data on the connected esp8266 */
    if(-1 == esp8266_get_version_info(version_buf, sizeof(version_buf)/sizeof(char))){
        dlog("Error getting version information from ESP8266\r\n");
        return; 
    }

    dlog(APP_PRELUDE);
    dlog("Esp8266 vSDK: %s\r\n", version_buf);   
}



void i2c_scan(void){
    int i; 

    for(i=1; i<127; i++){
        if(0 == i2c_addr_stat(i))
            dlog("Found i2c addr: 0x%.2x\r\n", i);
    }
}



int main(void){
    /* Init system */
    logging_init();
    bsp_init();

    Wire.begin(I2C_MASTER, 0x00, I2C_PINS_18_19, I2C_PULLUP_EXT, 400000);
    Wire.setDefaultTimeout(200000); 
  
    /* Provide version information, esp8266 info, etc */
    report_prelude();

    while(1){
        delay_us(1000*1000);
        i2c_scan();

        /* Send command to module and print response */
        dlog("\n\n= = = = = = = = = = = = = = = = = = = =\r\n");
    }

	return 0; 
}
