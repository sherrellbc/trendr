#ifndef _WEATHER_H
#define _WEATHER_H

#include "esp8266_driver.h"


#ifdef __cplusplus
extern "C" {
#endif



/*
 * Return the current temperature for the specified city
 *
 * @param city      : A null-terminated string representing a city to query
 * @param zipcode   : The zipcode of the location to query
 *
 * @return  : -1 on failure
 * 
 * Note: Zipcode and City are mutually exclude. City takes precedence
 */
float weather_get(const char *city, const char *zipcode);



#ifdef __cplusplus
}
#endif


#endif  /* _WEATHER_H */
