/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef MDEA_STROUTG_INPUT_H_
#define MDEA_STROUTG_INPUT_H_

#include "output.h"

/* Create string output */
struct mdea_output *mdea_string_output(char **buf, size_t *len);

#endif           // MDEA_STROUTG_INPUT_H_
