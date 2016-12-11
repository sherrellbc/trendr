#include <string.h>
#include "logging.h"
#include "esp8266_driver.h"
#include "delay.h"



int ap_connect_ut(void){
    struct ap_node node;
    int ret = 0; 
       
    dlog("\t[%s] Connecting to WiFi Access Point\r\n",  __FUNCTION__);
    memcpy(node.ssid, TEST_AP_SSID, sizeof(TEST_AP_SSID));
    memcpy(node.passwd, TEST_AP_PASSWD, sizeof(TEST_AP_PASSWD));

    /* Disconnect/wait, Connect/wait, Disconnect/wait */
    if(-1 == esp8266_ap_disconnect()){
        ret = -1; 
        goto done;
    }
    delay_us(1*1000*1000);
  
    dlog("\t\t[%s] Connecting to AP:%s\r\n", __FUNCTION__, TEST_AP_SSID);
    if(-1 == esp8266_ap_connect(&node)){
        ret = -1;
        goto done; 
    }
    delay_us(4*1000*1000);
   
    dlog("\t\t[%s] Discnnecting from AP:%s\r\n", __FUNCTION__, TEST_AP_SSID); 
    if(-1 == esp8266_ap_disconnect()){
        ret = -1; 
        goto done; 
    }

done: 
    dlog("\t\t[%s] %s\r\n", __FUNCTION__, (0!=ret)?("FAILED"):("PASSED"));
    
    return ret; 
}
