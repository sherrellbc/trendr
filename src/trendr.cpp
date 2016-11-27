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
#include "resources.h"
#include "display_plot.h"

#define APP_VERSION "1.0"
#define APP_PRELUDE "\r\n\n"                        \
                    "Trendr app\r\n"                \
                    "Version: " APP_VERSION "\r\n"  \
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


/* FIXME: Currently only valid cases work */
static const char *stock_list[] = {"TSLA", "HPQ", "FIT", "M", "BAC", "AAPL"};

/* Fixed values for testing the display plotting function */
static const float g_point_array[][2] =     {
                                                {0,  10},
                                                {10, -25},
                                                {20, 40},
                                                {30, 75}
                                            };
                                    


int main(void){
    uint8_t ipbuf[4] = {0}; 
    float stock_val; 
    unsigned int i; 

    /* Init system */
    logging_init();
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

    dlog("Connecting to AP .. ");
    if(-1 == esp8266_ap_connect(&test_ap))
        return -1; 
    dlog("Connected!\r\n");

    if(-1 != esp8266_get_ipv4_addr((uint8_t (*)[4])ipbuf))
        dlog("Got IP: %d.%d.%d.%d\r\n", ipbuf[0], ipbuf[1], ipbuf[2], ipbuf[3]);

    /* Display test */
    display_draw_plot(g_point_array, sizeof(g_point_array)/sizeof(float)/2);  

    while(1){
        for(i=0; i<sizeof(stock_list)/sizeof(char *); i++){
            /* Open TCP session */
            if(-1 == esp8266_tcp_open(&remote))
                break;;

            /* Query stock value */
            stock_val = stocks_get(&remote, stock_list[i]);
            
            if(stock_val >= 0)
                dlog("%s Stock Value: %.2f\r\n", stock_list[i], stock_val);
            else
                dlog("%s is not a valid stock handle\r\n", stock_list[i]);
        }

        dputs("\r\n");
        delay_us(5*1000*1000);
    }

	return 0;
}
