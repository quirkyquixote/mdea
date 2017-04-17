/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef MDEA_JSON_PARSER_H_
#define MDEA_JSON_PARSER_H_

#include "parser.h"
#include "input.h"

/* Create json parser */
struct mdea_parser *mdea_json_parser(struct mdea_input *in);

#endif  // MDEA_JSON_PARSER_H_

