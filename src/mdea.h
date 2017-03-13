/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef MDEA_H_
#define MDEA_H_

#include "error.h"
#include "escape.h"
#include "null_node.h"
#include "number_node.h"
#include "string_node.h"
#include "boolean_node.h"
#include "array_node.h"
#include "object_node.h"
#include "file_parser.h"
#include "string_parser.h"
#include "node_parser.h"
#include "file_emitter.h"
#include "string_emitter.h"
#include "node_emitter.h"

int mdea_get(struct mdea_node *root, char *key, struct mdea_node **rval, char **error);
int mdea_set(struct mdea_node **root, char *key, struct mdea_node *val, char **error);

#endif  // MDEA_H_

