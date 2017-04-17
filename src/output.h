/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef MDEA_OUTPUT_H_
#define MDEA_OUTPUT_H_

#include <stdlib.h>

struct mdea_output_type {
	void (*destroy) (void *);
	int (*puts) (void *, const char *);
};

struct mdea_output {
	const struct mdea_output_type *type;
};

static inline void mdea_output_destroy(struct mdea_output *i)
{
	i->type->destroy(i);
	free(i);
}

static inline int mdea_output_puts(struct mdea_output *i, const char *str)
{
	return i->type->puts(i, str);
}

#endif           // MDEA_OUTPUT_H_
