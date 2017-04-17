/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef MDEA_OUTPUT_H_
#define MDEA_OUTPUT_H_

#include <stdlib.h>

/*
 * Functions for output type
 */
struct mdea_output_type {
	/* Destructor */
	void (*destroy) (void *);
	/* Write string */
	int (*puts) (void *, const char *);
};

/*
 * Data common to all outputs
 */
struct mdea_output {
	/* Pointer to type definition */
	const struct mdea_output_type *type;
};

/* Destroy */
static inline void mdea_output_destroy(struct mdea_output *i)
{
	i->type->destroy(i);
	free(i);
}

/* Write string */
static inline int mdea_output_puts(struct mdea_output *i, const char *str)
{
	return i->type->puts(i, str);
}

#endif           // MDEA_OUTPUT_H_
