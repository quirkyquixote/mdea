/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef MDEA_JSON_EMITTER_H_
#define MDEA_JSON_EMITTER_H_

#include "emitter.h"
#include "output.h"

/* Create json emitter */
struct mdea_emitter *mdea_json_emitter(struct mdea_output *out);

#endif  // MDEA_JSON_EMITTER_H_

