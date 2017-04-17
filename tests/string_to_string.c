/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include <stdio.h>
#include <unistd.h>

#include "mdea.h"

int main(int argc, char *argv[])
{
	size_t alloc = 0, len = 0;
	char *buf = NULL, *buf2, *error;

	do {
		alloc += 256;
		buf = realloc(buf, alloc);
		len += read(STDIN_FILENO, buf + len, alloc - len);
	} while (len == alloc);
	buf[len] = 0;

	buf2 = malloc(alloc);
	struct mdea_input *i = mdea_string_input(buf);
	struct mdea_parser *t = mdea_json_parser(i);
	struct mdea_output *o = mdea_string_output(&buf2, &alloc);
	struct mdea_emitter *e = mdea_json_emitter(o);
	if (mdea_parse(t, e, &error) != 0)
		fprintf(stderr, "ERROR: %s\n", error);
	else
		write(STDOUT_FILENO, buf2, strlen(buf2));
	mdea_parser_destroy(t);
	mdea_emitter_destroy(e);
	mdea_input_destroy(i);
	mdea_output_destroy(o);
}

