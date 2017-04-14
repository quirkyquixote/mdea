/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef MDEA_OBJECT_H_
#define MDEA_OBJECT_H_

#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#include "object.h"

/* Forward declaration of mdea_node */
struct mdea_node;

/*
 * Map, indexed by field name
 */
struct mdea_object {
	/* Number of allocated elements */
	size_t alloc;
	/* Number of actual elements */
	size_t size;
	/* Allocated o */
	struct {
		/* Key for the field */
		char *key;
		/* Value for the field */
		struct mdea_node *val;
	} *fields;
};

/* Forward declaration of function to reference nodes */
extern void mdea_ref(struct mdea_node *);
/* Forward declaration of function to dereference nodes */
extern void mdea_unref(struct mdea_node *);

/* Initialize object */
static inline void mdea_object_init(struct mdea_object *o)
{
	o->alloc = 0;
	o->size = 0;
	o->fields = NULL;
}

/* Clear the contents */
static inline void mdea_object_clear(struct mdea_object *o)
{
	for (size_t i = 0; i < o->size; ++i) {
		free(o->fields[i].key);
		mdea_unref(o->fields[i].val);
	}
	o->size = 0;
}

/* Deinitialize object */
static inline void mdea_object_deinit(struct mdea_object *o)
{
	mdea_object_clear(o);
	if (o->fields)
		free(o->fields);
}

/* Checks whether object is empty */
static inline int mdea_object_empty(struct mdea_object *o)
{
	return o->size == 0;
}

/* Return number of fields */
static inline size_t mdea_object_size(struct mdea_object *o)
{
	return o->size;
}

/* Add field to object */
static inline int mdea_object_insert(struct mdea_object *o, const char *key, struct mdea_node *val)
{
	for (size_t i = 0; i < o->size; ++i) {
		if (strcmp(key, o->fields[i].key) == 0) {
			mdea_unref(o->fields[i].val);
			o->fields[i].val = val;
			return 0;
		}
	}
	if (o->alloc == o->size) {
		o->alloc = o->alloc ? o->alloc * 2 : 2;
		o->fields = realloc(o->fields,
				sizeof(*o->fields) * o->alloc);
	}
	o->fields[o->size].key = strdup(key);
	o->fields[o->size].val = val;
	++o->size;
	mdea_ref(val);
	return 0;
}

/* Remove field from object */
static inline int mdea_object_erase(struct mdea_object *o, const char *key)
{
	for (size_t i = 0; i < o->size; ++i) {
		if (strcmp(key, o->fields[i].key) == 0) {
			free(o->fields[i].key);
			mdea_unref(o->fields[i].val);
			memmove(o->fields + i, o->fields + i + 1,
					sizeof(*o->fields) * (o->size - i - 1));
			--o->size;
			return 0;
		}
	}
	return -1;
}

/* Get field from object */
static inline int mdea_object_get(struct mdea_object *o, const char *key, struct mdea_node **rval)
{
	for (size_t i = 0; i < o->size; ++i) {
		if (strcmp(key, o->fields[i].key) == 0) {
			*rval = o->fields[i].val;
			mdea_ref(*rval);
			return 0;
		}
	}
	return -1;
}

/* Iterate over object fields */
#define mdea_object_foreach(_k, _v, _o) \
	for (int _i = 0; (_i != (_o)->size) && (_k = (_o)->fields[_i].key) && (_v = (_o)->fields[_i].val); ++_i)

#endif  // MDEA_OBJECT_H_

