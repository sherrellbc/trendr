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

#define MAX_TREND_DATAPOINTS    100

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
static const char *city_list[] = {"London"};

/* Larger test point array for generating random data to  plot */
static int g_stock_array[MAX_TREND_DATAPOINTS][2];
static int g_weather_array[MAX_TREND_DATAPOINTS][2];
static int point_tmp[MAX_TREND_DATAPOINTS];



int load_trend_data(enum trend_data dataset, int dest[][2], unsigned int *len){
    unsigned int i; 

    dlog("Loading past data from memory .. \r\n");
    if(-1 == sfs_load(dataset, (uint8_t *)point_tmp, sizeof(point_tmp), len)){
        dlog("No historical data found in memory; starting anew\r\n");
        return -1;
    }else{
        dlog("Found %d bytes (%d objects) of historical data in memory!\r\n", *len, *len/sizeof(int));
        *len/=sizeof(int);
 
        /* 
         * FIXME: Two copies of the same data .. there is a conflict with the format stored to memory and what
         * is sent to the plotting routines. The "x" data is not currently saved ..  
         */
        for(i=0; i<*len; i++){
            dest[i][0] = i;
            dest[i][1] = point_tmp[i]; 
            dlog("%d,%d\r\n", i, dest[i][1]);
        }
    }

    return 0;
}



/*                                                                               
 * FIXME: We need to save both x,y data; not doing that breaks a lot of things
 * Until then, copy only the y data to temporary memory and store it
 * This works because the temp array is defined to be the same length as the storage arrays 
 */
int store_trend_data(enum trend_data dataset, int dest[][2], unsigned int len){
    int i; 
    for(i=0; i<len; i++)    
        point_tmp[i] = dest[i][1];

    sfs_store(dataset, (uint8_t*)point_tmp, len*sizeof(int));
    return 0;
}



//FIXME: Ensure we reconnect to the AP if disconnected 
//- Need simpler infrastructure to do so (WiFi abstraction module) 
void collect_loop(void){
    char buf[64]; 
    float stock_val;
    float weather_val; 
    unsigned int query_count = 0;
    unsigned int do_stock_plot = 0; /* Quick hack for altering plots */
     
    unsigned int len_stock, len_weather; 
    
    /* Load data from memory if available, else clear to zero */
    if(-1 == load_trend_data(STOCK_DATA, g_stock_array, &len_stock)){
        memset(g_stock_array, 0, sizeof(g_stock_array));
        len_stock = 0;
    }

    if(-1 == load_trend_data(WEATHER_DATA, g_weather_array, &len_weather)){
        memset(g_weather_array, 0, sizeof(g_weather_array));
        len_weather = 0;
    }
 
    dlog("Beginning data collection ... \r\n"); 
    while(1){
        if(0 == (query_count++)%10)
            do_stock_plot = ~do_stock_plot;

        /* Query stock value */
        stock_val = stocks_get(stock_list[0]);
        weather_val = weather_get(city_list[0], NULL);
       
        /* Check and save the stock data */ 
        if(stock_val >= 0){
            dlog("[idx=%d] %s Stock Value: %.2f", len_stock, stock_list[0], stock_val);
            g_stock_array[len_stock][0] = len_stock;
            g_stock_array[len_stock][1] = (int) stock_val; 
            len_stock++;
        
            store_trend_data(STOCK_DATA, g_stock_array, len_stock);
            sprintf(buf, "\"%s\" Stock Price", stock_list[0]);
    
            if(do_stock_plot)
                display_draw_plot(buf, "Samples", "Price, USD", g_stock_array, len_stock);
            
            /* Overflow to beginning of array */
            if(0 == len_stock%(sizeof(g_stock_array)/sizeof(int)/2))
                len_stock = 0;

        }
        else
            dlog("%s is not a valid stock handle\r\n", stock_list[0]);

        /* Check and save the weather data */
        if(weather_val >= 0){
            dlog("\t\t[idx=%d] %s Weather Value: %.2f\r\n", len_weather, city_list[0], weather_val);
            g_weather_array[len_weather][0] = len_weather;
            g_weather_array[len_weather][1] = (int) weather_val; 
            len_weather++;

            store_trend_data(WEATHER_DATA, g_weather_array, len_weather);
            sprintf(buf, "\"%s\" Weather", city_list[0]);
            
            if(!do_stock_plot)
                display_draw_plot(buf, "Samples", "Temperature, F", g_weather_array, len_weather);
            
            /* Overflow to beginning of array */
            if(0 == len_weather%(sizeof(g_weather_array)/sizeof(int)/2))
                len_weather = 0;
                                                                                            
        }
        else
            dlog("There was a problem collecting weather data for \"%s\"\r\n", city_list[0]);

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
