/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include <stdio.h>
#include <unistd.h>

#include "mdea.h"

int main(int argc, char *argv[])
{
	char *error;
	struct mdea_parser *t = mdea_file_parser(stdin);
	struct mdea_emitter *e = mdea_file_emitter(STDOUT_FILENO);
	if (mdea_parse(t, e, &error) != 0)
		fprintf(stderr, "ERROR: %s\n", error);
	mdea_parser_destroy(t);
	mdea_emitter_destroy(e);
}

