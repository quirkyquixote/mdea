/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef MDEA_ERROR_H_
#define MDEA_ERROR_H_

#include <wchar.h>

/* If error is not NULL, initialize it from the printf-like fmt string and
 * extra arguments */
void mdea_error(wchar_t **error, wchar_t *fmt, ...);

#endif  // MDEA_ERROR_H_
