/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "file_input.h"

#include <unistd.h>

struct mdea_file_input {
	struct mdea_input input;
	int desc;
};

void mdea_file_input_destroy(void *ptr)
{
}

int mdea_file_input_get(void *ptr)
{
	struct mdea_file_input *i = ptr;
	unsigned char c;
	if (read(i->desc, &c, 1) <= 0)
		return -1;
	return c;
}

static const struct mdea_input_type mdea_file_input_type = {
	mdea_file_input_destroy,
	mdea_file_input_get
};

struct mdea_input *mdea_file_input(int desc)
{
	struct mdea_file_input *i = calloc(1, sizeof(*i));
	i->input.type = &mdea_file_input_type;
	i->input.buf = -1;
	i->desc = desc;
	return (void *)i;
}
