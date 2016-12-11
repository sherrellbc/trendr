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
#include "simple_fs.h"

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
static const char *stock_list[] = {"TSLA"};//, "HPQ", "FIT", "M", "BAC", "AAPL"};
static const char *city_list[] = {"London", "20001"};

/* Larger test point array for generating random data to  plot */
static int g_point_array[100][2];
static int point_tmp[100];


//FIXME: Ensure we reconnect to the AP if disconnected 
//- Need simpler infrastructure to do so (WiFi abstraction module) 
void collect_loop(void){
    char buf[64]; 
    float stock_val;
    unsigned int i,j;
    unsigned int len = 0; 

    /* First, load from memory if available */
    dlog("Loading past data from memory .. \r\n");
    if(-1 == sfs_load(STOCK_DATA, (uint8_t *)point_tmp, sizeof(point_tmp), &len)){
        dlog("No historical data found in memory; starting anew\r\n");
        memset(g_point_array, 0, sizeof(g_point_array));
        len = 0;
    }else{
        dlog("Found %d bytes (%d objects) of historical data in memory!\r\n", len, len/sizeof(int));
        len/=sizeof(int); 
        /* 
         * FIXME: Two copies of the same data .. there is a conflict with the format stored to memory and what
         * is sent to the plotting routines. The "x" data is not currently saved ..  
         */
        for(i=0; i<len; i++){
            g_point_array[i][0] = i;
            g_point_array[i][1] = point_tmp[i]; 
            dlog("%d,%d\r\n", i, g_point_array[i][1]);
        }
    }

    dlog("Beginning data collection ... \r\n"); 
    while(1){
        for(i=0; i<sizeof(stock_list)/sizeof(char *); i++){
            /* Query stock value */
            stock_val = stocks_get(stock_list[i]);
            
            if(stock_val >= 0){
                dlog("[idx=%d] %s Stock Value: %.2f\r\n", len, stock_list[i], stock_val);
                g_point_array[len][0] = len;
                g_point_array[len][1] = (int) stock_val; 
                len++;
            
                /*
                 * FIXME: We need to save both x,y data; not doing that breaks a lot of things
                 * Until then, copy only the y data to temporary memory and store it 
                 */
                for(j=0; j<len; j++)    
                    point_tmp[j] = g_point_array[j][1];             

                sfs_store(STOCK_DATA, (uint8_t*)point_tmp, len*sizeof(int));

                sprintf(buf, "\"%s\" Stock Price", stock_list[i]);
                display_draw_plot(buf, "Samples", "Price, USD", g_point_array, len);
                
                /* Overflow to beginning of array */
                if(0 == len%(sizeof(g_point_array)/sizeof(int)/2))
                    len = 0;

            }
            else
                dlog("%s is not a valid stock handle\r\n", stock_list[i]);
        }

        delay_us(2*1000*1000);
    }
}



/*
 * Initialize system/board peripherals, connect to the AP and drop into the collection loop
 */ 
int main(void){
    uint8_t ipbuf[4] = {0}; 

    /* Init system */
    srand(1251413);
    logging_init();
    bsp_init();
#ifdef EXECUTE_UNIT_TESTS
    do_unit_tests(0);
#endif

    /* Provide version information, esp8266 info, etc */
    report_prelude();

    /* Local AP information */
    struct ap_node test_ap;
    memcpy(test_ap.ssid, TEST_AP_SSID, sizeof(TEST_AP_SSID));
    memcpy(test_ap.passwd, TEST_AP_PASSWD, sizeof(TEST_AP_PASSWD));

    /* Connect to AP before opening TCP sessions */
    dlog("Connecting to AP .. ");
    if(-1 == esp8266_ap_connect(&test_ap))
        return -1; 
    dlog("Connected!\r\n");

    if(-1 != esp8266_get_ipv4_addr((uint8_t (*)[4])ipbuf))
        dlog("Got IP: %d.%d.%d.%d\r\n", ipbuf[0], ipbuf[1], ipbuf[2], ipbuf[3]);

    /* Loop forever collecting/storing/plotting data */
    while(1)
        collect_loop();

	return 0;
}


//g_dyn_point_array[parray_len][0] = (float) (rand());
//g_dyn_point_array[parray_len][1] = (float) (rand());
//
//parray_len++;
//display_draw_plot("Test Title", (float (*)[2]) g_dyn_point_array, parray_len);
//
//if(0 == (parray_len%100))
//    parray_len = 0;


//#if 1 
//        for(i=0; i<sizeof(city_list)/sizeof(char *); i++){
//            /* Open TCP session */
//            if(-1 == esp8266_tcp_open(&weather_remote))
//                break;
//
//            /* Get the current weather */
//            if(-1 == weather_get(&weather_remote, (float (*)[2])weather_val, NULL, "21224"))
//                dlog("Something went wrong\r\n");
//            else
//                dlog("Current weather: Time=%f, Temp=%d\r\n", weather_val[0], weather_val[1]);
//
//            esp8266_tcp_close(&weather_remote);
//        }
//#endif












