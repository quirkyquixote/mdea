/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "string_input.h"

struct mdea_string_input {
	struct mdea_input input;
	const char *ptr;
};

void mdea_string_input_destroy(void *ptr)
{
}

int mdea_string_input_get(void *ptr)
{
	struct mdea_string_input *i = ptr;
	if (*i->ptr == 0)
		return -1;
	return *(i->ptr++);
}

static const struct mdea_input_type mdea_string_input_type = {
	mdea_string_input_destroy,
	mdea_string_input_get
};

struct mdea_input *mdea_string_input(const char *str)
{
	struct mdea_string_input *i = calloc(1, sizeof(*i));
	i->input.type = &mdea_string_input_type;
	i->input.buf = -1;
	i->ptr = str;
	return (void *)i;
}
