/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef MDEA_H_
#define MDEA_H_

#include "error.h"
#include "escape.h"
#include "array.h"
#include "object.h"
#include "node.h"

int mdea_read(FILE *f, struct MdeaNode **node, wchar_t **error);
int mdea_write(FILE *f, struct MdeaNode *node, wchar_t **error);
int mdea_get(struct MdeaNode *root, struct MdeaNode **rval, ...);

#endif  // MDEA_H_

