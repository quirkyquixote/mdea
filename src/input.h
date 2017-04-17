/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef MDEA_INPUT_H_
#define MDEA_INPUT_H_

#include <stdlib.h>

struct mdea_input_type {
	void (*destroy) (void *);
	int (*get) (void *);
};

struct mdea_input {
	const struct mdea_input_type *type;
	int buf;
};

static inline void mdea_input_destroy(struct mdea_input *i)
{
	i->type->destroy(i);
	free(i);
}

static inline int mdea_input_get(struct mdea_input *i)
{
	if (i->buf != -1) {
		int ret = i->buf;
		i->buf = -1;
		return ret;
	}
	return i->type->get(i);
}

static inline void mdea_input_unget(struct mdea_input *i, int c)
{
	i->buf = c;
}

#endif           // MDEA_INPUT_H_
