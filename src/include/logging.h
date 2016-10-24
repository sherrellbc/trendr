#ifndef _LOGGING_H
#define _LOGGING_H

#include "stddef.h"


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


int dputchar(char c);


int dputs(char const * const str);



#endif /* _LOGGING_H */
