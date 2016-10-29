#ifndef _LOGGING_H
#define _LOGGING_H

#include "stddef.h"

#ifdef __cplusplus
extern "C" {
#endif


void logging_init(void);

/* Print a character and hex value to the DEBUG serial interface
 * 
 * @param c : The character to print
 */
void print_char(char c);


/*
 *
 *
 *
 */
int dlog(const char *fmt, ...);


void dputchar(char c);


void dputs(char const * const str);


#ifdef __cplusplus
}
#endif

#endif /* _LOGGING_H */
