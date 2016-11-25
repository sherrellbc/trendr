#include <string.h>
#include "logging.h"
#include "esp8266_driver.h"
#include "delay.h"



int ap_connect_ut(void){
    struct ap_node node;
    int ret = 0; 
    
    memcpy(node.ssid, TEST_AP_SSID, sizeof(TEST_AP_SSID));
    memcpy(node.passwd, TEST_AP_PASSWD, sizeof(TEST_AP_PASSWD));

    /* Disconnect/wait, Connect/wait, Disconnect/wait */
    if(-1 == esp8266_ap_disconnect()){
        ret = -1; 
        goto done;
    }
    delay_us(1*1000*1000);
  
    if(-1 == esp8266_ap_connect(&node)){
        ret = -1;
        goto done; 
    }
    delay_us(4*1000*1000);
    
    if(-1 == esp8266_ap_disconnect()){
        ret = -1; 
        goto done; 
    }

done: 
    if(0 != ret)
        dlog("Failed %s unit test\r\n", __FUNCTION__);

    return ret; 
}
