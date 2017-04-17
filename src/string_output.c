/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "string_output.h"

#include <string.h>

struct mdea_string_output {
	struct mdea_output output;
	char **buf;
	size_t *len;
	size_t used;
};

void mdea_string_output_destroy(void *ptr)
{
}

int mdea_string_output_puts(void *ptr, const char *str)
{
	struct mdea_string_output *i = ptr;
	size_t len = strlen(str);
	if (*i->len < i->used + len + 1) {
		do
			*i->len = *i->len ? *i->len * 2 : 2;
		while (*i->len < i->used + len + 1);
		*i->buf = realloc(*i->buf, *i->len);
	}
	strcpy(*i->buf + i->used, str);
	i->used += len;
	return len;
}

static const struct mdea_output_type mdea_string_output_type = {
	mdea_string_output_destroy,
	mdea_string_output_puts
};

struct mdea_output *mdea_string_output(char **buf, size_t *len)
{
	struct mdea_string_output *i = calloc(1, sizeof(*i));
	i->output.type = &mdea_string_output_type;
	i->buf = buf;
	i->len = len;
	return (void *)i;
}
