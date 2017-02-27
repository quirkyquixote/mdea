/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef MDEA_H_
#define MDEA_H_

#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <wchar.h>

enum {
	MDEA_NULL,
	MDEA_NUMBER,
	MDEA_STRING,
	MDEA_BOOLEAN,
	MDEA_ARRAY,
	MDEA_OBJECT
};

struct MdeaArray {
	size_t alloc;
	size_t size;
	void **vals;
};

struct MdeaObject {
	size_t alloc;
	size_t size;
	struct MdeaObjectField {
		wchar_t *key;
		void *val;
	} *fields;
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

void mdea_destroy(struct MdeaNode *node);

static inline void mdea_array_init(struct MdeaArray *data)
{
	data->alloc = 0;
	data->size = 0;
	data->vals = NULL;
}

static inline void mdea_array_deinit(struct MdeaArray *data)
{
	if (data->size) {
		for (size_t i = 0; i < data->size; ++i)
			mdea_destroy(data->vals[i]);
		free(data->vals);
	}
}

static inline int mdea_array_add(struct MdeaArray *data, void *val)
{
	if (data->alloc == data->size) {
		data->alloc = data->alloc ? data->alloc * 2 : 2;
		data->vals = realloc(data->vals, sizeof(*data->vals) * data->alloc);
	}
	data->vals[data->size] = val;
	++data->size;
	return 0;
}

static inline int mdea_array_remove(struct MdeaArray *data, size_t i)
{
	if (i >= data->size)
		return -1;
	mdea_destroy(data->vals[i]);
	memmove(data->vals + i, data->vals + i + 1,
			sizeof(*data->vals) * (data->size - i - 1));
	--data->size;
	return 0;
}

static inline int mdea_array_get(struct MdeaArray *data, size_t key, void **rval)
{
	if (key >= data->size)
		return -1;
	*rval = data->vals[key];
	return 0;
}

#define mdea_array_foreach(_v, _a) \
	for (int _i = 0; (_i != (_a)->size) && (_v = (_a)->vals[_i]); ++_i)

static inline void mdea_object_init(struct MdeaObject *data)
{
	data->alloc = 0;
	data->size = 0;
	data->fields = NULL;
}

static inline void mdea_object_deinit(struct MdeaObject *data)
{
	if (data->size) {
		for (size_t i = 0; i < data->size; ++i) {
			free(data->fields[i].key);
			mdea_destroy(data->fields[i].val);
		}
		free(data->fields);
	}
}

static inline int mdea_object_add(struct MdeaObject *data, const wchar_t *key, void *val)
{
	for (size_t i = 0; i < data->size; ++i) {
		if (wcscmp(key, data->fields[i].key) == 0) {
			mdea_destroy(data->fields[i].val);
			data->fields[i].val = val;
			return 0;
		}
	}
	if (data->alloc == data->size) {
		data->alloc = data->alloc ? data->alloc * 2 : 2;
		data->fields = realloc(data->fields,
				sizeof(*data->fields) * data->alloc);
	}
	data->fields[data->size].key = wcsdup(key);
	data->fields[data->size].val = val;
	++data->size;
	return 0;
}

static inline int mdea_object_remove(struct MdeaObject *data, const wchar_t *key)
{
	for (size_t i = 0; i < data->size; ++i) {
		if (wcscmp(key, data->fields[i].key) == 0) {
			free(data->fields[i].key);
			mdea_destroy(data->fields[i].val);
			memmove(data->fields + i, data->fields + i + 1,
					sizeof(*data->fields) * (data->size - i - 1));
			--data->size;
			return 0;
		}
	}
	return -1;
}

static inline int mdea_object_get(struct MdeaObject *data, const wchar_t *key, void **rval)
{
	for (size_t i = 0; i < data->size; ++i) {
		if (wcscmp(key, data->fields[i].key) == 0) {
			*rval = data->fields[i].val;
			return 0;
		}
	}
	return -1;
}

#define mdea_object_foreach(_k, _v, _o) \
	for (int _i = 0; (_i != (_o)->size) && (_k = (_o)->fields[_i].key) && (_v = (_o)->fields[_i].val); ++_i)

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

