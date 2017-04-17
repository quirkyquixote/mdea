/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "file_output.h"

#include <unistd.h>
#include <string.h>

struct mdea_file_output {
	struct mdea_output output;
	int desc;
};

void mdea_file_output_destroy(void *ptr)
{
}

int mdea_file_output_puts(void *ptr, const char *str)
{
	struct mdea_file_output *i = ptr;
	return write(i->desc, str, strlen(str));
}

static const struct mdea_output_type mdea_file_output_type = {
	mdea_file_output_destroy,
	mdea_file_output_puts
};

struct mdea_output *mdea_file_output(int desc)
{
	struct mdea_file_output *i = calloc(1, sizeof(*i));
	i->output.type = &mdea_file_output_type;
	i->desc = desc;
	return (void *)i;
}
