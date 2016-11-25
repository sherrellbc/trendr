#ifndef _JSON_PARSER_H
#define _JSON_PARSER_H


#ifdef __cplusplus
extern "C" {
#endif



/*
 * Return the number of objects in the JSON list
 *
 * @param json_str  : The JSON string
 *
 * @return          : The number of objects or -1 on failure 
 */
int json_get_item_count(const char *json_str);



/*
 * Return the value corresponding to the requested key from a JSON list
 *
 * @param key       : String representing the key to find 
 * @param value     : [OUT] Pointer to location to store the extracted value 
 * @param json_str  : The JSON string 
 *
 * @return          : -1 on failure 
 */
int json_value_get(const char *key, char *value, const char *json_str);



#ifdef __cplusplus
}
#endif


#endif  /* _JSON_PARSER_H */
