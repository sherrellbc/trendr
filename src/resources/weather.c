#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "logging.h"
#include "http_helper.h"
#include "json_parser.h"


/* Weather API Key */
#define API_KEY "53e636fb6db1457399014941161112"

/* API url (and partial parameter) for querying Google for a current stock value */
static const char *g_api_url = "http://api.worldweatheronline.com/premium/v1/weather.ashx?key="API_KEY"&format=json&fx=noi&date_format=unix";


/* Static buffer in application DATA section; too large for stack */
static char g_reply[2*2048]; 

/* This API source does not respond in a usual manner; This is the termination string received */
static const struct term_str gjson_termstr = {
                                                .str = "]",
                                                .len = 1
                                             };


struct tcp_session weather_remote =     {
                                            .ipaddr = {31,193,9,237},//{31, 193, 9, 236},
                                            .port = 80,
                                            .status = 0
                                        }; 


/*
 * For this to work well we need to first implement some form of key/value pairing
 * such that we may dynamically create a query string. There is not enough time for this.
 * As such, make it simple. If NULL==city, use zipcode else use city
 */
int construct_query(char *buf, const char* city, const char* units, const char *zipcode){
    int retval; 
    const char *query; 

    if(NULL == city)
        query = zipcode;
    else
        query = city; 

//    retval = sprintf(buf, "%s?key="API_KEY"&q=%s", g_api_url, query);
    retval = sprintf(buf, "%s&q=%s", g_api_url, query);
    return retval;   
}



int weather_get(struct tcp_session *session, float (*point)[2], const char *city, const char *zipcode){
    char url_buf[256]; 
    char float_buf[10] = {0}; 
    char time_buf[10] = {0}; 
    int replylen; 

    dlog("Inside weather_get\r\n");
   
    /* Clear static buf before using it */ 
    memset(g_reply, 0, sizeof(g_reply));

    dlog("About to check string lengths\r\n");

    /* Create the query; +1 for nul-term */
    if( (strlen(g_api_url) + strlen(zipcode) + 1) > sizeof(url_buf) )
        return -1;

    dlog("Checked string length\r\n"); 

    if(construct_query(url_buf, city, "imperial", zipcode) < 0)
        return -1;

    dlog("About to http get\r\n"); 
    
    /* Download the JSON response from the remote source */
    if(-1 == http_json_get(session, url_buf, g_reply, sizeof(g_reply), &gjson_termstr, &replylen))
        return -1;
        
    dlog("Got here\r\n");

    /* Set the nul-term and convert the string to a float directly */
    if(0 != json_value_get("temp_F", float_buf, g_reply))
        return -1; 
 
    /* Set the nul-term and convert the string to a float directly */   
     if(0 != json_value_get("observation_time", time_buf, g_reply))
         return -1; 

    dlog("Got eem!\r\n");
    *point[0] = strtof(time_buf, NULL); 
    *point[1] = strtof(float_buf, NULL); /* Propagate error if one occurs */
    return 0;
}
