/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef MDEA_ERROR_H_
#define MDEA_ERROR_H_

#include <wchar.h>

void mdea_error(wchar_t **error, wchar_t *msg, ...);

#endif  // MDEA_ERROR_H_
