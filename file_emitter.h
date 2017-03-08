/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef MDEA_FILE_EMITTER_H_
#define MDEA_FILE_EMITTER_H_

#include <stdio.h>

#include "emitter.h"

struct mdea_emitter *mdea_file_emitter(FILE *file);

#endif  // MDEA_FILE_EMITTER_H_

