#include <string.h>
#include <stdlib.h>

#include "logging.h"



/* FIXME: Lazy counter; Values/names can have commas/quotations -- Check for escaped quotation */
int json_get_item_count(const char *json_str){
    const char *str = json_str;
    int count = 0;  

    /* Each key/value pair consumes 4 double quotation marks */
    do{
        if('"' == *str)
            count++; 
    }while('\0' != *(++str));
    
    return count/4; 
}

char buf[64]; 

/* Lazy json extractor -- writes nul-term */
/* Assume string has at least one character and is not NULL */
int json_value_get(const char *key, char *value, const char *json_str){
    const char *start, *end; 
    int item_count=0, mode=0, key_match=0;
    start = end = json_str;

    /* Get number of items */
    item_count = json_get_item_count(json_str);

    /* Find the item that matches our key */
    item_count*=2; 
    for(; item_count>0; item_count--){
        /* Find first set of quotations */
        while('"' != *(start++))    /* Stops just AFTER first quote */
            ;

        end = start;

        /* Empty string! */
        if('"' != *start){
            while('"' != *(end+1))      /* Stops just BEFORE second quote */
                end++;       
        }

        /* 0:Key, 1:Value */        
        if(0 == mode){
            mode = 1;

            /* Check if key is an identical match */
            memcpy(buf, start, end-start+1);
            buf[end-start+1] = '\0'; 
#ifdef JSON_DEBUG
            dlog("Comparing %s:%s\r\n", key, buf);
#endif
            if(0 == strncmp(key, start, end-start+1)){
                if(strlen(key) == end-start+1){
                    key_match = 1;  /* Found key */ 
                }
            }
        }else{
            mode = 0; 

            if(0 != key_match){
                /* Extract value; exit */
                /* FIXME: Bounds checking! */
                memcpy(value, start, end-start+1);
                value[end-start+2] = '\0';
                return 0; 
            }
    
            /* Else, Ignore the current value */
        }
        
        /* Put "end" pointer past the second quote; Note that the for loop bounds keeps us within range */
        end+=2; 
        start = end; 
    }
        
    return -1; 
}
