/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef MDEA_H_
#define MDEA_H_

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <wchar.h>

#include "error.h"
#include "escape.h"
#include "array.h"
#include "object.h"

enum {
	MDEA_NULL,
	MDEA_NUMBER,
	MDEA_STRING,
	MDEA_BOOLEAN,
	MDEA_ARRAY,
	MDEA_OBJECT
};

struct MdeaNode {
	int type;
	union {
		double number;
		wchar_t *string;
		int boolean;
		struct MdeaArray array;
		struct MdeaObject object;
	};
};

void mdea_destroy(void *node);

static inline struct MdeaNode *mdea_null()
{
	struct MdeaNode *node = calloc(1, sizeof(*node));
	node->type = MDEA_NULL;
	return node;
}

static inline struct MdeaNode *mdea_number(double data)
{
	struct MdeaNode *node = calloc(1, sizeof(*node));
	node->type = MDEA_NUMBER;
	node->number = data;
	return node;
}

static inline struct MdeaNode *mdea_string(wchar_t *data)
{
	struct MdeaNode *node = calloc(1, sizeof(*node));
	node->type = MDEA_STRING;
	node->string = wcsdup(data);
	return node;
}

static inline struct MdeaNode *mdea_boolean(int data)
{
	struct MdeaNode *node = calloc(1, sizeof(*node));
	node->type = MDEA_BOOLEAN;
	node->boolean = data;
	return node;
}

static inline struct MdeaNode *mdea_array(void)
{
	struct MdeaNode *node = calloc(1, sizeof(*node));
	node->type = MDEA_ARRAY;
	mdea_array_init(&node->array);
	return node;
}

static inline struct MdeaNode *mdea_object(void)
{
	struct MdeaNode *node = calloc(1, sizeof(*node));
	node->type = MDEA_OBJECT;
	mdea_object_init(&node->object);
	return node;
}

static inline int mdea_get_number(const struct MdeaNode *node, double *rval)
{
	if (node->type != MDEA_NUMBER)
		return -1;
	*rval = node->number;
	return 0;
}

static inline int mdea_get_string(const struct MdeaNode *node, const wchar_t **rval)
{
	if (node->type != MDEA_STRING)
		return -1;
	*rval = node->string;
	return 0;
}

static inline int mdea_get_boolean(const struct MdeaNode *node, int *rval)
{
	if (node->type != MDEA_BOOLEAN)
		return -1;
	*rval = node->boolean;
	return 0;
}

static inline int mdea_get_array(struct MdeaNode *node, struct MdeaArray **rval)
{
	if (node->type != MDEA_ARRAY)
		return -1;
	*rval = &node->array;
	return 0;
}

static inline int mdea_get_object(struct MdeaNode *node, struct MdeaObject **rval)
{
	if (node->type != MDEA_OBJECT)
		return -1;
	*rval = &node->object;
	return 0;
}

int mdea_read(FILE *f, struct MdeaNode **node, wchar_t **error);
int mdea_write(FILE *f, struct MdeaNode *node, int indent, wchar_t **error);
int mdea_get(struct MdeaNode *root, struct MdeaNode **rval, ...);

#endif  // MDEA_H_

