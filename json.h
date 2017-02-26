/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef JSON_H_
#define JSON_H_

#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <wchar.h>

enum {
	JSON_NULL,
	JSON_NUMBER,
	JSON_STRING,
	JSON_BOOLEAN,
	JSON_ARRAY,
	JSON_OBJECT
};

struct JsonArray {
	size_t alloc;
	size_t size;
	void **vals;
};

struct JsonObject {
	size_t alloc;
	size_t size;
	struct JsonObjectField {
		wchar_t *key;
		void *val;
	} *fields;
};

struct JsonNode {
	int type;
	union {
		double number;
		wchar_t *string;
		int boolean;
		struct JsonArray array;
		struct JsonObject object;
	};
};

void json_destroy(struct JsonNode *node);

static inline void json_array_init(struct JsonArray *data)
{
	data->alloc = 0;
	data->size = 0;
	data->vals = NULL;
}

static inline void json_array_deinit(struct JsonArray *data)
{
	if (data->size) {
		for (size_t i = 0; i < data->size; ++i)
			json_destroy(data->vals[i]);
		free(data->vals);
	}
}

static inline int json_array_add(struct JsonArray *data, void *val)
{
	if (data->alloc == data->size) {
		data->alloc = data->alloc ? data->alloc * 2 : 2;
		data->vals = realloc(data->vals, sizeof(*data->vals) * data->alloc);
	}
	data->vals[data->size] = val;
	++data->size;
	return 0;
}

static inline int json_array_remove(struct JsonArray *data, size_t i)
{
	if (i >= data->size)
		return -1;
	json_destroy(data->vals[i]);
	memmove(data->vals + i, data->vals + i + 1,
			sizeof(*data->vals) * (data->size - i - 1));
	--data->size;
	return 0;
}

static inline int json_array_get(struct JsonArray *data, size_t key, void **rval)
{
	if (key >= data->size)
		return -1;
	*rval = data->vals[key];
	return 0;
}

#define json_array_foreach(_v, _a) \
	for (int _i = 0; (_i != (_a)->size) && (_v = (_a)->vals[_i]); ++_i)

static inline void json_object_init(struct JsonObject *data)
{
	data->alloc = 0;
	data->size = 0;
	data->fields = NULL;
}

static inline void json_object_deinit(struct JsonObject *data)
{
	if (data->size) {
		for (size_t i = 0; i < data->size; ++i) {
			free(data->fields[i].key);
			json_destroy(data->fields[i].val);
		}
		free(data->fields);
	}
}

static inline int json_object_add(struct JsonObject *data, const wchar_t *key, void *val)
{
	for (size_t i = 0; i < data->size; ++i) {
		if (wcscmp(key, data->fields[i].key) == 0) {
			json_destroy(data->fields[i].val);
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

static inline int json_object_remove(struct JsonObject *data, const wchar_t *key)
{
	for (size_t i = 0; i < data->size; ++i) {
		if (wcscmp(key, data->fields[i].key) == 0) {
			free(data->fields[i].key);
			json_destroy(data->fields[i].val);
			memmove(data->fields + i, data->fields + i + 1,
					sizeof(*data->fields) * (data->size - i - 1));
			--data->size;
			return 0;
		}
	}
	return -1;
}

static inline int json_object_get(struct JsonObject *data, const wchar_t *key, void **rval)
{
	for (size_t i = 0; i < data->size; ++i) {
		if (wcscmp(key, data->fields[i].key) == 0) {
			*rval = data->fields[i].val;
			return 0;
		}
	}
	return -1;
}

#define json_object_foreach(_k, _v, _o) \
	for (int _i = 0; (_i != (_o)->size) && (_k = (_o)->fields[_i].key) && (_v = (_o)->fields[_i].val); ++_i)

static inline struct JsonNode *json_null()
{
	struct JsonNode *node = calloc(1, sizeof(*node));
	node->type = JSON_NULL;
	return node;
}

static inline struct JsonNode *json_number(double data)
{
	struct JsonNode *node = calloc(1, sizeof(*node));
	node->type = JSON_NUMBER;
	node->number = data;
	return node;
}

static inline struct JsonNode *json_string(wchar_t *data)
{
	struct JsonNode *node = calloc(1, sizeof(*node));
	node->type = JSON_STRING;
	node->string = wcsdup(data);
	return node;
}

static inline struct JsonNode *json_boolean(int data)
{
	struct JsonNode *node = calloc(1, sizeof(*node));
	node->type = JSON_BOOLEAN;
	node->boolean = data;
	return node;
}

static inline struct JsonNode *json_array(void)
{
	struct JsonNode *node = calloc(1, sizeof(*node));
	node->type = JSON_ARRAY;
	json_array_init(&node->array);
	return node;
}

static inline struct JsonNode *json_object(void)
{
	struct JsonNode *node = calloc(1, sizeof(*node));
	node->type = JSON_OBJECT;
	json_object_init(&node->object);
	return node;
}

static inline int json_get_number(const struct JsonNode *node, double *rval)
{
	if (node->type != JSON_NUMBER)
		return -1;
	*rval = node->number;
	return 0;
}

static inline int json_get_string(const struct JsonNode *node, const wchar_t **rval)
{
	if (node->type != JSON_STRING)
		return -1;
	*rval = node->string;
	return 0;
}

static inline int json_get_boolean(const struct JsonNode *node, int *rval)
{
	if (node->type != JSON_BOOLEAN)
		return -1;
	*rval = node->boolean;
	return 0;
}

static inline int json_get_array(struct JsonNode *node, struct JsonArray **rval)
{
	if (node->type != JSON_ARRAY)
		return -1;
	*rval = &node->array;
	return 0;
}

static inline int json_get_object(struct JsonNode *node, struct JsonObject **rval)
{
	if (node->type != JSON_OBJECT)
		return -1;
	*rval = &node->object;
	return 0;
}

int json_read(FILE *f, struct JsonNode **node, wchar_t **error);
int json_write(FILE *f, struct JsonNode *node, int indent, wchar_t **error);
int json_get(struct JsonNode *root, struct JsonNode **rval, ...);

#endif  // JSON_H_

