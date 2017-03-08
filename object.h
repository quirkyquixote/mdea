/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef MDEA_OBJECT_H_
#define MDEA_OBJECT_H_

#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#include "object.h"

/*
 * Map, indexed by field name
 */
struct mdea_object {
	/* Number of allocated elements */
	size_t alloc;
	/* Number of actual elements */
	size_t size;
	/* Allocated data */
	struct {
		/* Key for the field */
		wchar_t *key;
		/* Value for the field */
		void *val;
	} *fields;
};

/* Forward declaration for function to destroy values */
extern void mdea_destroy(void *);

/* Initialize object */
static inline void mdea_object_init(struct mdea_object *data)
{
	data->alloc = 0;
	data->size = 0;
	data->fields = NULL;
}

/* Deinitialize object */
static inline void mdea_object_deinit(struct mdea_object *data)
{
	if (data->size) {
		for (size_t i = 0; i < data->size; ++i) {
			free(data->fields[i].key);
			mdea_destroy(data->fields[i].val);
		}
		free(data->fields);
	}
}

/* Add field to object */
static inline int mdea_object_add(struct mdea_object *data, const wchar_t *key, void *val)
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

/* Remove field from object */
static inline int mdea_object_remove(struct mdea_object *data, const wchar_t *key)
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

/* Get field from object */
static inline int mdea_object_get(struct mdea_object *data, const wchar_t *key, void **rval)
{
	for (size_t i = 0; i < data->size; ++i) {
		if (wcscmp(key, data->fields[i].key) == 0) {
			*rval = data->fields[i].val;
			return 0;
		}
	}
	return -1;
}

/* Iterate over object fields */
#define mdea_object_foreach(_k, _v, _o) \
	for (int _i = 0; (_i != (_o)->size) && (_k = (_o)->fields[_i].key) && (_v = (_o)->fields[_i].val); ++_i)

#endif  // MDEA_OBJECT_H_

