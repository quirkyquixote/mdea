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

	buf2 = malloc(alloc);
	struct mdea_parser *t = mdea_string_parser(buf);
	struct mdea_emitter *e = mdea_string_emitter(buf2, alloc);
	if (mdea_parse(t, e, &error) != 0)
		fprintf(stderr, "ERROR: %s\n", error);
	else
		write(STDOUT_FILENO, buf2, strlen(buf2));
	mdea_parser_destroy(t);
	mdea_emitter_destroy(e);
}

