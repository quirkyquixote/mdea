/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef MDEA_ESCAPE_H_
#define MDEA_ESCAPE_H_

#include <wchar.h>

/* Translate all backlash sequences in str in a newly allocated string; return
 * NULL if there is a parsing error */
char *mdea_escape(const char *str, char **error);

#endif  // MDEA_ESCAPE_H_

