/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef MDEA_ARRAY_H_
#define MDEA_ARRAY_H_

#include <stdlib.h>
#include <string.h>

struct mdea_array {
	size_t alloc;
	size_t size;
	void **vals;
};

extern void mdea_destroy(void *);

static inline void mdea_array_init(struct mdea_array *data)
{
	data->alloc = 0;
	data->size = 0;
	data->vals = NULL;
}

static inline void mdea_array_deinit(struct mdea_array *data)
{
	if (data->size) {
		for (size_t i = 0; i < data->size; ++i)
			mdea_destroy(data->vals[i]);
		free(data->vals);
	}
}

static inline int mdea_array_add(struct mdea_array *data, void *val)
{
	if (data->alloc == data->size) {
		data->alloc = data->alloc ? data->alloc * 2 : 2;
		data->vals = realloc(data->vals, sizeof(*data->vals) * data->alloc);
	}
	data->vals[data->size] = val;
	++data->size;
	return 0;
}

static inline int mdea_array_remove(struct mdea_array *data, size_t i)
{
	if (i >= data->size)
		return -1;
	mdea_destroy(data->vals[i]);
	memmove(data->vals + i, data->vals + i + 1,
			sizeof(*data->vals) * (data->size - i - 1));
	--data->size;
	return 0;
}

static inline int mdea_array_get(struct mdea_array *data, size_t key, void **rval)
{
	if (key >= data->size)
		return -1;
	*rval = data->vals[key];
	return 0;
}

#define mdea_array_foreach(_v, _a) \
	for (int _i = 0; (_i != (_a)->size) && (_v = (_a)->vals[_i]); ++_i)

#endif  // MDEA_ARRAY_H_

