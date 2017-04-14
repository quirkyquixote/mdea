/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef MDEA_ARRAY_H_
#define MDEA_ARRAY_H_

#include <stdlib.h>
#include <string.h>

/* Forward declaration for mdea_node */
struct mdea_node;

/* 
 * Self-allocating array
 */
struct mdea_array {
	/* Number of allocated elements */
	size_t alloc;
	/* Number of actual elements */
	size_t size;
	/* Pointer to data */
	struct mdea_node **vals;
};

/* Forward declaration of function to reference nodes */
extern void mdea_ref(struct mdea_node *);
/* Forward declaration of function to dereference nodes */
extern void mdea_unref(struct mdea_node *);

/* Initialize array */
static inline void mdea_array_init(struct mdea_array *a)
{
	a->alloc = 0;
	a->size = 0;
	a->vals = NULL;
}

/* Clears the contents */
static inline void mdea_array_clear(struct mdea_array *a)
{
	for (size_t i = 0; i < a->size; ++i)
		mdea_unref(a->vals[i]);
	a->size = 0;
}

/* Deinitialize array */
static inline void mdea_array_deinit(struct mdea_array *a)
{
	mdea_array_clear(a);
	if (a->vals)
		free(a->vals);
}

/* Return 1 if empty, 0 otherwise */
static inline int mdea_array_empty(struct mdea_array *array)
{
	return array->size == 0;
}

/* Return number of elements */
static inline size_t mdea_array_size(struct mdea_array *array)
{
	return array->size;
}

/* Insert element */
static inline int mdea_array_insert(struct mdea_array *a, size_t i, struct mdea_node *val)
{
	if (i > a->size)
		return -1;
	if (a->alloc == a->size) {
		a->alloc = a->alloc ? a->alloc * 2 : 2;
		a->vals = realloc(a->vals, sizeof(*a->vals) * a->alloc);
	}
	memmove(a->vals + i + 1, a->vals + i,
			sizeof(*a->vals) * (a->size - i));
	a->vals[i] = val;
	++a->size;
	mdea_ref(val);
	return 0;
}

/* Remove element */
static inline int mdea_array_erase(struct mdea_array *a, size_t i)
{
	if (i >= a->size)
		return -1;
	mdea_unref(a->vals[i]);
	memmove(a->vals + i, a->vals + i + 1,
			sizeof(*a->vals) * (a->size - i - 1));
	--a->size;
	return 0;
}

/* Add element to the end */
static inline int mdea_array_push_back(struct mdea_array *a, struct mdea_node *val)
{
	if (a->alloc == a->size) {
		a->alloc = a->alloc ? a->alloc * 2 : 2;
		a->vals = realloc(a->vals, sizeof(*a->vals) * a->alloc);
	}
	a->vals[a->size] = val;
	++a->size;
	mdea_ref(val);
	return 0;
}

/* Remove element from the end */
static inline int mdea_array_pop_back(struct mdea_array *a)
{
	if (a->size == 0)
		return -1;
	--a->size;
	mdea_unref(a->vals[a->size]);
	return 0;
}

/* Get element from array */
static inline int mdea_array_get(struct mdea_array *a, size_t key, struct mdea_node **rval)
{
	if (key >= a->size)
		return -1;
	*rval = a->vals[key];
	mdea_ref(*rval);
	return 0;
}

/* Get last element from array */
static inline int mdea_array_get_back(struct mdea_array *a, struct mdea_node **rval)
{
	if (a->size == 0)
		return -1;
	*rval = a->vals[a->size - 1];
	mdea_ref(*rval);
	return 0;
}

/* Iterate over array */
#define mdea_array_foreach(_v, _a) \
	for (int _i = 0; (_i != (_a)->size) && (_v = (_a)->vals[_i]); ++_i)

#endif  // MDEA_ARRAY_H_

