/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "error.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

void mdea_error(wchar_t **error, wchar_t *msg, ...)
{
	if (error == NULL)
		return;
	va_list ap;
	va_start(ap, msg);
	*error = calloc(1024, sizeof(**error));
	vswprintf(*error, 1024, msg, ap);
	va_end(ap);
}

