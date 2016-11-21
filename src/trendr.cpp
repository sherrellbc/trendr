#include <stdio.h>
#include <string.h>
#include "i2c_t3.h"

#include "bsp.h"
#include "logging.h"
#include "delay.h"
#include "esp8266_driver.h"
#include "at24cx.h"
#include "util.h"
#include "unit_tests.h"

#define APP_VERSION "1.0"
#define APP_PRELUDE "\r\n\n"                        \
                    "Trendr app\r\n"                \
                    "Version: " APP_VERSION "\r\n"  \
                    "Build Date: " __DATE__ " " __TIME__ "\r\n"

#define GOOG_FIN_STR    "GET http://finance.google.com/finance/info?client=ig&q=NASDAQ:MSFT\n"


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



void sys_init(void){
     Wire.begin(I2C_MASTER, 0x00, I2C_PINS_18_19, I2C_PULLUP_EXT, 400000);
     Wire.setDefaultTimeout(200000);
}


char reply[2048];



int main(void){
    uint8_t ipbuf[4] = {0}; 
//    char reply[1024]; 
    int replylen; 

    /* Init system */
    logging_init();
    sys_init();
    bsp_init();

    /* Provide version information, esp8266 info, etc */
    report_prelude();

    /* Local AP information */
    struct ap_node test_ap;
    memcpy(test_ap.ssid, TEST_AP_SSID, sizeof(TEST_AP_SSID));
    memcpy(test_ap.passwd, TEST_AP_PASSWD, sizeof(TEST_AP_PASSWD));

    /* Remote TCP connection information */
    struct tcp_session remote = {
                                    .ipaddr = {172, 217, 2, 206},
                                    .port = 80,
                                    .status = 0
                                };
    
    struct term_str finance_query_term = {
                                            .str = "]",
                                            .len = 1
                                        };

    dlog("Connecting to AP .. ");
    if(-1 == esp8266_ap_connect(&test_ap))
        return -1; 
    dlog("Connected!\r\n");

    if(-1 != esp8266_get_ipv4_addr((uint8_t (*)[4])ipbuf))
        dlog("Got IP: %d.%d.%d.%d\r\n", ipbuf[0], ipbuf[1], ipbuf[2], ipbuf[3]); 

    while(1){
        memset(reply, 0, sizeof(reply)); 

        if(-1 == esp8266_tcp_open(&remote))
            continue;

        esp8266_tcp_send(&remote, (uint8_t *) GOOG_FIN_STR, sizeof(GOOG_FIN_STR)-1);
        esp8266_read(reply, sizeof(reply), &replylen, &finance_query_term);
        dlog("Recv'd %d bytes; %s\r\n", replylen, reply);

        delay_us(10*1000*1000);
    }

	return 0;
}
