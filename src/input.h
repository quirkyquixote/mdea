/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef MDEA_INPUT_H_
#define MDEA_INPUT_H_

#include <stdlib.h>

/*
 * Functions for input type
 */
struct mdea_input_type {
	/* Destructor */
	void (*destroy) (void *);
	/* Get next character */
	int (*get) (void *);
};

/*
 * Data common to all inputs
 */
struct mdea_input {
	/* Pointer to type definition */
	const struct mdea_input_type *type;
	/* Buffer for ungetted characters */
	int buf;
};

/* Destroy */
static inline void mdea_input_destroy(struct mdea_input *i)
{
	i->type->destroy(i);
	free(i);
}

/* Get next character */
static inline int mdea_input_get(struct mdea_input *i)
{
	if (i->buf != -1) {
		int ret = i->buf;
		i->buf = -1;
		return ret;
	}
	return i->type->get(i);
}

/* Return character to input */
static inline void mdea_input_unget(struct mdea_input *i, int c)
{
	i->buf = c;
}

#endif           // MDEA_INPUT_H_
